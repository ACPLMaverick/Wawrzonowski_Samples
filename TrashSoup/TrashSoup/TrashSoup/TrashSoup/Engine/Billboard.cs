using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Serialization;

namespace TrashSoup.Engine
{
    public class Billboard : ObjectComponent, IXmlSerializable
    {
        #region constants
        private static VertexPositionTexture[] vertices = 
        {
            new VertexPositionTexture(new Vector3(0.0f, 0.0f, 0.0f), new Vector2(0.0f, 0.0f)),
            new VertexPositionTexture(new Vector3(0.0f, 0.0f, 0.0f), new Vector2(1.0f, 0.0f)),
            new VertexPositionTexture(new Vector3(0.0f, 0.0f, 0.0f), new Vector2(1.0f, 1.0f)),
            new VertexPositionTexture(new Vector3(0.0f, 0.0f, 0.0f), new Vector2(0.0f, 1.0f))
        };

        private static int[] indices = 
        {
            2, 1, 0, 3, 2, 0
        };
        #endregion

        #region variables
        private VertexBuffer vertexBuffer;
        private IndexBuffer indexBuffer;
        private string contentPath;
        #endregion

        #region properties
        public Material Mat { get; set; }
        public Vector2 Size { get; set; }
        #endregion

        #region methods

        public Billboard(GameObject go)
            : base(go)
        {

        }

        public Billboard(GameObject go, Billboard bi)
            : base(go, bi)
        {
            this.contentPath = bi.contentPath;
            this.Mat = bi.Mat;
            this.Size = bi.Size;
        }

        public override void Update(GameTime gameTime)
        {
            // do nth
        }

        public override void Draw(Camera cam, Effect effect, GameTime gameTime)
        {
            if (TrashSoupGame.Instance.EditorMode)
                this.Visible = false;
            if(this.Visible && Mat != null && effect == null)
            {
                Camera camera;
                if (cam == null)
                    camera = ResourceManager.Instance.CurrentScene.Cam;
                else
                    camera = cam;

                Transform transform = MyObject.MyTransform;
                GraphicsDevice device = TrashSoupGame.Instance.GraphicsDevice;

                if (transform == null)
                    return;

                Mat.UpdateEffect(
                                 effect,
                                 transform.GetWorldMatrix(),
                                 transform.GetWorldMatrix() * camera.ViewProjMatrix,
                                 ResourceManager.Instance.CurrentScene.AmbientLight,
                                 ResourceManager.Instance.CurrentScene.DirectionalLights,
                                 MyObject.LightsAffecting,
                                 ResourceManager.Instance.CurrentScene.GetDirectionalShadowMap(),
                                 ResourceManager.Instance.CurrentScene.GetPointLight0ShadowMap(),
                                 camera.Position + camera.Translation,
                                 camera.Bounds,
                                 null,
                                 gameTime);
                SetAdditionalParameters(camera);

                Mat.MyEffect.CurrentTechnique.Passes[0].Apply();

                device.SetVertexBuffer(vertexBuffer);
                device.Indices = indexBuffer;

                device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, vertexBuffer.VertexCount, 0, indexBuffer.IndexCount / 3);

                device.SetVertexBuffer(null);
                device.Indices = null;
                Mat.FlushMaterialEffect();
            }
        }

        protected override void Start()
        {
            // nth
        }

        public override void Initialize()
        {
            vertexBuffer = new VertexBuffer(TrashSoupGame.Instance.GraphicsDevice,
                                            typeof(VertexPositionTexture),
                                            vertices.Count(),
                                            BufferUsage.WriteOnly);
            vertexBuffer.SetData<VertexPositionTexture>(vertices);

            indexBuffer = new IndexBuffer(TrashSoupGame.Instance.GraphicsDevice,
                                            IndexElementSize.ThirtyTwoBits,
                                            indices.Count(),
                                            BufferUsage.WriteOnly);
            indexBuffer.SetData<int>(indices);

            base.Initialize();
        }

        private void SetAdditionalParameters(Camera cam)
        {
            EffectParameter ep;

            ep = Mat.MyEffect.Parameters["Size"];
            if (ep != null)
                ep.SetValue(this.Size);

            ep = Mat.MyEffect.Parameters["CameraUp"];
            if (ep != null)
            {
                Vector3 up = -Vector3.Cross(cam.Direction, -cam.Right);
                up.Y = -up.Y;
                ep.SetValue(up);
            }

            ep = Mat.MyEffect.Parameters["CameraRight"];
            if (ep != null)
                ep.SetValue(cam.Right);
        }

        public override System.Xml.Schema.XmlSchema GetSchema()
        {
            return base.GetSchema();
        }

        public override void ReadXml(System.Xml.XmlReader reader)
        {
            reader.MoveToContent();
            reader.ReadStartElement();

            base.ReadXml(reader);

            if (reader.Name == "Material")
            {
            if (!TrashSoupGame.Instance.EditorMode)
            {
                contentPath = "../../../../TrashSoupContent/Materials/";
            }
            else
            {
                contentPath = "../../../TrashSoup/TrashSoupContent/Materials/";
            }
            reader.ReadStartElement();
            while (reader.NodeType != System.Xml.XmlNodeType.EndElement)
            {
                if (reader.Name == "Name")
                {

                        String newName = reader.ReadElementString("Name", "");
                        
                        Material m = new SkyboxMaterial();
                        XmlSerializer serializer = new XmlSerializer(typeof(Material));
                        using (FileStream file = new FileStream(contentPath + newName + ".xml", FileMode.Open))
                        {
                            m = (Material)serializer.Deserialize(file);
                            m.Name = newName;
                        }

                        Mat = m;
                    }
                }

                reader.ReadEndElement();
            }

            if (reader.Name == "Size")
            {
                reader.ReadStartElement();
                Size = new Vector2(reader.ReadElementContentAsFloat("X", ""),
                    reader.ReadElementContentAsFloat("Y", ""));
                reader.ReadEndElement();
            }

            reader.ReadEndElement();
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            if (!TrashSoupGame.Instance.EditorMode)
            {
                contentPath = "../../../../TrashSoupContent/Materials/";
            }
            else
            {
                contentPath = "../../../TrashSoup/TrashSoupContent/Materials/";
            }
            base.WriteXml(writer);

            writer.WriteStartElement("Material");
            if(Mat != null)
            {
                XmlSerializer serializer = new XmlSerializer(typeof(Material));
                using (FileStream file = new FileStream(Path.GetFullPath(contentPath) + Mat.Name + ".xml", FileMode.Create))
                {
                    serializer.Serialize(file, Mat);
                }
                writer.WriteElementString("Name", Mat.Name);
            }
            writer.WriteEndElement();

            writer.WriteStartElement("Size");
            writer.WriteElementString("X", XmlConvert.ToString(Size.X));
            writer.WriteElementString("Y", XmlConvert.ToString(Size.Y));
            writer.WriteEndElement();

        }

        #endregion
    }
}
