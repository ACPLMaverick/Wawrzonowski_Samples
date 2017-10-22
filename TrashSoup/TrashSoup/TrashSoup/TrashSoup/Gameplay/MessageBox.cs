using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;

namespace TrashSoup.Gameplay
{
    public class MessageBox : GUIElement
    {
        #region constants

        private static uint MAX_OPTIONS = 3;

        #endregion

        #region variables

        private static Vector2 offsetText = new Vector2(0.07f, 0.3f);
        private static Vector2[] offsetsOptionTexts = 
        {
            new Vector2(0.12f, 1.26f),
            new Vector2(0.44f, 1.26f),
            new Vector2(0.75f, 1.26f)
        };
        private static Vector2[] offsetsOptionButtons = 
        {
            new Vector2(0.06f, 1.2f),
            new Vector2(0.37f, 1.2f),
            new Vector2(0.68f, 1.2f)
        };
        private static string[] buttonsTexturePaths =
        {
            "Textures/HUD/x_button",
            "Textures/HUD/b_button",
            "Textures/HUD/y_button"
        };
        private static string backgroundTexturePath = "Textures/HUD/window_red2_0";
        private static string fontPath = "Fonts/manaspace";
        private static float textScale = 1.0f;
        private static float buttonTextScale = 0.7f;

        private bool initialized = false;

        private bool active;
        private string text;
        private uint optionCount = 2;
        private string[] optionTexts;

        private GUIText guiText;
        private GUIText[] guiOptionTexts;
        private GUITexture[] guiButtonTextures;
        private GUITexture guiBackgroundTexture;

        #endregion

        #region properties

        public string Text 
        { 
            get
            {
                return text;
            }
            set
            {
                if (initialized)
                {
                    throw new InvalidOperationException("MessageBox: Property changed after initialization!");
                }

                text = value;
            }
        }
        public uint OptionCount 
        { 
            get
            {
                return optionCount;
            }
            set
            {
                if(initialized)
                {
                    throw new InvalidOperationException("MessageBox: Property changed after initialization!");
                }
                if(value > MAX_OPTIONS)
                {
                    throw new InvalidOperationException("MessageBox: OptionCount given higher than MAX_OPTIONS = " + MAX_OPTIONS.ToString());
                }
                else if(value == 0)
                {
                    throw new InvalidOperationException("MessageBox: OptionCount given cannot equal zero!");
                }
                optionCount = value;
            }
        }
        public string[] OptionTexts
        {
            get
            {
                return optionTexts;
            }
            set
            {
                if (initialized)
                {
                    throw new InvalidOperationException("MessageBox: Property changed after initialization!");
                }

                int count = value.Count();

                if (count > OptionCount)
                {
                    throw new InvalidOperationException("MessageBox: More texts than options!");
                }
                else if(count < OptionCount)
                {
                    string[] cValue = new string[OptionCount];
                    for(int i = 0; i < OptionCount; ++i)
                    {
                        if (i < count)
                            cValue[i] = value[i];
                        else
                            cValue[i] = "<<EMPTY>>";
                    }
                    optionTexts = cValue;
                }
                else
                {
                    optionTexts = value;
                }
            }
        }

        /// <summary>
        /// Setting this property to true resets response!
        /// </summary>
        public bool Active 
        { 
            get
            {
                return active;
            }
            set
            {
                active = value;
                if(active == true)
                {
                    Response = 0;
                }
            }
        }
        public uint Response { get; private set; }

        #endregion

        #region methods

        public MessageBox(Vector2 pos, float width, float height) : base(pos, width, height)
        {

        }

        public override void Update(GameTime gameTime)
        {
            if(Active)
            {
                if(OptionCount >= 1 && InputHandler.Instance.PromptOption1())
                {
                    Response = 1;
                }
                else if (OptionCount >= 2 && InputHandler.Instance.PromptOption2())
                {
                    Response = 2;
                }
                else if (OptionCount >= 3 && InputHandler.Instance.PromptOption3())
                {
                    Response = 3;
                }
            }

            base.Update(gameTime);
        }

        public override void Draw(Microsoft.Xna.Framework.Graphics.SpriteBatch spriteBatch)
        {
            if(Active)
            {
                GUIManager.Instance.DrawTexture(guiBackgroundTexture);
                GUIManager.Instance.DrawText(guiText);
                for (int i = 0; i < OptionCount; ++i)
                {
                    GUIManager.Instance.DrawTexture(guiButtonTextures[i]);
                    GUIManager.Instance.DrawText(guiOptionTexts[i]);
                }

                base.Draw(spriteBatch);
            }
        }

        public void Initialize()
        {
            guiText = new GUIText(ResourceManager.Instance.LoadFont(fontPath), Text, Color.GreenYellow,
                this.relativePosition + new Vector2(offsetText.X * this.width, offsetText.Y * this.height), textScale);

            guiOptionTexts = new GUIText[OptionCount];
            for (int i = 0; i < OptionCount; ++i )
            {
                guiOptionTexts[i] = new GUIText(ResourceManager.Instance.LoadFont(fontPath), OptionTexts[i], Color.AntiqueWhite,
                    this.relativePosition + new Vector2(offsetsOptionTexts[i].X * this.width, offsetsOptionTexts[i].Y * this.height), buttonTextScale);
            }

            guiButtonTextures = new GUITexture[OptionCount];
            for (int i = 0; i < OptionCount; ++i )
            {
                guiButtonTextures[i] = new GUITexture(ResourceManager.Instance.LoadTexture(buttonsTexturePaths[i]), 
                    this.relativePosition + new Vector2(offsetsOptionButtons[i].X * this.width, offsetsOptionButtons[i].Y * this.height), 0.03f, 0.03f);
            }

            guiBackgroundTexture = new GUITexture(ResourceManager.Instance.LoadTexture(backgroundTexturePath), this.myRectangle);

            Active = false;
            Response = 0;

            initialized = true;
        }

        #endregion
    }
}
