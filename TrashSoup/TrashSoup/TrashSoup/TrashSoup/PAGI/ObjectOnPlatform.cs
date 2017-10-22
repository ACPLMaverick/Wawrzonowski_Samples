using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;

namespace TrashSoup.PAGI
{
    class ObjectOnPlatform : ObjectComponent
    {
        private GameObject platform;
        private Vector3 currentMovement;
        private float dragFactor = 0.05f;
        private float moveSpeed = 7.5f;
        private Vector3 platformRotationOnFall = Vector3.Zero;

        public ObjectOnPlatform(GameObject go) : base(go)
        {

        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            if(!this.MyObject.MyPhysicalObject.IsUsingGravity)
            {
                Vector3 tempVelo = Vector3.Right * gameTime.ElapsedGameTime.Milliseconds * 0.001f * -MathHelper.ToDegrees(platform.MyTransform.Rotation.Z);
                if (platform.MyTransform.Rotation.Z >= 0.0f) tempVelo = Vector3.Zero;
                this.currentMovement += tempVelo;

                tempVelo = Vector3.Right * gameTime.ElapsedGameTime.Milliseconds * 0.001f * -MathHelper.ToDegrees(platform.MyTransform.Rotation.Z);
                if (platform.MyTransform.Rotation.Z <= 0.0f) tempVelo = Vector3.Zero;
                this.currentMovement += tempVelo;

                tempVelo = Vector3.Forward * gameTime.ElapsedGameTime.Milliseconds * 0.001f * MathHelper.ToDegrees(platform.MyTransform.Rotation.X);
                if (platform.MyTransform.Rotation.X <= 0.0f) tempVelo = Vector3.Zero;
                this.currentMovement += tempVelo;

                tempVelo = Vector3.Forward * gameTime.ElapsedGameTime.Milliseconds * 0.001f * MathHelper.ToDegrees(platform.MyTransform.Rotation.X);
                if (platform.MyTransform.Rotation.X >= 0.0f) tempVelo = Vector3.Zero;
                this.currentMovement += tempVelo;
            }
            else
            {
                Vector3 tempVelo = Vector3.Right * gameTime.ElapsedGameTime.Milliseconds * 0.001f * -MathHelper.ToDegrees(platformRotationOnFall.Z);
                if (platformRotationOnFall.Z >= 0.0f) tempVelo = Vector3.Zero;
                this.currentMovement += tempVelo;

                tempVelo = Vector3.Right * gameTime.ElapsedGameTime.Milliseconds * 0.001f * -MathHelper.ToDegrees(platformRotationOnFall.Z);
                if (platformRotationOnFall.Z <= 0.0f) tempVelo = Vector3.Zero;
                this.currentMovement += tempVelo;

                tempVelo = Vector3.Forward * gameTime.ElapsedGameTime.Milliseconds * 0.001f * MathHelper.ToDegrees(platformRotationOnFall.X);
                if (platformRotationOnFall.X <= 0.0f) tempVelo = Vector3.Zero;
                this.currentMovement += tempVelo;

                tempVelo = Vector3.Forward * gameTime.ElapsedGameTime.Milliseconds * 0.001f * MathHelper.ToDegrees(platformRotationOnFall.X);
                if (platformRotationOnFall.X >= 0.0f) tempVelo = Vector3.Zero;
                this.currentMovement += tempVelo;
            }

            this.MyObject.MyTransform.Rotation += new Vector3(currentMovement.X, 0.0f, 0.0f) * gameTime.ElapsedGameTime.Milliseconds * 0.001f * moveSpeed;

            this.MyObject.MyTransform.Position += currentMovement * moveSpeed * gameTime.ElapsedGameTime.Milliseconds * 0.001f;
            this.currentMovement *= (1.0f - dragFactor);
        }

        public override void Draw(Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, Microsoft.Xna.Framework.GameTime gameTime)
        {

        }

        protected override void Start()
        {
        }

        public override void OnTriggerEnter(GameObject other)
        {
            if(other.UniqueID == 0)
            {
                this.MyObject.MyPhysicalObject.IsUsingGravity = false;
            }
            base.OnTriggerEnter(other);
        }

        public override void OnTriggerExit(GameObject other)
        {
            if(other.UniqueID == 0)
            {
                this.platformRotationOnFall = platform.MyTransform.Rotation;
                this.MyObject.MyPhysicalObject.IsUsingGravity = true;
            }
            base.OnTriggerExit(other);
        }

        public override void Initialize()
        {
            this.MyObject.MyTransform.Version = Transform.GameVersionEnum.STENGERT_PAGI;
            platform = ResourceManager.Instance.CurrentScene.GetObject(0);
            this.MyObject.MyTransform.Rotation += Vector3.Up * MathHelper.PiOver2;
            platform.AddChild(this.MyObject);
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
