using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using TrashSoup.Engine;
using TrashSoup.Engine.AI.BehaviorTree;

namespace TrashSoup.Gameplay
{
    class Rat : ObjectComponent
    {
        private BehaviorTree myBehavior;
        public Blackboard MyBlackBoard;
        public Enemy myEnemyScript;

        private bool isDead = false;
        private float timer = 0.0f;
        private float dieCooldown = 0.0f;
        int i = 0;
        private float nextBlinkTime = 0.0f;

        public Rat(GameObject go) : base(go)
        {

        }

        public Rat(GameObject go, Rat r) : base(go, r)
        {

        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            if(TrashSoupGame.Instance.EditorMode)
            {
                return;
            }

            if(isDead)
            {
                timer += gameTime.ElapsedGameTime.Milliseconds * 0.001f;
                if (timer > dieCooldown)
                {
                    this.MyObject.MyAnimator.StopAnimation();
                }

                if(timer > nextBlinkTime)
                {
                    this.MyObject.Visible = !this.MyObject.Visible;
                    if(!this.MyObject.Visible)
                    {
                        nextBlinkTime += 0.3f / (float)(i+1);
                    }
                    else
                    {
                        nextBlinkTime += (1.0f / (float)(i + 1));
                        ++i;
                    }
                }

                if(timer > dieCooldown + 5.0f)
                {
                    this.MyObject.Enabled = false;
                }
            }
        }

        public override void Draw(Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, Microsoft.Xna.Framework.GameTime gameTime)
        {

        }

        protected override void Start()
        {

        }

        public override void Initialize()
        {
            this.MyObject.MyAnimator.AvailableStates.Add("Walk", new AnimatorState("Walk", this.MyObject.MyAnimator.GetAnimationPlayer("Animations/Enemies/Rat_walk")));
            this.MyObject.MyAnimator.AvailableStates.Add("Run", new AnimatorState("Run", this.MyObject.MyAnimator.GetAnimationPlayer("Animations/Enemies/Rat_run")));
            this.MyObject.MyAnimator.AvailableStates.Add("Attack", new AnimatorState("Attack", this.MyObject.MyAnimator.GetAnimationPlayer("Animations/Enemies/Rat_attack")));
            this.MyObject.MyAnimator.AvailableStates.Add("Die", new AnimatorState("Die", this.MyObject.MyAnimator.GetAnimationPlayer("Animations/Enemies/Rat_dying")));
            this.MyObject.MyAnimator.AvailableStates.Add("Idle", new AnimatorState("Idle", this.MyObject.MyAnimator.GetAnimationPlayer("Animations/Enemies/Rat_idle")));
            string[] states = new string[] { "Idle", "Walk", "Run", "Die", "Attack" };
            for (int i = 0; i < states.Length; ++i)
            {
                for (int j = 0; j < states.Length; ++j)
                {
                    if (j != i)
                    {
                        this.MyObject.MyAnimator.AvailableStates[states[i]].AddTransition(this.MyObject.MyAnimator.AvailableStates[states[j]], new TimeSpan(0, 0, 0, 0, 200));
                    }
                }
            }
            MyObject.MyAnimator.CurrentState = MyObject.MyAnimator.AvailableStates["Idle"];
            
            XmlSerializer serializer = new XmlSerializer(typeof(BehaviorTree));
            string path = "";
            if (TrashSoupGame.Instance != null && TrashSoupGame.Instance.EditorMode)
            {
                path += "../";
            }
            path += "../../../../TrashSoupContent/BehaviorTrees/RatAI" + this.MyObject.UniqueID.ToString()+".xml";
            try
            {
                using (FileStream file = new FileStream(Path.GetFullPath(path), FileMode.Open))
                {
                    myBehavior = (BehaviorTree)serializer.Deserialize(file);
                    MyBlackBoard = myBehavior.Blackboard;
                }
                myBehavior.Run();
                this.MyBlackBoard.SetBool("Dead", false);
            }
            catch
            {

            }
            this.myEnemyScript = (Enemy)this.MyObject.GetComponent<Enemy>();
            this.myEnemyScript.OnDead += this.OnDead;
            this.myEnemyScript.ArmorPoints = 0;
            this.MyObject.MyCollider.Layer = LayerEnum.ENEMY;
            base.Initialize();
        }

        void OnDead()
        {
            this.myBehavior.Stop();
            this.MyBlackBoard.SetBool("Dead", true);
            this.MyObject.MyAnimator.ChangeState("Die");
            dieCooldown = (float)this.MyObject.MyAnimator.AvailableStates["Die"].Animation.GetDuration().TotalSeconds;
            nextBlinkTime = dieCooldown + 1.0f;
            this.MyObject.MyPhysicalObject.Velocity = Vector3.Zero;
            this.MyObject.MyPhysicalObject.ZeroForce();
            this.MyObject.MyPhysicalObject.IsUsingGravity = false;
            //this.MyObject.MyCollider.Enabled = false;
            Collider c = ResourceManager.Instance.CurrentScene.GetObject(1).MyCollider;
            this.MyObject.MyCollider.IgnoredColliders.Add(c);
            c.IgnoredColliders.Add(this.MyObject.MyCollider);
            isDead = true;
        }

        public override void OnCollision(GameObject other)
        {
            base.OnCollision(other);
        }

        public override void ReadXml(System.Xml.XmlReader reader)
        {
            reader.ReadStartElement();

            base.ReadXml(reader);

            reader.ReadEndElement();
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            base.WriteXml(writer);
        }
    }
}
