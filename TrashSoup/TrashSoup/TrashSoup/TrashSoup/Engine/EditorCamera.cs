using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrashSoup.Engine
{
    public class EditorCamera : Camera
    {
        private bool mouseDown = false;
        private Vector2 prevMousePosition = Vector2.Zero;
        private Vector2 currentMousePosition = Vector2.Zero;

        private int acceleration = 1;
        private float maxYRot = 89.99f;
        private float maxXRot = float.PositiveInfinity;
        private float curXRot = 0.0f;
        private float curYRot = 0.0f;

        public float CurXRot
        {
            set
            {
                curXRot = value;
            }
        }

        public float CurYRot
        {
            set
            {
                curYRot = value;
            }
        }

        public EditorCamera(uint uniqueID, string name, Vector3 pos, Vector3 translation, Vector3 target, Vector3 up, float fov, float near, float far) : base(uniqueID, name, pos, translation, target, up, fov, 1.0f, near, far)
        {
            this.currentMousePosition = new Vector2(Mouse.GetState().X, Mouse.GetState().Y);
            this.Direction = Vector3.Transform(Vector3.Forward, Matrix.CreateRotationX(MathHelper.ToRadians(-curYRot)) * Matrix.CreateRotationY(MathHelper.ToRadians(-curXRot)));
            this.prevMousePosition = this.currentMousePosition;
            
        }

        protected override void OnUpdate(GameTime gameTime)
        {
            base.OnUpdate(gameTime);

            MouseState mouse = Mouse.GetState();

            this.currentMousePosition = new Vector2(mouse.X, mouse.Y);

            if(!mouseDown && mouse.RightButton == ButtonState.Pressed)
            {
                mouseDown = true;
            }

            if (mouseDown && mouse.RightButton == ButtonState.Released)
            {
                mouseDown = false;
            }

            if(mouseDown)
            {
                KeyboardState keyboard = Keyboard.GetState();

                if(keyboard.IsKeyDown(Keys.LeftShift))
                {
                    acceleration = 5;
                }
                else
                {
                    acceleration = 1;
                }

                if(keyboard.IsKeyDown(Keys.W))
                {
                    this.Position += this.Direction * acceleration;
                    this.Target += this.Direction * acceleration;
                }

                if(keyboard.IsKeyDown(Keys.S))
                {
                    this.Position -= this.Direction * acceleration;
                    this.Target -= this.Direction * acceleration;
                }

                if(keyboard.IsKeyDown(Keys.A))
                {
                    this.Position -= this.Right * acceleration;
                    this.Target -= this.Right * acceleration;
                }

                if(keyboard.IsKeyDown(Keys.D))
                {
                    this.Position += this.Right * acceleration;
                    this.Target += this.Right * acceleration;
                }

                float dX = this.currentMousePosition.X - this.prevMousePosition.X;
                float dY = this.currentMousePosition.Y - this.prevMousePosition.Y;

                curXRot = MathHelper.Clamp(curXRot + dX, -this.maxXRot, this.maxXRot);
                curYRot = MathHelper.Clamp(curYRot + dY, -this.maxYRot, this.maxYRot);

                this.Direction = Vector3.Transform(Vector3.Forward, Matrix.CreateRotationX(MathHelper.ToRadians(-curYRot)) * Matrix.CreateRotationY(MathHelper.ToRadians(-curXRot)));
                this.Target = this.Position + this.Direction;
            }

            this.prevMousePosition = this.currentMousePosition;

        }
    }
}
