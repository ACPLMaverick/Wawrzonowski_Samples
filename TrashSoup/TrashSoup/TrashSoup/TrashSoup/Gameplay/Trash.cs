using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay
{
    public class Trash : ObjectComponent
    {
        #region variables

        public int TrashCount = 1;

        #endregion

        #region methods

        public Trash(GameObject go) : base(go)
        {

        }
        public Trash(GameObject go, Trash cm)
            : base(go, cm)
        {
            TrashCount = cm.TrashCount;
        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            
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
            go.MyTransform = new Transform(go, Vector3.Zero, Vector3.Forward, Vector3.Zero, this.MyObject.MyTransform.Scale + 2.0f);
            go.MyCollider = new BoxCollider(go, true);
            TrashTrigger tt = new TrashTrigger(go);
            tt.Init(null, this.TrashCount);
            go.Components.Add(tt);
            this.MyObject.AddChild(go);
            tt.Initialize();

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

        #endregion

    }
}
