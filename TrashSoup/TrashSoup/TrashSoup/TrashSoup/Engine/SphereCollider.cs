using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System.Xml.Serialization;

namespace TrashSoup.Engine
{
    public class SphereCollider : Collider, IXmlSerializable
    {
        #region Variables

        public BoundingSphere Sphere;

        private BoundingSphere initialSphere;
        private CustomModel model;
        private List<Vector3> verticesToDraw = new List<Vector3>();
        private List<short> indices = new List<short>();

        #endregion

        public Vector3 Center { get; private set; }
        public float Radius { get; private set; }

        #region Properties

        #endregion

        #region Methods

        public SphereCollider(GameObject go) : base(go)
        {

        }

        public SphereCollider(GameObject go, bool isTrigger) : base(go, isTrigger)
        {

        }

        public SphereCollider(GameObject go, SphereCollider sc) : base(go, sc)
        {

        }

        public override void Draw(Camera cam, Effect effect, GameTime gameTime)
        {
            if(!this.Enabled)
            {
                return;
            }
            verticesToDraw.Clear();
            indices.Clear();
            Vector3 right = Vector3.Right * this.Sphere.Radius;
            Vector3 verticalCirclePoint;
            Vector3 horizontalCirclePoint;
            Vector3 rightUp = Vector3.Right + Vector3.Up;
            Vector3 rightDown = Vector3.Right + Vector3.Down;
            rightUp.Normalize();
            rightDown.Normalize();
            rightDown *= this.Sphere.Radius;
            rightUp *= this.Sphere.Radius;
            Vector3 pointToAdd;
            for (short i = 0; i < 360; ++i)
            {
                if (i - 1 > 0)
                {
                    indices.Add((short)(i - 1));
                }
                verticalCirclePoint = Vector3.Transform(right, Matrix.CreateRotationZ(MathHelper.ToRadians(i)));
                indices.Add(i);
                verticalCirclePoint.X += this.MyObject.MyTransform.Position.X;
                verticalCirclePoint.Y += this.MyObject.MyTransform.Position.Y;
                verticalCirclePoint.Z -= this.MyObject.MyTransform.Position.Z;
                verticesToDraw.Add(verticalCirclePoint);
            }

            for (short i = 0; i < 360; ++i)
            {
                if (i - 1 > 0)
                {
                    indices.Add((short)(360 + i - 1));
                }
                horizontalCirclePoint = Vector3.Transform(right, Matrix.CreateRotationY(MathHelper.ToRadians(i)));
                indices.Add((short)(360 + i));
                horizontalCirclePoint.X += this.MyObject.MyTransform.Position.X;
                horizontalCirclePoint.Y += this.MyObject.MyTransform.Position.Y;
                horizontalCirclePoint.Z -= this.MyObject.MyTransform.Position.Z;
                verticesToDraw.Add(horizontalCirclePoint);
            }

            for (short i = 0; i < 360; ++i)
            {
                if (i - 1 > 0)
                {
                    indices.Add((short)(720 + i - 1));
                }
                pointToAdd = Vector3.Transform(rightUp, Matrix.CreateRotationZ(MathHelper.ToRadians(i)) * Matrix.CreateRotationY(MathHelper.ToRadians(45.0f)));
                indices.Add((short)(720 + i));
                pointToAdd.X += this.MyObject.MyTransform.Position.X;
                pointToAdd.Y += this.MyObject.MyTransform.Position.Y;
                pointToAdd.Z -= this.MyObject.MyTransform.Position.Z;
                verticesToDraw.Add(pointToAdd);
            }

            for (short i = 0; i < 360; ++i)
            {
                if (i - 1 > 0)
                {
                    indices.Add((short)(1080 + i - 1));
                }
                pointToAdd = Vector3.Transform(rightDown, Matrix.CreateRotationZ(MathHelper.ToRadians(i)) * Matrix.CreateRotationY(MathHelper.ToRadians(-45.0f)));
                indices.Add((short)(1080 + i));
                pointToAdd.X += this.MyObject.MyTransform.Position.X;
                pointToAdd.Y += this.MyObject.MyTransform.Position.Y;
                pointToAdd.Z -= this.MyObject.MyTransform.Position.Z;
                verticesToDraw.Add(pointToAdd);
            }

            Vector3[] vertices = verticesToDraw.ToArray();
            VertexPositionColor[] primitiveList = new VertexPositionColor[vertices.Length];
            for (int i = 0; i < vertices.Length; ++i)
            {
                primitiveList[i] = new VertexPositionColor(vertices[i], Color.White);
            }

            BasicEffect lineEffect = new BasicEffect(TrashSoupGame.Instance.GraphicsDevice);
            lineEffect.LightingEnabled = false;
            lineEffect.TextureEnabled = false;
            lineEffect.VertexColorEnabled = true;

            GraphicsDevice gd = TrashSoupGame.Instance.GraphicsDevice;
            VertexBuffer buffer = new VertexBuffer(gd, typeof(VertexPositionColor), primitiveList.Length, BufferUsage.None);
            buffer.SetData(primitiveList);
            short[] indicesArray = indices.ToArray();
            IndexBuffer ib = new IndexBuffer(gd, IndexElementSize.SixteenBits, indicesArray.Length, BufferUsage.WriteOnly);
            ib.SetData(indicesArray);
            gd.SetVertexBuffer(buffer);
            gd.Indices = ib;

            if (cam == null)
                cam = ResourceManager.Instance.CurrentScene.Cam;

            lineEffect.World = Matrix.Identity;
            lineEffect.View = cam.ViewMatrix;
            lineEffect.Projection = cam.ProjectionMatrix;
            foreach (EffectPass pass in lineEffect.CurrentTechnique.Passes)
            {
                pass.Apply();
                gd.DrawUserIndexedPrimitives(PrimitiveType.LineList, primitiveList, 0, 1440, indicesArray, 0, 4 * 359);
            }

            base.Draw(cam, effect, gameTime);
        }

