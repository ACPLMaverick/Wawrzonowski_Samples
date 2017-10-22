using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay.Safehouse
{
    class MapSelectionController : ObjectComponent
    {
        private Texture2D background;
        private Texture2D interactionTexture;

        private Vector2 backgroundPos;
        private Vector2 interactionPos;

        private float ar = 9.0f / 16.0f;

        public MapSelectionController(GameObject go) : base(go)
        {

        }

        public MapSelectionController(GameObject go, MapSelectionController msc) : base(go)
        {

        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            GUIManager.Instance.DrawTexture(this.background, this.backgroundPos, 1.0f, this.ar);

            GUIManager.Instance.DrawTexture(this.interactionTexture, this.interactionPos, 0.05f, 0.05f);

            if(InputManager.Instance.GetGamePadButtonDown(Microsoft.Xna.Framework.Input.Buttons.A) || InputManager.Instance.GetKeyboardButtonDown(Microsoft.Xna.Framework.Input.Keys.Enter))
            {
                SaveManager.Instance.XmlPath = "../../../../TrashSoupContent/Scenes/loading.xml";
                SaveManager.Instance.LoadFileAction();
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
            background = TrashSoupGame.Instance.Content.Load<Texture2D>(@"Textures/HUD/map_menu");
            interactionTexture = TrashSoupGame.Instance.Content.Load<Texture2D>(@"Textures/HUD/a_button");

            backgroundPos = new Vector2(0.0f, 0.0f);
            interactionPos = new Vector2(0.45f, 0.9f);

            base.Initialize();
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
