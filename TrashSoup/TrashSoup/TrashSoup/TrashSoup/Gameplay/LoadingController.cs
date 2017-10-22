using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay
{
    class LoadingController : ObjectComponent
    {
        private bool first = true;
        private float ar = 9.0f / 16.0f;

        private Texture2D backgroundTexture;

        public LoadingController(GameObject go) : base(go)
        {

        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            GUIManager.Instance.DrawTexture(this.backgroundTexture, new Microsoft.Xna.Framework.Vector2(0.0f, 0.0f), 1.0f, this.ar);
            if(!first)
            {
                SaveManager.Instance.XmlPath = "../../../../TrashSoupContent/Scenes/save2.xml";
                SaveManager.Instance.LoadFileAction();
            }
            first = false;
        }

        public override void Draw(Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, Microsoft.Xna.Framework.GameTime gameTime)
        {
        }

        protected override void Start()
        {
        }

        public override void Initialize()
        {
            this.backgroundTexture = ResourceManager.Instance.LoadTexture(@"Textures/HUD/LoadingScreen");
            first = true;
            base.Initialize();
        }


        public override System.Xml.Schema.XmlSchema GetSchema()
        {
            return null;
        }

        public override void ReadXml(System.Xml.XmlReader reader)
        {
            reader.MoveToContent();
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
