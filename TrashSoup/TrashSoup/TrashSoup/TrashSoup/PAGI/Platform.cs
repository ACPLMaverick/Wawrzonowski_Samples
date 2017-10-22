using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;

namespace TrashSoup.PAGI
{
    class Platform : ObjectComponent
    {
        private float speed = 1.3f;

        public Platform(GameObject go) : base(go)
        {

        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            if(InputManager.Instance.GetKeyboardButton(Microsoft.Xna.Framework.Input.Keys.D))
            {
                this.MyObject.MyTransform.Rotation += Vector3.Forward * MathHelper.ToRadians(30.0f * gameTime.ElapsedGameTime.Milliseconds * 0.001f * speed);
            }

            if (InputManager.Instance.GetKeyboardButton(Microsoft.Xna.Framework.Input.Keys.A))
            {
                this.MyObject.MyTransform.Rotation -= Vector3.Forward * MathHelper.ToRadians(30.0f * gameTime.ElapsedGameTime.Milliseconds * 0.001f * speed);
            }

            if (InputManager.Instance.GetKeyboardButton(Microsoft.Xna.Framework.Input.Keys.S))
            {
                this.MyObject.MyTransform.Rotation += Vector3.Right * MathHelper.ToRadians(30.0f * gameTime.ElapsedGameTime.Milliseconds * 0.001f * speed);
            }

            if (InputManager.Instance.GetKeyboardButton(Microsoft.Xna.Framework.Input.Keys.W))
            {
                this.MyObject.MyTransform.Rotation -= Vector3.Right * MathHelper.ToRadians(30.0f * gameTime.ElapsedGameTime.Milliseconds * 0.001f * speed);
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
            this.MyObject.MyTransform.Version = Transform.GameVersionEnum.STENGERT_PAGI;
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
