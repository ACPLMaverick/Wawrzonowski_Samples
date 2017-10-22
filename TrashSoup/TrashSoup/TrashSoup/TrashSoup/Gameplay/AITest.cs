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
    class AITest : ObjectComponent
    {
        BehaviorTree myBT;
        Blackboard myBoard;

        public AITest(GameObject obj) : base(obj)
        {
            Start();
        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {

        }

        public override void Draw(Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, Microsoft.Xna.Framework.GameTime gameTime)
        {
            //Do nothing ;)
        }

        protected override void Start()
        {
            XmlSerializer serializer = new XmlSerializer(typeof(BehaviorTree));
            string path = "";
            if(TrashSoupGame.Instance != null && TrashSoupGame.Instance.EditorMode)
            {
                path += "../";
            }
            path += "../../../../TrashSoupContent/BehaviorTrees/Test.xml";
            try
            {
                using (FileStream file = new FileStream(Path.GetFullPath(path), FileMode.Open))
                {
                    myBT = (BehaviorTree)serializer.Deserialize(file);
                    myBoard = myBT.Blackboard;
                }
                myBT.Run();
            }
            catch
            {
                
            }
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
