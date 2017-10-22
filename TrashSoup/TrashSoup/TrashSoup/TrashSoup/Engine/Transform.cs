using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using System.Xml;
using System.Xml.Serialization;

namespace TrashSoup.Engine
{
    public class Transform : ObjectComponent, IXmlSerializable
    {
        #region constants
        protected Matrix preRotationMatrix = Matrix.CreateRotationY(MathHelper.Pi);
        #endregion

        #region variables

        protected Matrix worldMatrix;
        protected Vector3 position;
        protected Vector3 prevPosition;
        protected Vector3 positionChangeNormal;
        protected Vector3 rotation;
        protected Vector3 forward;
        protected float scale;

        #endregion

        #region properties

        public enum GameVersionEnum { PBL, STENGERT_PAGI };

        public GameVersionEnum Version = GameVersionEnum.PBL;
        public Vector3 Position
        { 
            get
            {
                return position;
            }
            set
            {
                if (Version == GameVersionEnum.STENGERT_PAGI)
                {
                    this.position = value;
                }
                else
                {                       
                    CalculatePosition(value);
                }
            }
        }

        public Vector3 PositionGlobal
        {
            get
            {
                CalculateWorldMatrix();
                Vector3 p, s;
                Quaternion r;
                worldMatrix.Decompose(out s, out r, out p);

                p.Z = - p.Z;

                return p;
            }
        }

        public Vector3 PositionGlobalNoZInverted
        {
            get
            {
                CalculateWorldMatrix();
                Vector3 p, s;
                Quaternion r;
                worldMatrix.Decompose(out s, out r, out p);

                return p;
            }
        }

        public Vector3 PreviousPosition
        {
            get
            {
                return this.prevPosition;
            }
        }

        public Vector3 PositionChangeNormal
        {
            get
            {
                return this.positionChangeNormal;
            }
        }

        public Vector3 Rotation 
        { 
            get
            {
                return rotation;
            }
            set
            {
                rotation = value;

                if(rotation.X > MathHelper.TwoPi)
                {
                    rotation.X -= MathHelper.TwoPi;
                }
                else if (rotation.X < -MathHelper.TwoPi)
                {
                    rotation.X += MathHelper.TwoPi;
                }

                if (rotation.Y > MathHelper.TwoPi)
                {
                    rotation.Y -= MathHelper.TwoPi;
                }
                else if (rotation.Y < -MathHelper.TwoPi)
                {
                    rotation.Y += MathHelper.TwoPi;
                }

                if (rotation.Z > MathHelper.TwoPi)
                {
                    rotation.Z -= MathHelper.TwoPi;
                }
                else if (rotation.Z < -MathHelper.TwoPi)
                {
                    rotation.Z += MathHelper.TwoPi;
                }

                CalculateWorldMatrix();
            }
        }
        public Vector3 Forward
        {
            get
            {
                return forward;
            }
            set
            {
                forward = value;
                CalculateWorldMatrix();
            }
        }

        public float Scale 
        { 
            get
            {
                return scale;
            }
            set
            {
                scale = value;
                CalculateWorldMatrix();
            }
        }

        #endregion

        #region events

        public delegate void PositionChangedEventHandler(object sender, EventArgs e);

        public event PositionChangedEventHandler PositionChanged;

        #endregion

        #region methods

        public Transform(GameObject obj) : base(obj)
        {
            this.Position = Vector3.Zero;
            this.prevPosition = Vector3.Zero;
            this.Rotation = Vector3.Zero;
            this.Forward = Vector3.Forward;
            this.Scale = 1.0f;
        }

        public Transform(GameObject obj, Vector3 position, Vector3 forward, Vector3 rotation, float scale)
            : this(obj)
        {
            this.Position = position;
            this.Rotation = rotation;
            this.prevPosition = position;
            this.Scale = scale;
            this.Forward = forward;
        }

        public Transform(GameObject obj, Transform t) : base(obj)
        {
            this.Position = t.Position;
            this.Rotation = t.Rotation;
            this.prevPosition = t.prevPosition;
            this.Scale = t.Scale;
            this.Forward = t.Forward;
        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            if(MyObject.Dynamic)
            {
                CalculateWorldMatrix();
            }
        }

        public override void Draw(Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, Microsoft.Xna.Framework.GameTime gameTime)
        {
            // do nothing
        }

