using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;
using TrashSoup.Engine.AI.BehaviorTree;
using Microsoft.Xna.Framework;

namespace TrashSoup.Gameplay.RatAI
{
    class RatAttack : TrashSoup.Engine.AI.BehaviorTree.Action
    {
        private Vector3 targetPos;
        private Vector3 myPos;
        private float attackCooldown = 1.5f;
        private float timer = 0.0f;
        private float damage = 5.0f;
        private PlayerController target;
        private Vector3 difference;
        private float prevRotY;
        private float rotY;
        private bool firstTime = true;
        private Enemy myEnemy;

        public override void Initialize()
        {
            this.prevRotY = this.blackboard.Owner.MyTransform.Rotation.Y;
            GameObject go = ResourceManager.Instance.CurrentScene.GetObject(1);
            if(go != null)
            {
                target = (PlayerController)go.GetComponent<PlayerController>();
            }
            myEnemy = (Enemy)this.blackboard.Owner.GetComponent<Enemy>();
            base.Initialize();
        } 

        public override TickStatus Tick(GameTime gameTime, out INode node)
        {
            if (this.blackboard.GetBool("Dead"))
            {
                node = null;
                firstTime = true;
                return TickStatus.FAILURE;
            }

            if (!this.blackboard.GetBool("TargetSeen"))
            {
                firstTime = true;
                node = null;
                return TickStatus.FAILURE;
            }

            this.targetPos = this.blackboard.GetVector3("TargetPosition");
            this.myPos = this.blackboard.Owner.MyTransform.Position;
            float distance = Vector3.Distance(this.targetPos, this.myPos);

            if(distance > 3.5f)
            {
                firstTime = true;
                node = null;
                return TickStatus.FAILURE;
            }

            if(firstTime)
            {
                firstTime = false;
            }
            this.difference = this.targetPos - this.myPos;
            this.difference.Y = 0.0f;
            this.difference.Normalize();
            this.prevRotY = this.blackboard.Owner.MyTransform.Rotation.Y;
            this.rotY = (float)Math.Atan2(this.difference.Z, this.difference.X);
            this.rotY = this.CurveAngle(this.prevRotY, this.rotY, 0.1f);
            Vector3 rot = this.blackboard.Owner.MyTransform.Rotation;
            rot.Y = rotY;
            this.blackboard.Owner.MyTransform.Rotation = rot;

            if(timer > attackCooldown)
            {
                if(myEnemy != null)
                {
                    myEnemy.AttackTarget(damage);
                    //target.DecreaseHealth(this.damage);
                }
                timer = 0.0f;
            }

            timer += gameTime.ElapsedGameTime.Milliseconds * 0.001f;

            node = this;
            return TickStatus.RUNNING;
        }

        private float CurveAngle(float from, float to, float step)
        {
            if (step == 0) return from;
            if (from == to || step == 1) return to;

            Vector2 fromVector = new Vector2((float)Math.Cos(from), (float)Math.Sin(from));
            Vector2 toVector = new Vector2((float)Math.Cos(to), (float)Math.Sin(to));

            Vector2 currentVector = Slerp(fromVector, toVector, step);

            float toReturn = (float)Math.Atan2(currentVector.Y, currentVector.X);

            return toReturn;
        }

        private Vector2 Slerp(Vector2 from, Vector2 to, float step)
        {
            if (step == 0) return from;
            if (from == to || step == 1) return to;

            double dot = (double)Vector2.Dot(from, to);

            // clampin'!
            if (dot > 1) dot = 1;
            else if (dot < -1) dot = -1;

            double theta = Math.Acos(dot);
            if (theta == 0) return to;

            double sinTheta = Math.Sin(theta);

            Vector2 toReturn = (float)(Math.Sin((1 - step) * theta) / sinTheta) * from + (float)(Math.Sin(step * theta) / sinTheta) * to;

            if (float.IsNaN(toReturn.X) || float.IsNaN(toReturn.Y))
            {
                Debug.Log("RAT ERROR: NaN detected in Slerp()");
                throw new InvalidOperationException("RAT ERROR: NaN detected in Slerp()");
            }

            return toReturn;
        }
    }
}
