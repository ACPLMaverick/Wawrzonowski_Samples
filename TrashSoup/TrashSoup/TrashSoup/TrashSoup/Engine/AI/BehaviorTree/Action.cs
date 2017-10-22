using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace TrashSoup.Engine.AI.BehaviorTree
{
    public class Action : INode, IXmlSerializable
    {
        protected Blackboard blackboard;

        public virtual void Initialize()
        {

        }

        public virtual TickStatus Tick(GameTime gameTime, out INode node)
        {
            node = null;
            return TickStatus.FAILURE;
        }

        public void SetBlackboard(Blackboard bb)
        {
            this.blackboard = bb;
        }

        public void AddChild(INode node)
        {

        }

        public System.Xml.Schema.XmlSchema GetSchema()
        {
            return null;
        }

        public void ReadXml(System.Xml.XmlReader reader)
        {

        }

        public void WriteXml(System.Xml.XmlWriter writer)
        {

        }
    }
}
