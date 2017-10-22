using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;

namespace TrashSoup.Engine
{
    class InputManager : Singleton<InputManager>
    {
        #region Variables

        private GamePadState currentGamePadState;
        private GamePadState previousGamePadState;

        private KeyboardState currentKeyboardState;
        private KeyboardState prevKeyboardState;

        private MouseState currentMouseState;
        private MouseState prevMouseState;

        #endregion

        #region Methods

        public InputManager()
        {
            this.currentGamePadState = GamePad.GetState(PlayerIndex.One);
            this.currentKeyboardState = Keyboard.GetState();
            this.currentMouseState = Mouse.GetState();
        }

        public void Update(GameTime gameTime)
        {
            this.previousGamePadState = this.currentGamePadState;
            this.currentGamePadState = GamePad.GetState(PlayerIndex.One);

            this.prevKeyboardState = this.currentKeyboardState;
            this.currentKeyboardState = Keyboard.GetState();

            this.prevMouseState = this.currentMouseState;
            this.currentMouseState = Mouse.GetState();
        }

        public bool IsGamePadConnected()
        {
            return this.currentGamePadState.IsConnected;
        }

        /// <summary>
        /// 
        /// Returns left stick value or keyboard WASD value. For example if player clicks keys W and D the returned value will be (1,1). If the player clicks S and D keys the returned value will be (1,-1).
        /// </summary>
        public Vector2 GetLeftStickValue()
        {
            float x = this.currentGamePadState.ThumbSticks.Left.X;
            float y = this.currentGamePadState.ThumbSticks.Left.Y;

            return new Vector2(x, y);
        }

        /// <summary>
        ///
        /// Returns right stick value or delta mouse position. Similar to GetLeftStickValue() method
        /// </summary>
        public Vector2 GetRightStickValue()
        {
            float x = this.currentGamePadState.ThumbSticks.Right.X;
            float y = this.currentGamePadState.ThumbSticks.Right.Y;

            return new Vector2(x, y);
        }

        /// <summary>
        /// 
        /// Returns true if button passed is pressed once
        /// </summary>
        public bool GetGamePadButtonDown(Buttons button)
        {
            bool condition = this.currentGamePadState.IsButtonDown(button) && this.previousGamePadState.IsButtonUp(button);
            return condition;
        }

        /// <summary>
        /// 
        /// Returns true if button passed is not pressed
        /// </summary>
        public bool GetGamePadButtonUp(Buttons button)
        {
            bool condition = this.currentGamePadState.IsButtonUp(button) && this.previousGamePadState.IsButtonDown(button);
            return condition;
        }

        /// <summary>
        /// 
        /// Returns true if button pass is being pressed
        /// </summary>
        public bool GetGamePadButton(Buttons button)
        {
            return this.currentGamePadState.IsButtonDown(button);
        }

        public bool GetKeyboardButton(Keys key)
        {
            return this.currentKeyboardState.IsKeyDown(key);
        }

        public bool GetKeyboardButtonDown(Keys key)
        {
            return (this.currentKeyboardState.IsKeyDown(key) && this.prevKeyboardState.IsKeyUp(key));
        }

        public bool GetKeyboardButtonUp(Keys key)
        {
            return (this.currentKeyboardState.IsKeyUp(key) && this.prevKeyboardState.IsKeyDown(key));
        }

        public bool GetLeftMouseButton()
        {
            return (this.currentMouseState.LeftButton == ButtonState.Pressed);
        }

        public bool GetLeftMouseButtonDown()
        {
            return ((this.currentMouseState.LeftButton == ButtonState.Pressed) &&
                (this.prevMouseState.LeftButton == ButtonState.Released));
        }

        public bool GetLeftMouseButtonUp()
        {
            return ((this.currentMouseState.LeftButton == ButtonState.Released) &&
                (this.prevMouseState.LeftButton == ButtonState.Pressed));
        }

        public bool GetRightMouseButton()
        {
            return (this.currentMouseState.RightButton == ButtonState.Pressed);
        }

        public bool GetRightMouseButtonDown()
        {
            return ((this.currentMouseState.RightButton == ButtonState.Pressed) &&
                (this.prevMouseState.RightButton == ButtonState.Released));
        }

        public bool GetRightMouseButtonUp()
        {
            return ((this.currentMouseState.RightButton == ButtonState.Released) &&
                (this.prevMouseState.RightButton == ButtonState.Pressed));
        }

        public bool GetMiddleMouseButton()
        {
            return (this.currentMouseState.MiddleButton == ButtonState.Pressed);
        }

        public bool GetMiddleMouseButtonDown()
        {
            return ((this.currentMouseState.MiddleButton == ButtonState.Pressed) &&
                (this.prevMouseState.MiddleButton == ButtonState.Released));
        }

        public bool GetMiddleMouseButtonUp()
        {
            return ((this.currentMouseState.MiddleButton == ButtonState.Released) &&
                (this.prevMouseState.MiddleButton == ButtonState.Pressed));
        }

        public int GetRelativeScrollValue()
        {
            return this.currentMouseState.ScrollWheelValue - this.prevMouseState.ScrollWheelValue;
        }

        public int GetTotalScrollValue()
        {
            return this.currentMouseState.ScrollWheelValue;
        }

        public Vector2 GetMouseRelativeValue()
        {
            return new Vector2(this.currentMouseState.X - this.prevMouseState.X, this.currentMouseState.Y - this.prevMouseState.Y);
        }

        public Vector2 GetMouseTotalValue()
        {
            return new Vector2(this.currentMouseState.X, this.currentMouseState.Y);
        }
        
        #endregion
    }
}
