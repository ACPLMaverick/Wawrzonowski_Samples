using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay
{
    public class MutantTrigger : ObjectComponent
    {
        private Mutant myMutant;
        private bool targetSeen = false;
        private bool targetDead = false;
        private GameObject target;
        private PlayerController pc;
        private Fortification f;

        public uint MyMutantID;

        public MutantTrigger(GameObject go) : base(go)
        {

        }

        public MutantTrigger(GameObject go, MutantTrigger rt)
            : base(go, rt)
        {
            this.myMutant = rt.myMutant;
            this.MyMutantID = rt.MyMutantID;
        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            if (targetDead) return;
            if(targetSeen && target != null)
            {
                myMutant.MyBlackBoard.SetVector3("TargetPosition", target.MyTransform.Position);

                if(pc != null)
                {
                    if(pc.IsDead)
                    {
                        myMutant.MyBlackBoard.SetBool("TargetSeen", false);
                        targetSeen = false;
                        targetDead = true;
                    }
                }
                else
                {
                    if(f != null)
                    {
                        if(f.CurrentHealth <= 0)
                        {
                            myMutant.MyBlackBoard.SetBool("TargetSeen", false);
                            target = null;
                            targetSeen = false;
                        }
                    }
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
            this.myMutant = (Mutant)ResourceManager.Instance.CurrentScene.GetObject(MyMutantID).GetComponent<Mutant>();
            this.myMutant.MyObject.AddChild(this.MyObject);
            this.MyObject.MyTransform.Position = new Vector3(5.0f, 0.0f, 0.0f);
            this.MyObject.MyCollider.IgnoredColliders.Add(this.myMutant.MyObject.MyCollider);
            this.MyObject.MyPhysicalObject = new PhysicalObject(this.MyObject, 0.0f, 0.0f, false);
            base.Initialize();
        }

        public override void OnTrigger(GameObject other)
        {
            if (targetDead) return;
            if (target != null && target.UniqueID != 1) return;
            bool fort = other.Tags.Contains("Fortification");
            if (other.UniqueID == 1 || fort)
            {
                myMutant.MyBlackBoard.SetBool("TargetSeen", true);
                targetSeen = true;
                target = other;
                myMutant.myEnemyScript.target = other;
                pc = (PlayerController)target.GetComponent<PlayerController>();
                if(fort)
                {
                    target = other.GetParent();
                    f = (Fortification)target.GetComponent<Fortification>();
                    if(f == null)
                    {
                        f = (Fortification)target.GetComponent<Fortification>();
                    }
                }
                Vector3 pos = target.MyTransform.Position;
                myMutant.MyBlackBoard.SetVector3("TargetPosition", pos);
            }
            base.OnTrigger(other);
        }

        public override void OnTriggerExit(GameObject other)
        {
            if (targetDead) return;
            if (other.UniqueID == 1)
            {
                myMutant.MyBlackBoard.SetBool("TargetSeen", false);
                targetSeen = false;
                target = null;
                myMutant.MyBlackBoard.SetVector3("TargetPosition", Vector3.Zero);
            }
            base.OnTriggerExit(other);
        }

        public override void ReadXml(System.Xml.XmlReader reader)
        {
            reader.ReadStartElement();

            reader.ReadStartElement("MyMutantID");
            this.MyMutantID = (uint)reader.ReadContentAsInt();
            reader.ReadEndElement();
            base.ReadXml(reader);
            

            reader.ReadEndElement();
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            writer.WriteStartElement("MyMutantID");
            writer.WriteValue(this.MyMutantID);
            writer.WriteEndElement();
            base.WriteXml(writer);
        }
    }
}
