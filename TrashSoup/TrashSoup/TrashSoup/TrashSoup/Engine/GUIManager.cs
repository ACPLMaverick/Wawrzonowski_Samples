using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace TrashSoup.Engine
{
    #region ButtonState enum

    /// <summary>
    /// 
    /// Enum that helps GUIButton keep info about his state
    /// </summary>
    public enum GUIButtonState
    {
        NORMAL = 0,
        HOVER,
        PRESSED,
        DISABLED
    }

    #endregion

    #region ButtonPressedDelegate

     public delegate void ButtonPressedCallback();

     #endregion

    #region GUIElement class

    public class GUIElement
    {
        #region Variables

        protected Vector2 position;
        protected Vector2 relativePosition;
        protected float width;
        protected float height;

        protected Rectangle myRectangle;

        #endregion

        #region Methods

        public GUIElement(Vector2 pos, float width, float height)
        {
            this.relativePosition = pos;
            this.position = new Vector2(pos.X * TrashSoupGame.Instance.Window.ClientBounds.Width, pos.Y * TrashSoupGame.Instance.Window.ClientBounds.Height);
            this.width = width;
            this.height = height;

            this.myRectangle = new Rectangle((int)this.position.X, (int)this.position.Y, (int)(this.width * TrashSoupGame.Instance.Window.ClientBounds.Width),
                (int)(this.height * TrashSoupGame.Instance.Window.ClientBounds.Width));
        }

        public GUIElement(Rectangle rect)
        {
            this.position = new Vector2(rect.X, rect.Y);
            this.width = rect.Width;
            this.height = rect.Height;

            this.myRectangle = rect;
        }

        #endregion

        #region Methods

        public virtual void Update(GameTime gameTime)
        {

        }

        public virtual void Draw(SpriteBatch spriteBatch)
        {

        }

        #endregion
    }

    #endregion

    #region GUITexture class

    public class GUITexture : GUIElement
    {
        #region Variables

        private Texture2D texture;

        #endregion

        #region Methods

        public GUITexture(Texture2D texture, Rectangle rect)
            : base(rect)
        {
            this.texture = texture;
        }

        public GUITexture(Texture2D texture, Vector2 position, float width, float height)
            : base(position, width, height)
        {
            this.texture = texture;
        }

        public override void Draw(SpriteBatch spriteBatch)
        {
            if(this.texture == null)
            {
                Debug.Log("Some texture is null :(");
                return;
            }
            //spriteBatch.Draw(this.texture, this.position, Color.White);
            spriteBatch.Draw(this.texture, this.myRectangle, Color.White);
            base.Draw(spriteBatch);
        }

        #endregion
    }

    #endregion

    #region GUIText class

    public class GUIText : GUIElement
    {
        #region Variables

        private SpriteFont font;
        private string text;
        private Color color;
        private float scale;

        #endregion

        #region Methods

        public GUIText(SpriteFont font, string text, Color color, Vector2 position)
            : base(position, 0.0f, 0.0f)
        {
            this.font = font;
            this.text = text;
            this.color = color;
            this.scale = 1.0f;
        }

        public GUIText(SpriteFont font, string text, Color color, Vector2 position, float scale) : base(position, 0.0f, 0.0f)
        {
            this.font = font;
            this.text = text;
            this.color = color;
            this.scale = scale;
        }

        public override void Draw(SpriteBatch spriteBatch)
        {
            spriteBatch.DrawString(this.font, this.text, this.position, this.color, 0.0f, Vector2.Zero, scale, SpriteEffects.None, 1);
            base.Draw(spriteBatch);
        }

        #endregion
    }

    #endregion

    #region GUIButton class

    public class GUIButton : GUIElement
    {
        #region Variables

        private Texture2D currentTexture;

        private Texture2D normalTexture;
        private Texture2D hoverTexture;
        private Texture2D pressedTexture;
        private Texture2D disabledTexture;

        private GUIButtonState myState = GUIButtonState.NORMAL;
        private ButtonPressedCallback callbackEvent;

        #endregion

        #region Methods

        /// <summary>
        /// 
        /// Constructor of GUIButton
        /// </summary>
        /// <param name="callbackEvent">Function that will be called when button is pressed</param>
        public GUIButton(Texture2D normal, Texture2D hover, Texture2D pressed, Texture2D disabled, ButtonPressedCallback callbackEvent, Vector2 position, float width, float height)
            : base(position, width, height)
        {
            this.normalTexture = normal;
            this.hoverTexture = hover;
            this.pressedTexture = pressed;
            this.disabledTexture = disabled;

            this.currentTexture = this.normalTexture;

            this.callbackEvent += callbackEvent;
        }

        /// <summary>
        /// 
        /// Constructor of GUIButton
        /// </summary>
        /// <param name="callbackEvent">Function that will be called when button is pressed</param>
        public GUIButton(Texture2D normal, Texture2D hover, Texture2D pressed, Texture2D disabled, ButtonPressedCallback callbackEvent, Rectangle rect)
            : base(rect)
        {
            this.normalTexture = normal;
            this.hoverTexture = hover;
            this.pressedTexture = pressed;
            this.disabledTexture = disabled;

            this.currentTexture = this.normalTexture;

            this.callbackEvent += callbackEvent;
        }

        /// <summary>
        /// 
        /// Constructor of GUIButton
        /// </summary>
        /// <param name="callbackEvent">Function that will be called when button is pressed</param>
        public GUIButton(Texture2D normal, Texture2D hover, Texture2D pressed, Texture2D disabled, ButtonPressedCallback callbackEvent, GUIButtonState startState, Vector2 position, float width, float height)
            : base(position, width, height)
        {
            this.normalTexture = normal;
            this.hoverTexture = hover;
            this.pressedTexture = pressed;
            this.disabledTexture = disabled;

            this.myState = startState;

            switch(this.myState)
            {
                case GUIButtonState.DISABLED:
                    this.currentTexture = this.disabledTexture;
                    break;
                case GUIButtonState.HOVER:
                    this.currentTexture = this.hoverTexture;
                    break;
                case GUIButtonState.NORMAL:
                    this.currentTexture = this.normalTexture;
                    break;
                case GUIButtonState.PRESSED:
                    this.currentTexture = this.pressedTexture;
                    break;
            }

            this.callbackEvent += callbackEvent;
        }

        /// <summary>
        /// 
        /// Constructor of GUIButton
        /// </summary>
        /// <param name="callbackEvent">Function that will be called when button is pressed</param>
        public GUIButton(Texture2D normal, Texture2D hover, Texture2D pressed, Texture2D disabled, ButtonPressedCallback callbackEvent, GUIButtonState startState, Rectangle rect)
            : base(rect)
        {
            this.normalTexture = normal;
            this.hoverTexture = hover;
            this.pressedTexture = pressed;
            this.disabledTexture = disabled;

            this.myState = startState;

            switch (this.myState)
            {
                case GUIButtonState.DISABLED:
                    this.currentTexture = this.disabledTexture;
                    break;
                case GUIButtonState.HOVER:
                    this.currentTexture = this.hoverTexture;
                    break;
                case GUIButtonState.NORMAL:
                    this.currentTexture = this.normalTexture;
                    break;
                case GUIButtonState.PRESSED:
                    this.currentTexture = this.pressedTexture;
                    break;
            }

            this.callbackEvent += callbackEvent;
        }

        /// <summary>
        /// 
        /// Call this function to disable GUIButton
        /// </summary>
        public void Disable()
        {
            this.ChangeState(GUIButtonState.DISABLED);
        }

        /// <summary>
        /// 
        /// Call this function to set GUIButton state to normal
        /// </summary>
        public void Enable()
        {
            this.ChangeState(GUIButtonState.NORMAL);
        }

        /// <summary>
        /// 
        /// Updates GUIButton state
        /// </summary>
        public override void Update(GameTime gameTime)
        {
            switch(this.myState)
            {
                case GUIButtonState.DISABLED:
                    return;
                case GUIButtonState.HOVER:
                    if(InputManager.Instance.GetGamePadButtonDown(Microsoft.Xna.Framework.Input.Buttons.A))
                    {
                        this.ChangeState(GUIButtonState.PRESSED);
                    }
                    break;
                case GUIButtonState.NORMAL:
                    break;
                case GUIButtonState.PRESSED:
                    if (InputManager.Instance.GetGamePadButtonUp(Microsoft.Xna.Framework.Input.Buttons.A))
                    {
                        if(this.callbackEvent != null)
                        {
                            callbackEvent();
                        }
                    }
                    break;
            }
        }

        /// <summary>
        /// 
        /// Changes GUIButton state to passed in params
        /// </summary>
        public void ChangeState(GUIButtonState newState)
        {
            if (this.myState == GUIButtonState.PRESSED && newState == GUIButtonState.HOVER) return;
            this.myState = newState;
            this.currentTexture = null;

            switch (this.myState)
            {
                case GUIButtonState.DISABLED:
                    if (this.disabledTexture != null)
                    {
                        this.currentTexture = this.disabledTexture;
                    }
                    break;
                case GUIButtonState.HOVER:
                    if (this.hoverTexture != null)
                    {
                        this.currentTexture = this.hoverTexture;
                    }
                    break;
                case GUIButtonState.NORMAL:
                    if (this.normalTexture != null)
                    {
                        this.currentTexture = this.normalTexture;
                    }
                    break;
                case GUIButtonState.PRESSED:
                    if (this.pressedTexture != null)
                    {
                        this.currentTexture = this.pressedTexture;
                    }
                    break;
            }
        }

        /// <summary>
        /// 
        /// Draws GUIButton
        /// </summary>
        public override void Draw(SpriteBatch spriteBatch)
        {
            if(this.currentTexture != null)
            {
                spriteBatch.Draw(this.currentTexture, this.myRectangle, Color.White);
            }

            base.Draw(spriteBatch);
        }

        /// <summary>
        /// 
        /// This function will compare two GUIButtons. If theirs textures and callbackEvent are equal it will return true. Otherwise it will return false.
        /// </summary>
        public override bool Equals(object obj)
        {
            GUIButton buttonToCompare = (GUIButton)obj;
            if (buttonToCompare == null) return false;

            if (this.disabledTexture != buttonToCompare.disabledTexture) return false;
            if (this.callbackEvent != buttonToCompare.callbackEvent) return false;
            if (this.hoverTexture != buttonToCompare.hoverTexture) return false;
            if (this.normalTexture != buttonToCompare.normalTexture) return false;
            if (this.pressedTexture != buttonToCompare.pressedTexture) return false;

            return true;
        }

        #endregion
    }

    #endregion

    #region GUIManager class

    public class GUIManager : Singleton<GUIManager>
    {
        #region Variables

        private List<GUIElement> elementsToDraw = new List<GUIElement>();
        private List<GUIButton> allButtonsDeclared = new List<GUIButton>();
        private List<int> buttonsToDrawIndices = new List<int>();

        private bool fade;
        private double fadeTime;
        private float fadeAlpha = 0.0f;
        private float fadeDirection;
        private Color fadeColor;

        #endregion 

        #region Methods

        public GUIManager()
        {

        }

        /// <summary>
        /// 
        /// Updates state of GUIElements
        /// </summary>
        public void Update(GameTime gameTime)
        {
            // fade solving
            if(fade)
            {
                fadeAlpha = MathHelper.Clamp(
                    fadeAlpha + fadeDirection * (float)(gameTime.ElapsedGameTime.TotalMilliseconds / fadeTime),
                    0.0f, 1.0f);

                if (fadeAlpha <= 0.0f || fadeAlpha >= 1.0f)
                    fade = false;
            }

            foreach (GUIElement element in this.elementsToDraw)
            {
                element.Update(gameTime);
            }

            foreach(int i in this.buttonsToDrawIndices)
            {
                this.allButtonsDeclared[i].Update(gameTime);
            }
        }

        /// <summary>
        /// 
        /// Draws texture on top of everything, should be called per frame for each GUI texture that has to be drawn
        /// </summary>
        public void DrawTexture(Texture2D texture, Vector2 position, float width, float height)
        {
            GUITexture newTexture = new GUITexture(texture, position, width, height);
            this.elementsToDraw.Add(newTexture);
        }

        /// <summary>
        /// 
        /// Draws texture on top of everything, should be called per frame for each GUI texture that has to be drawn
        /// </summary>
        public void DrawTexture(Texture2D texture, Rectangle rect)
        {
            GUITexture newTexture = new GUITexture(texture, rect);
            this.elementsToDraw.Add(newTexture);
        }

        /// <summary>
        /// 
        /// Draw given guiTexture on top of everything, should be called per frame for each GUI texture that has to be drawn
        /// </summary>
        public void DrawTexture(GUITexture guiTexture)
        {
            this.elementsToDraw.Add(guiTexture);
        }

        /// <summary>
        /// 
        /// Draws text on top of everything, should be called per frame for each text that has to be drawn
        /// </summary>
        public void DrawText(SpriteFont font, string text, Vector2 position, Color color)
        {
            GUIText newText = new GUIText(font, text, color, position);
            this.elementsToDraw.Add(newText);
        }

        /// <summary>
        /// 
        /// Draws text on top of everything, should be called per frame for each text that has to be drawn
        /// </summary>
        public void DrawText(SpriteFont font, string text, Vector2 position, Color color, float scale)
        {
            GUIText newText = new GUIText(font, text, color, position, scale);
            this.elementsToDraw.Add(newText);
        }

        /// <summary>
        /// 
        /// Draws given guiText on top of everything, should be called per frame for each text that has to be drawn
        /// </summary>
        public void DrawText(GUIText guiText)
        {
            this.elementsToDraw.Add(guiText);
        }

        /// <summary>
        /// 
        /// Draws GUIButton with passed parameters
        /// </summary>
        /// <param name="callbackEvent">Function that will be called when button is pressed</param>
        public void DrawButton(Texture2D normalTexture, Texture2D hoverTexture, Texture2D pressedTexture, Texture2D disabledTexture, ButtonPressedCallback callbackEvent, Vector2 position, float width, float height)
        {
            GUIButton guiButton = new GUIButton(normalTexture, hoverTexture, pressedTexture, disabledTexture, callbackEvent, position, width, height);
            this.DrawButton(guiButton);
        }

        /// <summary>
        /// 
        /// Draws GUIButton with passed parameters
        /// </summary>
        /// <param name="callbackEvent">Function that will be called when button is pressed</param>
        public void DrawButton(Texture2D normalTexture, Texture2D hoverTexture, Texture2D pressedTexture, Texture2D disabledTexture, ButtonPressedCallback callbackEvent, Rectangle rect)
        {
            GUIButton guiButton = new GUIButton(normalTexture, hoverTexture, pressedTexture, disabledTexture, callbackEvent, rect);
            this.DrawButton(guiButton);
        }

        /// <summary>
        /// 
        /// Draws GUIButton with passed parameters
        /// </summary>
        /// <param name="callbackEvent">Function that will be called when button is pressed</param>
        public void DrawButton(Texture2D normalTexture, Texture2D hoverTexture, Texture2D pressedTexture, Texture2D disabledTexture, GUIButtonState startState, ButtonPressedCallback callbackEvent, Vector2 position, float width, float height)
        {
            GUIButton guiButton = new GUIButton(normalTexture, hoverTexture, pressedTexture, disabledTexture, callbackEvent, startState, position, width, height);
            this.DrawButton(guiButton);
        }

        /// <summary>
        /// 
        /// Draws GUIButton with passed parameters
        /// </summary>
        /// <param name="callbackEvent">Function that will be called when button is pressed</param>
        public void DrawButton(Texture2D normalTexture, Texture2D hoverTexture, Texture2D pressedTexture, Texture2D disabledTexture, GUIButtonState startState, ButtonPressedCallback callbackEvent, Rectangle rect)
        {
            GUIButton guiButton = new GUIButton(normalTexture, hoverTexture, pressedTexture, disabledTexture, callbackEvent, startState, rect);
            this.DrawButton(guiButton);
        }

        /// <summary>
        /// 
        /// Draws given guiButton on top of everything, should be called per frame for each button that has to be drawn
        /// </summary>
        public void DrawButton(GUIButton guiButton)
        {
            int i = this.FindButton(guiButton);
            if(i == -1)
            {
                this.allButtonsDeclared.Add(guiButton);
                this.buttonsToDrawIndices.Add(this.allButtonsDeclared.IndexOf(guiButton));
            }
            else
            {
                this.buttonsToDrawIndices.Add(i);
            }
        }

        /// <summary>
        /// 
        /// Finds index of GUIButton given. If there is no such a button the function will return -1
        /// </summary>
        private int FindButton(GUIButton guiButton)
        {
            if (this.allButtonsDeclared.Count == 0) return -1;

            foreach(GUIButton button in this.allButtonsDeclared)
            {
                if (button.Equals(guiButton)) return this.allButtonsDeclared.IndexOf(button);
            }

            return -1;
        }

        /// <summary>
        /// 
        /// Renders current list of GUIElements passed in Draw* methods
        /// </summary>
        public void Render(SpriteBatch spriteBatch)
        {
            if(spriteBatch != null && elementsToDraw.Count > 0)
            {
                if (ResourceManager.Instance.ImmediateStop)
                {
                    this.buttonsToDrawIndices.Clear();
                    this.elementsToDraw.Clear();
                    return;
                }

                spriteBatch.Begin();

                foreach (GUIElement element in this.elementsToDraw)
                {
                    element.Draw(spriteBatch);
                }

                foreach (int i in this.buttonsToDrawIndices)
                {
                    this.allButtonsDeclared[i].Draw(spriteBatch);
                }

                // fade draw
                spriteBatch.Draw(ResourceManager.Instance.Textures["DefaultDiffuse"], 
                    new Rectangle(0, 0, TrashSoupGame.Instance.Window.ClientBounds.Width, TrashSoupGame.Instance.Window.ClientBounds.Height), 
                    new Color(fadeColor.R, fadeColor.B, fadeColor.A, fadeAlpha));

                spriteBatch.End();

                this.buttonsToDrawIndices.Clear();
                this.elementsToDraw.Clear();
            }
        }

        public void FadeIn(Color color, double timeMS)
        {
            if(!fade)
            {
                fade = true;
                fadeDirection = 1.0f;
                fadeColor = color;
                fadeTime = timeMS;
            }
        }

        public void FadeOut(Color color, double timeMS)
        {
            if (!fade)
            {
                fade = true;
                fadeDirection = -1.0f;
                fadeColor = color;
                fadeTime = timeMS;
            }
        }

        public void FadeClear()
        {
            fade = false;
            fadeDirection = 0.0f;
            fadeColor = Color.White;
            fadeTime = 0.0f;
            fadeAlpha = 0.0f;
        }

        public void Clear()
        {
            elementsToDraw.Clear();
        }

        #endregion
    }

    #endregion
}
