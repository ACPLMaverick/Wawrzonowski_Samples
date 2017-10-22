using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay
{
    class ControlsScreenController : ObjectComponent
    {
        private Texture2D controlsScreen;
        private Vector2 texturePosition;
        private float ar = 9.0f / 16.0f;

        public ControlsScreenController(GameObject go) : base(go)
        {

        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            GUIManager.Instance.DrawTexture(this.controlsScreen, this.texturePosition, 1.0f, this.ar);
            if(InputManager.Instance.GetGamePadButtonDown(Microsoft.Xna.Framework.Input.Buttons.A) || InputManager.Instance.GetKeyboardButtonDown(Microsoft.Xna.Framework.Input.Keys.Enter))
            {
                SaveManager.Instance.XmlPath = "../../../../TrashSoupContent/Scenes/intro.xml";
                SaveManager.Instance.LoadFileAction();
            }
        }

        public override void Draw(Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, Microsoft.Xna.Framework.GameTime gameTime)
        {

        }

        public override void Initialize()
        {
            this.controlsScreen = ResourceManager.Instance.LoadTexture("Textures/HUD/controls_screen");
            this.texturePosition = Vector2.Zero;

            base.Initialize();
        }

        protected override void Start()
        {

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
