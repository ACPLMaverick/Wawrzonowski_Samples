using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using TrashSoup.Engine;
using System.Xml.Schema;
using System.Xml;

namespace TrashSoup.Gameplay
{
    class JournalView : ObjectComponent
    {
        private Texture2D noteTexture;
        private Vector2 noteTexturePos = new Vector2(0.25f, 0.0f);

        public JournalView(GameObject go) :base(go)
        {
            GameManager.Instance.MovementEnabled = false;
            Start();
        }

        public override void Update(GameTime gameTime)
        {
            GUIManager.Instance.DrawTexture(this.noteTexture, this.noteTexturePos, 0.5f, 0.5f);
            if(InputManager.Instance.GetKeyboardButtonDown(Keys.Enter) || InputManager.Instance.GetGamePadButtonDown(Buttons.A))
            {
                GameManager.Instance.MovementEnabled = true;
                this.MyObject.Enabled = false;
            }
        }

        public override void Draw(Camera cam, Effect effect, GameTime gameTime)
        {

        }

        protected override void Start()
        {
            
        }

        public override void Initialize()
        {
            this.noteTexture = ResourceManager.Instance.LoadTexture(@"Textures/Storyline/note");

            base.Initialize();
        }

        public override XmlSchema GetSchema()
        {
            return base.GetSchema();
        }

        public override void ReadXml(XmlReader reader)
        {
            reader.MoveToContent();
            reader.ReadStartElement();

            base.ReadXml(reader);

            reader.ReadEndElement();
        }

        public override void WriteXml(XmlWriter writer)
        {
            base.WriteXml(writer);
        }
    }
}
