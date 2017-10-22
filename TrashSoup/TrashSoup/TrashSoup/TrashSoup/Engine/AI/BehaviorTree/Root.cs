using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace TrashSoup.Engine.AI.BehaviorTree
{
    public class Root : INode, IXmlSerializable
    {
        private Blackboard blackboard;
        private INode child;

        public INode Child
        {
            get
            {
                return this.child;
            }
            set
            {
                this.child = value;
            }
        }

        public void Initialize()
        {
            if (this.child == null)
            {
                Console.WriteLine("Caution! The child of root is null");
            }
            else
            {
                this.child.Initialize();
            }
        }

        public TickStatus Tick(GameTime gameTime, out INode node)
        {
            node = null;
            return this.child.Tick(gameTime, out node);
        }

        public void SetBlackboard(Blackboard bb)
        {
            this.blackboard = bb;
            this.child.SetBlackboard(this.blackboard);
        }


        public void AddChild(INode node)
        {
            this.child = node;
            this.child.SetBlackboard(this.blackboard);
        }

        public System.Xml.Schema.XmlSchema GetSchema()
        {
            return null;
        }

        public void ReadXml(System.Xml.XmlReader reader)
        {
            reader.ReadStartElement();

            if (reader.Name == "Child")
            {
                reader.ReadStartElement();
                string s = reader.ReadElementString("Type", "");
                Object obj = Activator.CreateInstance(Type.GetType(s));
                (obj as IXmlSerializable).ReadXml(reader);
                AddChild((INode)obj);
                reader.ReadEndElement();
            }

            reader.ReadEndElement();
        }

        public void WriteXml(System.Xml.XmlWriter writer)
        {
            writer.WriteStartElement("Child");
            writer.WriteElementString("Type", child.GetType().ToString());
            (child as IXmlSerializable).WriteXml(writer);
            writer.WriteEndElement();
        }
    }
}
