using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;
using Microsoft.Xna.Framework.Media;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace TrashSoup.Gameplay
{
    class IntroPlayer : ObjectComponent
    {
        Video introVideo;
        VideoPlayer videoPlayer;
        Texture2D videoTexture;
        float ar = 9.0f / 16.0f;
        private Texture2D skipTexture;
        private Vector2 skipPosition;
        private float timer = 0.0f;
        private float textureWidth;
        private float textureHeight;

        public IntroPlayer(GameObject go) : base(go)
        {

        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            if(videoPlayer.State == MediaState.Stopped)
            {
                LoadNextScene();
            }
            else
            {
                videoTexture = videoPlayer.GetTexture();
                GUIManager.Instance.DrawTexture(videoTexture, Vector2.Zero, 1.0f, ar);
                if(timer < 1.5f)
                {
                    timer += gameTime.ElapsedGameTime.Milliseconds * 0.001f;
                    GUIManager.Instance.DrawTexture(skipTexture, skipPosition, textureWidth, textureHeight);
                }
            }

            if(InputManager.Instance.GetKeyboardButtonDown(Microsoft.Xna.Framework.Input.Keys.Enter) || InputManager.Instance.GetGamePadButtonDown(Microsoft.Xna.Framework.Input.Buttons.A))
            {
                LoadNextScene();
            }
        }

        private void LoadNextScene()
        {
            this.videoPlayer.Stop();
            SaveManager.Instance.XmlPath = "../../../../TrashSoupContent/Scenes/menu.xml";
            SaveManager.Instance.LoadFileAction();
        }

        public override void Draw(Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, Microsoft.Xna.Framework.GameTime gameTime)
        {
            
        }

        protected override void Start()
        {

        }

        public override void Initialize()
        {
            introVideo = TrashSoupGame.Instance.Content.Load<Video>("Videos/intro");
            videoPlayer = new VideoPlayer();
            videoPlayer.Play(introVideo);
            textureHeight = 0.05f;
            if(InputManager.Instance.IsGamePadConnected())
            {
                skipTexture = ResourceManager.Instance.LoadTexture("Textures/HUD/a_button");
                textureWidth = 0.05f;
            }
            else
            {
                skipTexture = ResourceManager.Instance.LoadTexture("Textures/HUD/enter_key");
                textureWidth = 0.1f;
            }
            skipPosition = new Vector2(0.8f, 0.9f);
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