        public Matrix GetWorldMatrix()
        {
            return worldMatrix;
        }

        public void BakeTransformFromParent()
        {
            GameObject parent = MyObject.GetParent();
            if (parent != null && parent.MyTransform != null)
            {
                Matrix parents = parent.MyTransform.GetWorldMatrix();
                Vector3 trans, scl;
                Quaternion quat;
                parents.Decompose(out scl, out quat, out trans);

                this.position = Vector3.Transform(this.position, Matrix.CreateFromQuaternion(quat) * Matrix.CreateTranslation(trans));
                this.rotation += parent.MyTransform.rotation;
            }
        }

        public void BakeTransformFromCarrier()
        {
            if (MyObject.MyCarrierSocket == null)
                return;

            GameObject carrier = MyObject.MyCarrierSocket.Carrier;
            if (carrier != null && carrier.MyTransform != null)
            {
                Matrix parents = carrier.MyTransform.GetWorldMatrix();
                Vector3 trans, scl;
                Quaternion quat;
                parents.Decompose(out scl, out quat, out trans);

                this.position = Vector3.Transform(this.position, Matrix.CreateFromQuaternion(quat) * Matrix.CreateTranslation(trans));
                this.rotation += carrier.MyTransform.rotation;
            }
        }

        public void BakeTransformFromCarrierInvertZ()
        {
            if (MyObject.MyCarrierSocket == null)
                return;

            BakeTransformFromCarrier();

            Position = new Vector3(Position.X, Position.Y, -Position.Z);
        }

        protected override void Start()
        {
            /// do nothing
        }

        protected void CalculateWorldMatrix()
        {
            Matrix translation, rotation, scale, fromSocket, parents, secondPreRot;
            translation = Matrix.CreateTranslation(new Vector3(this.Position.X, this.Position.Y, -this.Position.Z));
            rotation = Matrix.CreateFromYawPitchRoll(Rotation.Y, Rotation.X, Rotation.Z);
            scale = Matrix.CreateScale(this.Scale);
            fromSocket = Matrix.Identity;
            parents = Matrix.Identity;
            secondPreRot = Matrix.Identity;

            if(MyObject.MyCarrierSocket != null)
            {
                fromSocket = MyObject.MyCarrierSocket.BoneTransform;

                Vector3 trans, scl;
                Quaternion quat;
                MyObject.MyCarrierSocket.Carrier.MyTransform.GetWorldMatrix().Decompose(out scl, out quat, out trans);


                fromSocket = fromSocket * Matrix.CreateFromQuaternion(quat) * Matrix.CreateTranslation(trans);
            }

            if(MyObject.GetParent() != null && MyObject.GetParent().MyTransform != null)
            {
                parents = MyObject.GetParent().MyTransform.GetWorldMatrix();
                Vector3 trans, scl;
                Quaternion quat;
                parents.Decompose(out scl, out quat, out trans);

                if(MyObject.GetType() == typeof(LightPoint))
                {
                    trans.Z = -trans.Z;
                }

                parents = Matrix.CreateFromQuaternion(quat) * Matrix.CreateTranslation(trans);
                secondPreRot = preRotationMatrix;
            }

            this.worldMatrix = preRotationMatrix * scale * rotation * translation * fromSocket * secondPreRot * parents;
        }

        protected void CalculatePositionChange()
        {
            this.positionChangeNormal.X = Math.Abs(this.position.X - this.prevPosition.X);// < 0.01f ? 0.0f : 1.0f;
            this.positionChangeNormal.Y = Math.Abs(this.position.Y - this.prevPosition.Y);// < 0.01f ? 0.0f : 1.0f;
            this.positionChangeNormal.Z = Math.Abs(this.position.Z - this.prevPosition.Z);// < 0.01f ? 0.0f : 1.0f;
        }

