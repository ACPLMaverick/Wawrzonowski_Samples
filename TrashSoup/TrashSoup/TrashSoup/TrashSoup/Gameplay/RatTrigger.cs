using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay
{
    public class RatTrigger : ObjectComponent
    {
        private Rat myRat;
        private bool targetSeen = false;
        private bool targetDead = false;
        private GameObject target;
        private PlayerController pc;

        public uint MyRatID;

        public RatTrigger(GameObject go) : base(go)
        {

        }

        public RatTrigger(GameObject go, RatTrigger rt) : base(go, rt)
        {
            this.myRat = rt.myRat;
            this.MyRatID = rt.MyRatID;
        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            if (targetDead) return;
            if(targetSeen && target != null)
            {
                myRat.MyBlackBoard.SetVector3("TargetPosition", target.MyTransform.Position);

                if(pc != null)
                {
                    if(pc.IsDead)
                    {
                        myRat.MyBlackBoard.SetBool("TargetSeen", false);
                        targetSeen = false;
                        targetDead = true;
                    }
                }
                else if(!target.Enabled)
                {
                    myRat.MyBlackBoard.SetBool("TargetSeen", false);
                    target = null;
                    targetSeen = false;
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
            this.myRat = (Rat)ResourceManager.Instance.CurrentScene.GetObject(MyRatID).GetComponent<Rat>();
            this.myRat.MyObject.AddChild(this.MyObject);
            this.MyObject.MyCollider.IgnoredColliders.Add(this.myRat.MyObject.MyCollider);
            base.Initialize();
        }

        public override void OnTrigger(GameObject other)
        {
            if (targetDead) return;
            if (other.UniqueID == 1 || other.Tags.Contains("Fortification"))
            {
                myRat.MyBlackBoard.SetBool("TargetSeen", true);
                targetSeen = true;
                target = other;
                myRat.myEnemyScript.target = other;
                pc = (PlayerController)target.GetComponent<PlayerController>();
                myRat.MyBlackBoard.SetVector3("TargetPosition", other.MyTransform.Position);
            }
            base.OnTrigger(other);
        }

        public override void OnTriggerExit(GameObject other)
        {
            if (targetDead) return;
            if (other.UniqueID == 1)
            {
                myRat.MyBlackBoard.SetBool("TargetSeen", false);
                targetSeen = false;
                target = null;
                myRat.MyBlackBoard.SetVector3("TargetPosition", Vector3.Zero);
            }
            base.OnTriggerExit(other);
        }

        public override void ReadXml(System.Xml.XmlReader reader)
        {
            reader.ReadStartElement();

            reader.ReadStartElement("MyRatID");
            this.MyRatID = (uint)reader.ReadContentAsInt();
            reader.ReadEndElement();
            base.ReadXml(reader);
            

            reader.ReadEndElement();
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            writer.WriteStartElement("MyRatID");
            writer.WriteValue(this.MyRatID);
            writer.WriteEndElement();
            base.WriteXml(writer);
        }
    }
}
