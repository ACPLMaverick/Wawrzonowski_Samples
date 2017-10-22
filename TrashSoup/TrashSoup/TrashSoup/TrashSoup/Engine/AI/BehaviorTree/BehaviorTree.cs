using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace TrashSoup.Engine.AI.BehaviorTree
{
    [XmlRoot("BehaviorTree")]
    public class BehaviorTree : IXmlSerializable
    {
        public INode CurrentRunning = null;

        public Blackboard Blackboard
        {
            get;
            set;
        }

        public Root BTRoot
        {
            get;
            set;
        }

        public double MilisecondsTillLastTick
        {
            get;
            set;
        }

        public bool Enabled = false;

        public BehaviorTree()
        {
            this.Blackboard = null;
            this.MilisecondsTillLastTick = 0;
            BehaviorTreeManager.Instance.AddBehaviorTree(this);
        }

        public BehaviorTree(Blackboard b)
        {
            this.Blackboard = b;
            this.MilisecondsTillLastTick = 0;
            BehaviorTreeManager.Instance.AddBehaviorTree(this);
        }

        public BehaviorTree(Blackboard b, Root btRoot)
        {
            this.Blackboard = b;
            this.BTRoot = btRoot;
            this.BTRoot.SetBlackboard(this.Blackboard);
            this.MilisecondsTillLastTick = 0;
            BehaviorTreeManager.Instance.AddBehaviorTree(this);
        }

        public void SetRoot(Root root)
        {
            this.BTRoot = root;
            this.BTRoot.SetBlackboard(this.Blackboard);
        }

        public void Run()
        {
            this.Enabled = true;
            this.MilisecondsTillLastTick = 0;
            if(this.BTRoot != null)
            {
                this.BTRoot.Initialize();
            }
        }

        public void Stop()
        {
            this.Enabled = false;
        }

        public TickStatus Tick(GameTime gameTime)
        {
            return this.BTRoot.Tick(gameTime, out this.CurrentRunning);
        }

        public System.Xml.Schema.XmlSchema GetSchema()
        {
            return null;
        }

        public void ReadXml(System.Xml.XmlReader reader)
        {
            reader.MoveToContent();
            reader.ReadStartElement();

            int ownerID = reader.ReadElementContentAsInt("OwnerID", "");
            GameObject owner = ResourceManager.Instance.CurrentScene.GetObject((uint)ownerID);

            Blackboard = new Blackboard(owner);
            (Blackboard as IXmlSerializable).ReadXml(reader);

            BTRoot = new Root();
            (BTRoot as IXmlSerializable).ReadXml(reader);
            BTRoot.SetBlackboard(Blackboard);

            reader.ReadEndElement();
        }

        public void WriteXml(System.Xml.XmlWriter writer)
        {
            writer.WriteElementString("OwnerID", Blackboard.Owner.UniqueID.ToString());

            writer.WriteStartElement("Blackboard");
            (Blackboard as IXmlSerializable).WriteXml(writer);
            writer.WriteEndElement();

            writer.WriteStartElement("BTRoot");
            (BTRoot as IXmlSerializable).WriteXml(writer);
            writer.WriteEndElement();
        }
    }
}
