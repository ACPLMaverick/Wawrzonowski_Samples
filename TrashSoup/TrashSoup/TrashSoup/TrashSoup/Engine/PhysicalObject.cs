using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Serialization;
using Microsoft.Xna.Framework;

namespace TrashSoup.Engine
{
    public enum RotationConstraintsEnum
    {
        None,
        X,
        Y,
        Z,
        XandY,
        XandZ,
        YandZ,
        All
    }

    public enum PositionConstraintsEnum
    {
        None,
        X,
        Y,
        Z,
        XandY,
        XandZ,
        YandZ,
        All
    }

    //TODO:
    //Improve Update function
    //Add function to deal with collisions
    public class PhysicalObject : ObjectComponent, IXmlSerializable
    {
        #region Variables

        private Vector3 acceleration;

        #endregion

        #region Properties

        public float Mass { get; set; }
        public float DragFactor { get; set; }
        public bool IsUsingGravity { get; set; }
        public Vector3 Velocity { get; set; }
        public RotationConstraintsEnum RotationConstraints { get; set; }
        public PositionConstraintsEnum PositionConstraints { get; set; }

        public bool Sleeping { get; set; }

        #endregion

        #region Methods

        public PhysicalObject(GameObject gameObj) : base(gameObj)
        {
            this.Sleeping = false;
            this.RotationConstraints = RotationConstraintsEnum.None;
            this.PositionConstraints = PositionConstraintsEnum.None;
            this.Velocity = Vector3.Zero;
        }

        public PhysicalObject(GameObject gameObj, float mass, float dragFactor, bool isUsingGravity) : base(gameObj)
        {
            this.Mass = mass;
            this.DragFactor = dragFactor;
            this.IsUsingGravity = isUsingGravity;
            this.Sleeping = false;
            this.RotationConstraints = RotationConstraintsEnum.None;
            this.PositionConstraints = PositionConstraintsEnum.None;
            this.Velocity = Vector3.Zero;
        }

        public PhysicalObject(GameObject gameObj, PhysicalObject po) : base(gameObj)
        {
            this.Mass = po.Mass;
            this.DragFactor = po.DragFactor;
            this.IsUsingGravity = po.IsUsingGravity;
            this.Sleeping = po.Sleeping;
            this.RotationConstraints = po.RotationConstraints;
            this.PositionConstraints = po.PositionConstraints;
            this.Velocity = po.Velocity;
        }

        protected override void Start()
        {

        }

        /// <summary>
        /// 
        /// Makes physical object not sleeping so it can be affected by external forces or gravity
        /// </summary>
        public void Awake()
        {
            this.Sleeping = false;
        }

        /// <summary>
        /// 
        /// Makes physical object sleeping so it can not be affected by anything related to physics
        /// </summary>
        public void Sleep()
        {
            this.Sleeping = true;
            this.Velocity = Vector3.Zero;
        }

        /// <summary>
        /// 
        /// Adds passed force to physical object so it can jump for example
        /// </summary>
        public void AddForce(Vector3 force)
        {
            this.acceleration += force / this.Mass;
        }

        public void ZeroForce()
        {
            this.acceleration = Vector3.Zero;
        }

        public override void Update(GameTime gameTime)
        {
            //If is not sleeping
            if(!this.Sleeping)
            {
                //Slow down (there's a angular drag or whatever)
                this.acceleration *= (1 - this.DragFactor);
                if (this.acceleration.Length() < 0.001f) this.acceleration = Vector3.Zero;

                //Accelerate
                this.Velocity += this.acceleration * (float)gameTime.ElapsedGameTime.TotalMilliseconds / 1000.0f;

                //Add a gravity
                if(this.IsUsingGravity)
                {
                    this.Velocity += this.Mass * PhysicsManager.Instance.Gravity * (float)gameTime.ElapsedGameTime.TotalMilliseconds / 1000.0f;
                }


                //Change game object position because of velocity
                if (this.MyObject.MyTransform != null)
                {
                    this.MyObject.MyTransform.Position += Velocity * (float)gameTime.ElapsedGameTime.TotalMilliseconds / 1000.0f;
                }
            }
        }

        public override void Draw(Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, GameTime gameTime)
        {
            //Do nothing, we do not expect to draw something as abstract as physical object component
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
            //MyObject = ResourceManager.Instance.CurrentScene.GetObject(tmp);

            if(reader.Name == "Acceleration")
            {
                reader.ReadStartElement();
                acceleration = new Vector3(reader.ReadElementContentAsFloat("X", ""),
                    reader.ReadElementContentAsFloat("Y", ""),
                    reader.ReadElementContentAsFloat("Z", ""));
                reader.ReadEndElement();
            }

            Mass = reader.ReadElementContentAsFloat("Mass", "");
            DragFactor = reader.ReadElementContentAsFloat("DragFactor", "");
            IsUsingGravity = reader.ReadElementContentAsBoolean("IsUsingGravity", "");

            if(reader.Name == "Velocity")
            {
                reader.ReadStartElement();
                Velocity = new Vector3(reader.ReadElementContentAsFloat("X", ""),
                    reader.ReadElementContentAsFloat("Y", ""),
                    reader.ReadElementContentAsFloat("Z", ""));
                reader.ReadEndElement();
            }

            RotationConstraints = (RotationConstraintsEnum)Enum.Parse(typeof(RotationConstraintsEnum), 
                reader.ReadElementString("RotationConstraints", ""));
            PositionConstraints = (PositionConstraintsEnum)Enum.Parse(typeof(PositionConstraintsEnum),
                reader.ReadElementString("PositionConstraints", ""));

            reader.ReadEndElement();
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            base.WriteXml(writer);

            writer.WriteStartElement("Acceleration");
            writer.WriteElementString("X", XmlConvert.ToString(acceleration.X));
            writer.WriteElementString("Y", XmlConvert.ToString(acceleration.Y));
            writer.WriteElementString("Z", XmlConvert.ToString(acceleration.Z));
            writer.WriteEndElement();

            writer.WriteElementString("Mass", XmlConvert.ToString(Mass));
            writer.WriteElementString("DragFactor", XmlConvert.ToString(DragFactor));
            writer.WriteElementString("IsUsingGravity", XmlConvert.ToString(IsUsingGravity));

            writer.WriteStartElement("Velocity");
            writer.WriteElementString("X", XmlConvert.ToString(Velocity.X));
            writer.WriteElementString("Y", XmlConvert.ToString(Velocity.Y));
            writer.WriteElementString("Z", XmlConvert.ToString(Velocity.Z));
            writer.WriteEndElement();

            writer.WriteElementString("RotationConstraints", RotationConstraints.ToString());
            writer.WriteElementString("PositionConstraints", PositionConstraints.ToString());
        }

        #endregion
    }
}