        protected void CalculatePosition(Vector3 value)
        {
            if(MyObject.GetType() == typeof(LightPoint))
            {
                value.Z = -value.Z;
            }
            Vector3 tmp = this.prevPosition;
            this.positionChangeNormal = value - this.position;
            this.prevPosition = this.position;
            position.X = value.X;
            CalculateWorldMatrix();
            if (!TrashSoupGame.Instance.EditorMode)
            {
                if (this.MyObject.MyCollider != null) this.MyObject.MyCollider.UpdateCollider();
                if (!PhysicsManager.Instance.CanMove(this.MyObject))
                {
                    this.position = this.prevPosition;
                    this.prevPosition = tmp;
                    this.positionChangeNormal = Vector3.Zero;
                    if (this.MyObject.MyCollider != null) this.MyObject.MyCollider.UpdateCollider();
                    CalculateWorldMatrix();
                }
            }
            this.prevPosition = this.position;
            position.Y = value.Y;
            CalculateWorldMatrix();
            if (!TrashSoupGame.Instance.EditorMode)
            {
                if (this.MyObject.MyCollider != null) this.MyObject.MyCollider.UpdateCollider();
                if (!PhysicsManager.Instance.CanMove(this.MyObject))
                {
                    this.position = this.prevPosition;
                    this.prevPosition = tmp;
                    this.positionChangeNormal = Vector3.Zero;
                    if (this.MyObject.MyCollider != null) this.MyObject.MyCollider.UpdateCollider();
                    CalculateWorldMatrix();
                }
            }
            this.prevPosition = this.position;
            position.Z = value.Z;
            CalculateWorldMatrix();
            if (!TrashSoupGame.Instance.EditorMode)
            {
                if (this.MyObject.MyCollider != null) this.MyObject.MyCollider.UpdateCollider();
                if (!PhysicsManager.Instance.CanMove(this.MyObject))
                {
                    this.position = this.prevPosition;
                    this.prevPosition = tmp;
                    this.positionChangeNormal = Vector3.Zero;
                    if (this.MyObject.MyCollider != null) this.MyObject.MyCollider.UpdateCollider();
                    CalculateWorldMatrix();
                }
            }
            if (PositionChanged != null) PositionChanged(this, null);
        }

        public override System.Xml.Schema.XmlSchema GetSchema() { return null; }
        public override void ReadXml(System.Xml.XmlReader reader)
        {
            reader.MoveToContent();
            reader.ReadStartElement();

            base.ReadXml(reader);
            //MyObject = ResourceManager.Instance.CurrentScene.GetObject(tmp);

            if(reader.Name == "Position")
            {
                reader.ReadStartElement();
                Position = new Vector3(reader.ReadElementContentAsFloat("X", ""),
                    reader.ReadElementContentAsFloat("Y", ""),
                    reader.ReadElementContentAsFloat("Z", ""));
                reader.ReadEndElement();
            }

            if(reader.Name == "Rotation")
            {
                reader.ReadStartElement();
                Rotation = new Vector3(reader.ReadElementContentAsFloat("X", ""),
                    reader.ReadElementContentAsFloat("Y", ""),
                    reader.ReadElementContentAsFloat("Z", ""));
                reader.ReadEndElement();
            }

            if(reader.Name == "Forward")
            {
                reader.ReadStartElement();
                Forward = new Vector3(reader.ReadElementContentAsFloat("X", ""),
                    reader.ReadElementContentAsFloat("Y", ""),
                    reader.ReadElementContentAsFloat("Z", ""));
                reader.ReadEndElement();
            }

            Scale = reader.ReadElementContentAsFloat("Scale", "");

            reader.ReadEndElement();
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            base.WriteXml(writer);

            writer.WriteStartElement("Position");
            writer.WriteElementString("X", XmlConvert.ToString(Position.X));
            writer.WriteElementString("Y", XmlConvert.ToString(Position.Y));
            if(MyObject.GetType() == typeof(LightPoint))
            {
                writer.WriteElementString("Z", XmlConvert.ToString(-Position.Z));
            }
            else
            {
                writer.WriteElementString("Z", XmlConvert.ToString(Position.Z));
            }
            writer.WriteEndElement();

            writer.WriteStartElement("Rotation");
            writer.WriteElementString("X", XmlConvert.ToString(Rotation.X));
            writer.WriteElementString("Y", XmlConvert.ToString(Rotation.Y));
            writer.WriteElementString("Z", XmlConvert.ToString(Rotation.Z));
            writer.WriteEndElement();

            writer.WriteStartElement("Forward");
            writer.WriteElementString("X", XmlConvert.ToString(Forward.X));
            writer.WriteElementString("Y", XmlConvert.ToString(Forward.Y));
            writer.WriteElementString("Z", XmlConvert.ToString(Forward.Z));
            writer.WriteEndElement();

            writer.WriteElementString("Scale", XmlConvert.ToString(Scale));
        }
        #endregion
    }
}
