using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework.Audio;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay
{
    public enum MenuStateEnum
    {
        MainMenu,
        Credits
    }

    class MenuController : ObjectComponent
    {
        private string[] names = new string[6]
        {
            "BELCZACKI JAN - DESIGN + CODE",
            "KOZLOWICZ WIKTORIA - 3D ARTIST",
            "ORDON KATARZYNA - 3D ARTIST",
            "STRABURZYNSKI KAROL - SOUND MASTER",
            "STENGERT KRZYSZTOF - CODE",
            "WAWRZONOWSKI MARCIN - CODE"
        };

        private Texture2D backgroundTexture;
        private Texture2D logoTexture;
        private SpriteFont font;

        //Main menu
        private Vector2 logoOnePos = new Vector2(0.125f, 0.32f);
        private Vector2 logoTwoPos = new Vector2(0.35f, 0.33f);
        private Vector2 newGamePos = new Vector2(0.205f, 0.43f);
        private Vector2 channelPos = new Vector2(0.11f, 0.31f);
        private Vector2 namePos = new Vector2(0.195f, 0.36f);
        private Vector2 continuePos = new Vector2(0.205f, 0.49f);
        private Vector2 creditsPos = new Vector2(0.205f, 0.55f);
        private Vector2 exitPos = new Vector2(0.205f, 0.61f);

        //Credits
        private Vector2 firstNamePos = new Vector2(0.145f, 0.445f);
        private Vector2 backPos = new Vector2(0.215f, 0.645f);
        private Vector2 creditsDollarOnePos = new Vector2(0.195f, 0.645f);
        private Vector2 creditsDollarTwoPos = new Vector2(0.275f, 0.645f);

        private float dollarOneXPos = 0.185f;
        private float dollarTwoXPos = 0.315f;
        private float dollarInitialYPos = 0.43f;
        private float currentDollarYPos = 0.43f;

        private float invertedAspectRatio = 9.0f / 16.0f;

        private int currentOption = 0;

        private Color activeColor = Color.Yellow;
        private Color normalColor = Color.Lime;

        private Cue bgCue;
        private Cue welcomeCue;
        //private bool bg;
        private bool welcome = true;

        private MenuStateEnum menuState = MenuStateEnum.MainMenu;

        public MenuController(GameObject go) : base(go)
        {

        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            if (welcome)
            {
                if (welcomeCue.IsStopped)
                {
                    bgCue.Play();
                    welcome = false;
                }
            }

            GUIManager.Instance.DrawTexture(this.backgroundTexture, Vector2.Zero, 1.0f, this.invertedAspectRatio);
            GUIManager.Instance.DrawText(this.font, "11 - DEATH TV", this.channelPos, this.normalColor);
            GUIManager.Instance.DrawTexture(this.logoTexture, this.logoOnePos, 0.05f, 0.07f);
            GUIManager.Instance.DrawTexture(this.logoTexture, this.logoTwoPos, 0.05f, 0.07f);
            GUIManager.Instance.DrawText(this.font, "TRASH SOUP", this.namePos, Color.Red);

            if(this.menuState == MenuStateEnum.MainMenu)
            {
                this.DrawMainMenu();
            }
            else if(this.menuState == MenuStateEnum.Credits)
            {
                this.DrawCredits();
            }
        }

        public override void Draw(Engine.Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, Microsoft.Xna.Framework.GameTime gameTime)
        {

        }

        public void DrawCredits()
        {
            if(InputManager.Instance.GetKeyboardButtonDown(Microsoft.Xna.Framework.Input.Keys.Enter) || InputManager.Instance.GetGamePadButtonDown(Microsoft.Xna.Framework.Input.Buttons.A))
            {
                this.menuState = MenuStateEnum.MainMenu;
            }
            GUIManager.Instance.DrawText(this.font, "BACK", this.backPos, Color.Yellow);
            GUIManager.Instance.DrawText(this.font, "$", this.creditsDollarOnePos, this.normalColor);
            GUIManager.Instance.DrawText(this.font, "$", this.creditsDollarTwoPos, this.normalColor);
            for(int i = 0; i < 6; ++i)
            {
                GUIManager.Instance.DrawText(this.font, this.names[i], this.firstNamePos + new Vector2(0.0f, i * 0.033f), this.normalColor, 0.55f);
            }
        }

        public void DrawMainMenu()
        {
            if (InputManager.Instance.GetKeyboardButtonDown(Microsoft.Xna.Framework.Input.Keys.S) || InputManager.Instance.GetGamePadButtonDown(Microsoft.Xna.Framework.Input.Buttons.DPadDown) || InputManager.Instance.GetGamePadButtonDown(Microsoft.Xna.Framework.Input.Buttons.LeftThumbstickDown) || InputManager.Instance.GetKeyboardButtonDown(Microsoft.Xna.Framework.Input.Keys.Down))
            {
                if (currentOption < 3)
                {
                    currentOption += 1;
                }
                else
                {
                    currentOption = 0;
                }

                if(currentOption == 1)
                {
                    currentOption += 1;
                }

                this.currentDollarYPos = this.dollarInitialYPos + 0.06f * this.currentOption;
            }

            if (InputManager.Instance.GetKeyboardButtonDown(Microsoft.Xna.Framework.Input.Keys.W) || InputManager.Instance.GetGamePadButtonDown(Microsoft.Xna.Framework.Input.Buttons.DPadUp) || InputManager.Instance.GetGamePadButtonDown(Microsoft.Xna.Framework.Input.Buttons.LeftThumbstickUp) || InputManager.Instance.GetKeyboardButtonDown(Microsoft.Xna.Framework.Input.Keys.Up))
            {
                if (currentOption > 0)
                {
                    currentOption -= 1;
                }
                else
                {
                    currentOption = 3;
                }

                if (currentOption == 1)
                {
                    currentOption -= 1;
                }

                this.currentDollarYPos = this.dollarInitialYPos + 0.06f * this.currentOption;
            }


            GUIManager.Instance.DrawText(this.font, "NEW GAME", this.newGamePos, this.currentOption == 0 ? this.activeColor : this.normalColor);
            GUIManager.Instance.DrawText(this.font, "CONTINUE", this.continuePos, Color.Gray);
            GUIManager.Instance.DrawText(this.font, "CREDITS", this.creditsPos, this.currentOption == 2 ? this.activeColor : this.normalColor);
            GUIManager.Instance.DrawText(this.font, "EXIT", this.exitPos, this.currentOption == 3 ? this.activeColor : this.normalColor);
            GUIManager.Instance.DrawText(this.font, "$", new Vector2(this.dollarOneXPos, this.currentDollarYPos), this.normalColor);
            GUIManager.Instance.DrawText(this.font, "$", new Vector2(this.dollarTwoXPos, this.currentDollarYPos), this.normalColor);

            if (InputManager.Instance.GetKeyboardButtonDown(Microsoft.Xna.Framework.Input.Keys.Enter) || InputManager.Instance.GetGamePadButtonDown(Microsoft.Xna.Framework.Input.Buttons.A))
            {
                switch (this.currentOption)
                {
                    case 0:
                        SaveManager.Instance.XmlPath = "../../../../TrashSoupContent/Scenes/loading.xml";
                        welcomeCue.Stop(AudioStopOptions.Immediate);
                        bgCue.Stop(AudioStopOptions.Immediate);
                        SaveManager.Instance.LoadFileAction();
                        break;
                    case 1:
                        SaveManager.Instance.XmlPath = "../../../../TrashSoupContent/Scenes/loading.xml";
                        SaveManager.Instance.LoadFileAction();
                        break;
                    case 2:
                        this.menuState = MenuStateEnum.Credits;
                        break;
                    case 3:
                        TrashSoupGame.Instance.Exit();
                        break;
                }
            }
        }

        protected override void Start()
        {

        }

        public override void Initialize()
        {
            this.backgroundTexture = ResourceManager.Instance.LoadTexture(@"Textures/HUD/menu");
            this.logoTexture = ResourceManager.Instance.LoadTexture(@"Textures/HUD/pixelatedLogo");
            this.font = TrashSoupGame.Instance.Content.Load<SpriteFont>("Fonts/manaspace");
            if (!TrashSoupGame.Instance.EditorMode)
            {
                bgCue = AudioManager.Instance.SoundBank.GetCue("menu");
                welcomeCue = AudioManager.Instance.SoundBank.GetCue("welcome");
                welcomeCue.Play();
            }
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
