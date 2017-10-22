using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay
{
    public class DisappearingTrash : ObjectComponent
    {
        public int TrashCount = 1;

        private CustomModel modelGood;
        private CustomModel modelBad;
        private TrashTrigger tt;

        public DisappearingTrash(GameObject go) : base(go)
        {

        }

        public DisappearingTrash(GameObject go, DisappearingTrash cm) : base(go, cm)
        {
            TrashCount = cm.TrashCount;
        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            if(!TrashSoupGame.Instance.EditorMode)
            {
                if (tt.Picked && this.Enabled)
                {
                    if (modelBad != null)
                    {
                        modelBad.Visible = true;
                        modelGood.Visible = false;
                    }
                    else
                    {
                        MyObject.Enabled = false;
                        ResourceManager.Instance.CurrentScene.DeleteObjectRuntime(MyObject);
                    }
                    this.Enabled = false;
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
            GameObject go = new GameObject((uint)SingleRandom.Instance.rnd.Next() + 105012, "MyTrigger");
            go.MyTransform = new Transform(go, this.MyObject.MyTransform.Position, Vector3.Forward, Vector3.Zero, this.MyObject.MyTransform.Scale + 3.0f);
            tt = new TrashTrigger(go);
            go.MyCollider = new BoxCollider(go, true);
            tt.Init(this.MyObject, this.TrashCount);
            go.Components.Add(tt);
            this.MyObject.AddChild(go);
            tt.Initialize();
            //ResourceManager.Instance.CurrentScene.AddObjectRuntime(go);

            bool second = false;
            foreach (ObjectComponent comp in MyObject.Components)
            {
                if (comp.GetType() == typeof(CustomModel))
                {
                    if (!second)
                    {
                        modelGood = (CustomModel)comp;
                        second = true;
                    }
                    else
                    {
                        modelBad = (CustomModel)comp;
                        break;
                    }
                }
            }

            if (modelBad != null)
            {
                modelBad.Visible = false;
            }

            base.Initialize();
        }


        public override void ReadXml(System.Xml.XmlReader reader)
        {
            reader.ReadStartElement();

            reader.ReadStartElement("TrashCount");
            this.TrashCount = reader.ReadContentAsInt();
            reader.ReadEndElement();
            base.ReadXml(reader);

            reader.ReadEndElement();
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            writer.WriteStartElement("TrashCount");
            writer.WriteValue(this.TrashCount);
            writer.WriteEndElement();
            base.WriteXml(writer);
        }
    }
}
