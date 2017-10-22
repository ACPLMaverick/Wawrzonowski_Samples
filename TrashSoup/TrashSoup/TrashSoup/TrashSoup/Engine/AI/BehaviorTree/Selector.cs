using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace TrashSoup.Engine.AI.BehaviorTree
{
    public class Selector : INode, IXmlSerializable
    {
        private Blackboard blackboard;
        private List<INode> children = new List<INode>();

        public List<INode> Children
        {
            get { return this.children; }
            set { this.children = value; }
        }

        public void Initialize()
        {
            foreach(INode child in children)
            {
                child.Initialize();
            }
        }

        public TickStatus Tick(GameTime gameTime, out INode node)
        {
            node = null;
            if (children.Count == 0) return TickStatus.FAILURE;
            foreach (INode child in children)
            {
                TickStatus tickStatus = child.Tick(gameTime, out node);
                if (tickStatus != TickStatus.FAILURE)
                {
                    return tickStatus;
                }
            }

            return TickStatus.FAILURE;
        }

        public void SetBlackboard(Blackboard bb)
        {
            this.blackboard = bb;
            foreach (INode child in children)
            {
                child.SetBlackboard(this.blackboard);
            }
        }


        public void AddChild(INode node)
        {
            node.SetBlackboard(this.blackboard);
            this.children.Add(node);
        }

        public System.Xml.Schema.XmlSchema GetSchema()
        {
            return null;
        }

        public void ReadXml(System.Xml.XmlReader reader)
        {
            if (reader.Name == "Children")
            {
                reader.ReadStartElement();
                while (reader.NodeType != System.Xml.XmlNodeType.EndElement)
                {
                    string s = reader.ReadElementString("Type", "");
                    Object obj = Activator.CreateInstance(Type.GetType(s));
                    (obj as IXmlSerializable).ReadXml(reader);
                    AddChild((INode)obj);
                }
                reader.ReadEndElement();
            }
        }

        public void WriteXml(System.Xml.XmlWriter writer)
        {
            writer.WriteStartElement("Children");
            foreach (INode child in children)
            {
                writer.WriteElementString("Type", child.GetType().ToString());
                (child as IXmlSerializable).WriteXml(writer);
            }
            writer.WriteEndElement();
        }
    }
}
