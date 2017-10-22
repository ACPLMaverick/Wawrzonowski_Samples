using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;
using TrashSoup.Engine.AI.BehaviorTree;

namespace TrashSoup.Gameplay.RatAI
{
    class RatChase : TrashSoup.Engine.AI.BehaviorTree.Action
    {
        private Vector3 targetPos;
        private Vector3 myPos;
        private float chaseSpeed = 6.0f;
        private Vector3 chaseVector = Vector3.Zero;
        private float prevRotY;
        private float rotY;
        private bool firstTime = true;

        public override void Initialize()
        {
            this.prevRotY = this.blackboard.Owner.MyTransform.Rotation.Y;
            base.Initialize();
        }

        public override TickStatus Tick(Microsoft.Xna.Framework.GameTime gameTime, out INode node)
        {
            if (this.blackboard.GetBool("Dead"))
            {
                node = null;
                firstTime = true;
                return TickStatus.FAILURE;
            }

            if (!this.blackboard.GetBool("TargetSeen"))
            {
                this.blackboard.SetBool("TargetSeen", false);
                firstTime = true;
                node = null;
                return TickStatus.FAILURE;
            }

            this.targetPos = this.blackboard.GetVector3("TargetPosition");
            this.myPos = this.blackboard.Owner.MyTransform.Position;
            float distance = Vector3.Distance(this.myPos, this.targetPos);

            if(distance < 3.0f)
            {
                node = null;
                if(!firstTime)
                {
                    this.blackboard.Owner.MyAnimator.ChangeState("Attack");
                    Debug.Log("Hello");
                }
                firstTime = true;
                return TickStatus.SUCCESS;
            }

            if (firstTime)
            {
                firstTime = false;
                this.blackboard.Owner.MyAnimator.StopAnimation();
                this.blackboard.Owner.MyAnimator.ChangeState("Run");
            }
            
            this.chaseVector = this.targetPos - this.myPos;
            this.chaseVector.Y = 0.0f;
            this.chaseVector.Normalize();
            this.prevRotY = this.blackboard.Owner.MyTransform.Rotation.Y;
            this.rotY = (float)Math.Atan2(this.chaseVector.Z, this.chaseVector.X);
            this.rotY = this.CurveAngle(this.prevRotY, this.rotY, 0.1f);
            Vector3 rot = this.blackboard.Owner.MyTransform.Rotation;
            rot.Y = rotY;
            this.blackboard.Owner.MyTransform.Rotation = rot;

            this.blackboard.Owner.MyTransform.Position += this.chaseVector * gameTime.ElapsedGameTime.Milliseconds * 0.001f * chaseSpeed;
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
                Debug.Log("PLAYERCONTROLLER ERROR: NaN detected in Slerp()");
                throw new InvalidOperationException("PLAYERCONTROLLER ERROR: NaN detected in Slerp()");
            }

            return toReturn;
        }
    }
}