        protected override void Start()
        {
            base.Start();
        }

        public override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
        }

        protected override void CreateCollider()
        {
            if (this.MyObject == null) return;

            foreach(ObjectComponent oc in this.MyObject.Components)
            {
                if(oc.GetType() == typeof(CustomModel))
                {
                    this.model = (CustomModel)oc;
                }
            }

            this.Radius = float.MinValue;
            this.Center = Vector3.Zero;
            int verticesNum = 0;

            if(this.model != null)
            {
                foreach(ModelMesh mesh in this.model.LODs[0].Meshes)
                {
                    this.MyBoundingSphere = BoundingSphere.CreateMerged(this.MyBoundingSphere, mesh.BoundingSphere);
                    foreach (ModelMeshPart part in mesh.MeshParts)
                    {
                        int vertexStride = part.VertexBuffer.VertexDeclaration.VertexStride;
                        int vertexBufferSize = part.NumVertices * vertexStride;
                        float[] vertexData = new float[vertexBufferSize / sizeof(float)];
                        part.VertexBuffer.GetData<float>(vertexData);
                        for (int i = 0; i < vertexBufferSize / sizeof(float); i += vertexStride / sizeof(float))
                        {
                            Vector3 transformedPosition = Vector3.Transform(new Vector3(vertexData[i], vertexData[i + 1], vertexData[i + 2]), this.worldMatrix);
                            transformedPosition = Vector3.Transform(transformedPosition, mesh.ParentBone.Transform);
                            this.Center += transformedPosition;
                            ++verticesNum;
                        }
                    }
                }

                this.Center /= verticesNum;

                foreach (ModelMesh mesh in this.model.LODs[0].Meshes)
                {
                    foreach (ModelMeshPart part in mesh.MeshParts)
                    {
                        int vertexStride = part.VertexBuffer.VertexDeclaration.VertexStride;
                        int vertexBufferSize = part.NumVertices * vertexStride;
                        float[] vertexData = new float[vertexBufferSize / sizeof(float)];
                        part.VertexBuffer.GetData<float>(vertexData);
                        for (int i = 0; i < vertexBufferSize / sizeof(float); i += vertexStride / sizeof(float))
                        {
                            Vector3 transformedPosition = Vector3.Transform(new Vector3(vertexData[i], vertexData[i + 1], vertexData[i + 2]), this.worldMatrix);
                            transformedPosition = Vector3.Transform(transformedPosition, mesh.ParentBone.Transform);

                            if(Vector3.Distance(this.Center, transformedPosition) > this.Radius)
                            {
                                this.Radius = Vector3.Distance(this.Center, transformedPosition);
                            }
                        }
                    }
                }
            }
            else
            {
                this.Center = new Vector3(0.0f, 0.0f, 0.0f);
                this.Radius = 1.0f;
            }

            this.initialSphere = new BoundingSphere(this.Center, this.Radius);

            UpdateCollider();

            base.CreateCollider();
        }

        public override void UpdateCollider()
        {
            this.worldMatrix = this.MyObject.MyTransform.GetWorldMatrix();
            Vector3 newCenter = Vector3.Zero;
            newCenter = Vector3.Transform(this.Center, this.worldMatrix);
            this.Sphere.Center = newCenter + CustomOffset;
            this.Sphere = this.initialSphere.Transform(this.worldMatrix);
            this.Radius = this.initialSphere.Radius * MyObject.MyTransform.Scale;
            this.Sphere.Radius = this.Radius * CustomScale.X;

            if(MyObject.GetType() == typeof(LightPoint))
            {
                this.Sphere.Center.Z = -this.Sphere.Center.Z;
            }
        }

        public override bool Intersects(Collider col)
        {
            if (col == null)
            {
                IsCollision = false;
                return false;
            }

            if (this.IgnoredColliders.Count > 0 && this.IgnoredColliders.Contains(col))
            {
                IsCollision = false;
                return false;
            }

            if (!this.MyBoundingSphere.Intersects(col.MyBoundingSphere))
            {
                IsCollision = false;
                return false;
            }

            if (col.GetType() == typeof(BoxCollider))
            {
                IsCollision = IntersectsWithAABB(((BoxCollider)col).Box);
                return IsCollision;
            }
            else if (col.GetType() == typeof(SphereCollider))
            {
                IsCollision = IntersectsWithSphere(((SphereCollider)col).Sphere);
                return IsCollision;
            }

            IsCollision = false;
            return false;
        }

        private bool IntersectsWithSphere(BoundingSphere boundingSphere)
        {
            return this.Sphere.Intersects(boundingSphere);
        }

        private bool IntersectsWithAABB(BoundingBox boundingBox)
        {
            return this.Sphere.Intersects(boundingBox);
        }

        protected override Vector3 GetFarthestPointInDirection(Vector3 direction)
        {
            Vector3 d = direction;
            d.Normalize();
            return Center + d * Radius;
        }

        public override System.Xml.Schema.XmlSchema GetSchema()
        {
            return base.GetSchema();
        }

        public override void ReadXml(System.Xml.XmlReader reader)
        {
            //reader.MoveToContent();
            //reader.ReadStartElement();
            base.ReadXml(reader);
            //reader.ReadEndElement();
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            base.WriteXml(writer);
        }

        #endregion
    }
}
