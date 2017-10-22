using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay
{
    class InputHandler
    {
        private static InputHandler instance;

        public static InputHandler Instance
        {
            get
            {
                if(instance == null)
                {
                    instance = new InputHandler();
                }
                return instance;
            }
        }

        private InputHandler()
        {

        }

        public bool IsSprinting()
        {
            return (InputManager.Instance.GetGamePadButton(Buttons.A) ||
                InputManager.Instance.GetKeyboardButton(Keys.LeftShift));
        }

        /// <summary>
        /// 
        /// Returns true if player pushed right trigger (once and is not holding)
        /// </summary>
        public bool IsAttacking()
        {
            return (InputManager.Instance.GetGamePadButtonDown(Buttons.RightTrigger) ||
                InputManager.Instance.GetLeftMouseButtonDown());
        }

        /// <summary>
        /// 
        /// Returns true if player pushed A (once and is not holding) or Left alt
        /// </summary>
        public bool IsJumping()
        {
            return (InputManager.Instance.GetGamePadButtonDown(Buttons.A) ||
                InputManager.Instance.GetKeyboardButtonDown(Keys.Space));
        }

        /// <summary>
        /// 
        /// Returns true if player hold X button on X360 gamepad or E on keyboard
        /// </summary>
        public bool Action()
        {
            return InputManager.Instance.GetGamePadButton(Buttons.X) || 
                InputManager.Instance.GetKeyboardButton(Keys.E);
        }

        public bool GetWeapon()
        {
            return InputManager.Instance.GetGamePadButtonDown(Buttons.X) ||
                InputManager.Instance.GetKeyboardButtonDown(Keys.E);
        }

        public bool Eat()
        {
            return InputManager.Instance.GetGamePadButtonDown(Buttons.Y) || InputManager.Instance.GetKeyboardButtonDown(Keys.R);
        }

        public bool PromptOption1()
        {
            return (InputManager.Instance.GetGamePadButtonDown(Buttons.X) ||
                InputManager.Instance.GetKeyboardButtonDown(Keys.E));
        }

        public bool PromptOption2()
        {
            return InputManager.Instance.GetGamePadButtonDown(Microsoft.Xna.Framework.Input.Buttons.B) ||
                InputManager.Instance.GetKeyboardButtonDown(Keys.C);
        }

        public bool PromptOption3()
        {
            return InputManager.Instance.GetGamePadButtonDown(Microsoft.Xna.Framework.Input.Buttons.Y) ||
                InputManager.Instance.GetKeyboardButtonDown(Keys.V);
        }

        public bool Block()
        {
            return InputManager.Instance.GetRightMouseButton() || InputManager.Instance.GetGamePadButton(Buttons.LeftTrigger);
        }

        public bool Dodge()
        {
            return InputManager.Instance.GetKeyboardButtonDown(Keys.Q) || InputManager.Instance.GetGamePadButtonDown(Buttons.B);
        }

        public bool DropWeapon()
        {
            return InputManager.Instance.GetKeyboardButtonDown(Keys.U) || InputManager.Instance.GetGamePadButtonDown(Buttons.LeftShoulder);
        }

        public bool GodMode()
        {
            return InputManager.Instance.GetKeyboardButtonDown(Keys.G) || InputManager.Instance.GetGamePadButtonDown(Buttons.RightShoulder);
        }

        public bool ResetPlayer()
        {
            return InputManager.Instance.GetKeyboardButtonDown(Keys.H) || InputManager.Instance.GetGamePadButtonDown(Buttons.Start);
        }

        public Vector2 NoClipVector()
        {
            float y = 0.0f;
            float x = 0.0f;
            if(InputManager.Instance.GetGamePadButton(Buttons.DPadUp) || InputManager.Instance.GetKeyboardButton(Keys.Up))
            {
                y += 1.0f;
            }
            if (InputManager.Instance.GetGamePadButton(Buttons.DPadDown) || InputManager.Instance.GetKeyboardButton(Keys.Down))
            {
                y -= 1.0f;
            }

            if (InputManager.Instance.GetGamePadButton(Buttons.DPadLeft) || InputManager.Instance.GetKeyboardButton(Keys.Left))
            {
                x -= 1.0f;
            }

            if (InputManager.Instance.GetGamePadButton(Buttons.DPadRight) || InputManager.Instance.GetKeyboardButton(Keys.Right))
            {
                x += 1.0f;
            }

            return new Vector2(x, y);
        }

        /// <summary>
        /// 
        /// Returns movement vector of the player, works on gamepad as well as on keyboard
        /// </summary>
        public Vector2 GetMovementVector()
        {
            Vector2 toReturn = Vector2.Zero;
            if ((toReturn = InputManager.Instance.GetLeftStickValue()).Length() != 0)
            {
                return toReturn;
            }
            else
            {
                if(InputManager.Instance.GetKeyboardButton(Keys.W) && InputManager.Instance.GetKeyboardButton(Keys.D))
                {
                    toReturn = new Vector2(1.0f, 1.0f);
                }
                else if (InputManager.Instance.GetKeyboardButton(Keys.W) && InputManager.Instance.GetKeyboardButton(Keys.A))
                {
                    toReturn = new Vector2(-1.0f, 1.0f);
                }
                else if (InputManager.Instance.GetKeyboardButton(Keys.S) && InputManager.Instance.GetKeyboardButton(Keys.D))
                {
                    toReturn = new Vector2(1.0f, -1.0f);
                }
                else if (InputManager.Instance.GetKeyboardButton(Keys.S) && InputManager.Instance.GetKeyboardButton(Keys.A))
                {
                    toReturn = new Vector2(-1.0f, -1.0f);
                }
                else if (InputManager.Instance.GetKeyboardButton(Keys.W))
                {
                    toReturn = new Vector2(0.0f, 1.0f);
                }
                else if (InputManager.Instance.GetKeyboardButton(Keys.S))
                {
                    toReturn = new Vector2(0.0f, -1.0f);
                }
                else if (InputManager.Instance.GetKeyboardButton(Keys.A))
                {
                    toReturn = new Vector2(-1.0f, 0.0f);
                }
                else if (InputManager.Instance.GetKeyboardButton(Keys.D))
                {
                    toReturn = new Vector2(1.0f, 0.0f);
                }
                else
                {
                    return toReturn;
                }
            }
            return Vector2.Normalize(toReturn);
        }

        /// <summary>
        /// 
        /// Returns camera vector, works on gamepad as well as on keyboard
        /// </summary>
        public Vector2 GetCameraVector()
        {
            Vector2 toReturn;
            if ((toReturn = InputManager.Instance.GetRightStickValue()).Length() != 0)
            {
                return toReturn;
            }
            else
            {
                toReturn = InputManager.Instance.GetMouseRelativeValue() * 0.2f;
                toReturn.Y = - toReturn.Y;
            }
            return toReturn;
        }
    }
}
