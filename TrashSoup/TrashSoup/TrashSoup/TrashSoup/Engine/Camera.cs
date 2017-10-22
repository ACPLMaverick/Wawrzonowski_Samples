using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using System.Xml;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace TrashSoup.Engine
{
    public class Camera : GameObject, IXmlSerializable
    {
        #region variables
        protected GameObject toFollow;
        protected bool ortho;
        #endregion
       
        #region properties

        public Matrix ViewMatrix
        {
            get;
            set;
        }

        public Matrix ProjectionMatrix
        {
            get;
            protected set;
        }

        public Matrix ViewProjMatrix
        {
            get;
            set;
        }

        public Vector3 Position { get; set; }
        public Vector3 Translation { get; set; }
        public Vector3 Direction { get; set; }
        public Vector3 Target { get; set; }
        public Vector3 Up { get; set; }
        public Vector3 Right { get; set; }
        public float Speed { get; set; }
        public float FOV { get; set; }
        public float Ratio { get; set; }
        public float Near { get; set; }
        public float Far { get; set; }
        public float OrthoWidth { get; set; }
        public float OrthoHeight { get; set; }
        public BoundingFrustumExtended Bounds { get; set; }
        public bool Ortho 
        {
            get
            {
                return ortho;
            }
            set
            {
                ortho = value;
                CreateProjection();
            }
        }

        #endregion

        #region methods
        public Camera(uint uniqueID, string name, Vector3 pos, Vector3 translation, Vector3 target, Vector3 up, float fov, float ratio, float near, float far) 
            : base(uniqueID, name)
        {
            this.Position = pos;
            this.Translation = translation;
            this.Direction = target - pos;
            this.Target = target;
            this.Direction.Normalize();
            this.Up = up;
            this.Up.Normalize();
            this.Right = Vector3.Cross(this.Direction, this.Up);
            this.FOV = fov;
            this.Ratio = ratio;
            this.Near = near;
            this.Far = far;
            this.Bounds = new BoundingFrustumExtended(Matrix.Identity);

            CreateLookAt();

            CreateProjection();

            ViewProjMatrix = ViewMatrix * ProjectionMatrix;
            this.Bounds.Matrix = ViewProjMatrix;
        }

        public override void Update(GameTime gameTime)
        {
            base.Update(gameTime);

            OnUpdate(gameTime);

            this.Direction = this.Target - this.Position;
            this.Direction = this.Direction / this.Direction.Length();
            this.Right = Vector3.Cross(this.Direction, this.Up);
            this.Right = this.Right / this.Right.Length();

            UpdateViewProj();

            //Debug.Log(this.Direction.ToString());

            if(this.MyTransform != null)
            {
                MyTransform.Position = this.Position + this.Translation;
                MyTransform.Position = new Vector3(MyTransform.Position.X, MyTransform.Position.Y, -MyTransform.Position.Z);
            }
        }

        protected virtual void OnUpdate(GameTime gameTime)
        {
            //Do nothing, it's for editor camera that derieves from this class
        }

        public void UpdateViewProj()
        {
            CreateLookAt();

            ViewProjMatrix = ViewMatrix * ProjectionMatrix;
            this.Bounds.Matrix = ViewProjMatrix;
        }

        public Vector3 GetDirection() { return Direction; }

        public void CreateProjection()
        {
            if(!Ortho)
            {
                this.ProjectionMatrix = Matrix.CreatePerspectiveFieldOfView
                (
                    this.FOV,
                    this.Ratio,
                    this.Near,
                    this.Far
                );
            }
            else
            {
                this.ProjectionMatrix = Matrix.CreateOrthographic
                (
                    this.OrthoWidth,
                    this.OrthoHeight,
                    this.Near,
                    this.Far
                );
            }
            
        }

        public void CreateCameraCollider()
        {
            this.MyTransform = new Transform(this);
            this.MyTransform.Scale = 0.5f;
            this.Components.Add(new CustomModel(this, new Model[] {ResourceManager.Instance.Models["Models/Test/TestSphere"], null, null }, null));
            this.MyPhysicalObject = new PhysicalObject(this, 1.0f, 0.05f, false);
            this.MyCollider = new SphereCollider(this);
        }

        protected void CreateLookAt()
        {
            this.ViewMatrix = Matrix.CreateLookAt(Position + Translation, Target + Translation, Up);
        }

        public System.Xml.Schema.XmlSchema GetSchema() { return null; }

        public void ReadXml(System.Xml.XmlReader reader)
        {
            

            if(reader.Name == "CameraPosition")
            {
                reader.ReadStartElement();
                Position = new Vector3(reader.ReadElementContentAsFloat("X", ""),
                    reader.ReadElementContentAsFloat("Y", ""),
                    reader.ReadElementContentAsFloat("Z", ""));
                reader.ReadEndElement();
            }

            if (reader.Name == "CameraTranslation")
            {
                reader.ReadStartElement();
                Translation = new Vector3(reader.ReadElementContentAsFloat("X", ""),
                    reader.ReadElementContentAsFloat("Y", ""),
                    reader.ReadElementContentAsFloat("Z", ""));
                reader.ReadEndElement();
            }

            if (reader.Name == "CameraDirection")
            {
                reader.ReadStartElement();
                Direction = new Vector3(reader.ReadElementContentAsFloat("X", ""),
                    reader.ReadElementContentAsFloat("Y", ""),
                    reader.ReadElementContentAsFloat("Z", ""));
                reader.ReadEndElement();
            }

            if(reader.Name == "CameraUp")
            {
                reader.ReadStartElement();
                Up = new Vector3(reader.ReadElementContentAsFloat("X", ""),
                    reader.ReadElementContentAsFloat("Y", ""),
                    reader.ReadElementContentAsFloat("Z", ""));
                reader.ReadEndElement();
            }

            if(reader.Name == "CameraTarget")
            {
                reader.ReadStartElement();
                Target = new Vector3(reader.ReadElementContentAsFloat("X", ""),
                    reader.ReadElementContentAsFloat("Y", ""),
                    reader.ReadElementContentAsFloat("Z", ""));
                reader.ReadEndElement();

            }

            if (reader.Name == "CameraRight")
            {
                reader.ReadStartElement();
                Right = new Vector3(reader.ReadElementContentAsFloat("X", ""),
                    reader.ReadElementContentAsFloat("Y", ""),
                    reader.ReadElementContentAsFloat("Z", ""));
                reader.ReadEndElement();

            }

            FOV = reader.ReadElementContentAsFloat("FOV", "");
            Ratio = reader.ReadElementContentAsFloat("Ratio", "");
            Near = reader.ReadElementContentAsFloat("Near", "");
            Far = reader.ReadElementContentAsFloat("Far", "");

            CreateLookAt();
            CreateProjection();

            base.ReadXml(reader);
        }

        public void WriteXml(System.Xml.XmlWriter writer)
        {


            writer.WriteStartElement("CameraPosition");
            writer.WriteElementString("X", XmlConvert.ToString(Position.X));
            writer.WriteElementString("Y", XmlConvert.ToString(Position.Y));
            writer.WriteElementString("Z", XmlConvert.ToString(Position.Z));
            writer.WriteEndElement();

            writer.WriteStartElement("CameraTranslation");
            writer.WriteElementString("X", XmlConvert.ToString(Translation.X));
            writer.WriteElementString("Y", XmlConvert.ToString(Translation.Y));
            writer.WriteElementString("Z", XmlConvert.ToString(Translation.Z));
            writer.WriteEndElement();

            writer.WriteStartElement("CameraDirection");
            writer.WriteElementString("X", XmlConvert.ToString(Direction.X));
            writer.WriteElementString("Y", XmlConvert.ToString(Direction.Y));
            writer.WriteElementString("Z", XmlConvert.ToString(Direction.Z));
            writer.WriteEndElement();

            writer.WriteStartElement("CameraUp");
            writer.WriteElementString("X", Up.X.ToString());
            writer.WriteElementString("Y", Up.Y.ToString());
            writer.WriteElementString("Z", Up.Z.ToString());
            writer.WriteEndElement();

            writer.WriteStartElement("CameraTarget");
            writer.WriteElementString("X", XmlConvert.ToString(Target.X));
            writer.WriteElementString("Y", XmlConvert.ToString(Target.Y));
            writer.WriteElementString("Z", XmlConvert.ToString(Target.Z));
            writer.WriteEndElement();

            writer.WriteStartElement("CameraRight");
            writer.WriteElementString("X", XmlConvert.ToString(Right.X));
            writer.WriteElementString("Y", XmlConvert.ToString(Right.Y));
            writer.WriteElementString("Z", XmlConvert.ToString(Right.Z));
            writer.WriteEndElement();

            writer.WriteElementString("FOV", XmlConvert.ToString(FOV));
            writer.WriteElementString("Ratio", XmlConvert.ToString(Ratio));
            writer.WriteElementString("Near", XmlConvert.ToString(Near));
            writer.WriteElementString("Far", XmlConvert.ToString(Far));

            base.WriteXml(writer);
        }
        #endregion
    }
}
