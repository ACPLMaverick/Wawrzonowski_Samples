using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace TrashSoup.Engine
{
    /// <summary>
    /// 
    /// Simple box collider class. It creates a box arround the game object
    /// </summary>
    public class BoxCollider : Collider
    {
        #region Variables

        public BoundingBox Box;
        private CustomModel model;
        //private CustomSkinnedModel skinned;
        private Vector3 min;
        private Vector3 max;
        private Vector3[] corners;
        private Vector3[] initialCorners;

        private Face[] faces = new Face[6];

        #endregion

        #region Properties

        #endregion

        #region Methods

        public BoxCollider(GameObject go) : base(go)
        {

        }

        public BoxCollider(GameObject go, bool isTrigger) : base(go, isTrigger)
        {

        }

        public BoxCollider(GameObject go, BoxCollider bc) : base(go, bc)
        {
            this.IsTrigger = bc.IsTrigger;
            this.initialCorners = bc.initialCorners;
            this.min = bc.min;
            this.max = bc.max;
            this.Box = bc.Box;
            this.MyBoundingSphere = bc.MyBoundingSphere;
        }

        /// <summary>
        /// 
        /// Debug drawing collider, no one draws colliders in game :D
        /// </summary>
        public override void Draw(Camera cam, Effect effect, Microsoft.Xna.Framework.GameTime gameTime)
        {
            if (!this.Enabled)
            {
                return;
            }
            short[] bBoxIndices = {
                                0, 1, 1, 2, 2, 3, 3, 0, // Front edges
                                4, 5, 5, 6, 6, 7, 7, 4, // Back edges
                                0, 4, 1, 5, 2, 6, 3, 7 // Side edges connecting front and back
                                };

            Vector3[] corners = this.corners;
            VertexPositionColor[] primitiveList = new VertexPositionColor[corners.Length];

            // Assign the 8 box vertices
            for (int i = 0; i < corners.Length; i++)
            {
                primitiveList[i] = new VertexPositionColor(corners[i], Color.White);
            }

            BasicEffect lineEffect = new BasicEffect(TrashSoupGame.Instance.GraphicsDevice);
            lineEffect.LightingEnabled = false;
            lineEffect.TextureEnabled = false;
            lineEffect.VertexColorEnabled = true;

            GraphicsDevice gd = TrashSoupGame.Instance.GraphicsDevice;
            VertexBuffer buffer = new VertexBuffer(gd, typeof(VertexPositionColor), primitiveList.Length, BufferUsage.None);
            buffer.SetData(primitiveList);
            IndexBuffer ib = new IndexBuffer(gd, IndexElementSize.SixteenBits, bBoxIndices.Length, BufferUsage.WriteOnly);
            ib.SetData(bBoxIndices);
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
                gd.DrawUserIndexedPrimitives(PrimitiveType.LineList, primitiveList, 0, 8, bBoxIndices, 0, 12);
            }

            base.Draw(cam, effect, gameTime);
        }

        protected override void Start()
        {
            base.Start();
        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            base.Update(gameTime);
        }

        /// <summary>
        /// 
        /// This function creates box collider but it can create collider only for object that contains CustomModel or CustomSkinnedModel with first state of LOD (LODs[0] mustn't be null)
        /// </summary>
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

            min = new Vector3(float.MaxValue, float.MaxValue, float.MaxValue);
            max = new Vector3(float.MinValue, float.MinValue, float.MinValue);

            if (this.model != null)
            {
                foreach (ModelMesh mesh in this.model.LODs[0].Meshes)
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

                            min = Vector3.Min(min, transformedPosition);
                            max = Vector3.Max(max, transformedPosition);
                        }
                        min = Vector3.Transform(min, mesh.ParentBone.Transform);
                        max = Vector3.Transform(max, mesh.ParentBone.Transform);
                        if(min.X > max.X)
                        {
                            float tmp = min.X;
                            min.X = max.X;
                            max.X = tmp;
                        }
                        if(min.Y > max.Y)
                        {
                            float tmp = min.Y;
                            min.Y = max.Y;
                            max.Y = tmp;
                        }
                        if(min.Z > max.Z)
                        {
                            float tmp = min.Z;
                            min.Z = max.Z;
                            max.Z = tmp;
                        }
                    }
                }
            }
            else
            {
                this.min = new Vector3(-1.0f, -1.0f, -1.0f);
                this.max = new Vector3(1.0f, 1.0f, 1.0f);
            }

            if (Math.Abs(min.X - max.X) < 0.1f) max.X += 0.1f;
            if (Math.Abs(min.Y - max.Y) < 0.1f) max.Y += 0.1f;
            if (Math.Abs(min.Z - max.Z) < 0.1f) max.Z += 0.1f;

            this.Box = new BoundingBox(min, max);
            this.initialCorners = this.Box.GetCorners();
            this.corners = this.Box.GetCorners();

            this.UpdateCollider();

            base.CreateCollider();
        }

        /// <summary>
        /// 
        /// This function accepts a PhysicalObject as parameter and checks if bounding boxes (or spheres in future) intersect each other. Return true if collision detected
        /// </summary>
        public override bool Intersects(Collider col)
        {
            if (col == null)
            {
                IsCollision = false;
                return false;
            }

            if(this.IgnoredColliders.Count > 0 && this.IgnoredColliders.Contains(col))
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
                IsCollision = _OldIntersectsWithAABB(((BoxCollider)col).Box); 
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
            return this.Box.Intersects(boundingSphere);
        }

        private bool IntersectsWithOBB(BoxCollider box)
        {
            Simplex simplex = new Simplex(4);
            Vector3 d = box.MyObject.MyTransform.Position - this.MyObject.MyTransform.Position;
            Vector3 p1 = this.SupportFunction(this, box, d);
            d = -p1;
            simplex.Add(p1);
            uint maxIterations = 20;
            Vector3 A = Vector3.Zero;
            while(maxIterations > 0)
            {
                --maxIterations;
                A = this.SupportFunction(this, box, d);
                if(Vector3.Dot(A, d) < 0)
                {
                    return false;
                }
                simplex.Add(A);
                if (simplex.ContainsOrigin())
                {
                    return true;
                }
                else
                {
                    d = simplex.Direction;
                }
                    
            }
            return false;
        }

        protected override Vector3 GetFarthestPointInDirection(Vector3 direction)
        {
            Vector3 maxPoint = new Vector3(float.MinValue, float.MinValue, float.MinValue);
            float maxDot = float.MinValue;
            float currentDot = 0;

            foreach(Vector3 v in this.corners)
            {
                currentDot = Vector3.Dot(v, direction);
                if(currentDot > maxDot)
                {
                    maxDot = currentDot;
                    maxPoint = v;
                }
            }

            return maxPoint;
        }

        private bool _OldIntersectsWithAABB(BoundingBox boundingBox)
        {
            if(this.Box.Intersects(boundingBox))
            {
                float x1, x2, y1, y2, z1, z2;
                x1 = x2 = y1 = y2 = z1 = z2 = 0.0f;

                float a, b;
                a = b = 0.0f;
                a = this.Box.Min.X - boundingBox.Min.X;
                b = this.Box.Min.X - boundingBox.Max.X;
                if(a > 0.0f && b < 0.0f)
                {
                    if(Math.Abs(a) < Math.Abs(b))
                    {
                        x1 = a;
                    }
                    else
                    {
                        x1 = b;
                    }
                }
                a = this.Box.Max.X - boundingBox.Max.X;
                b = this.Box.Max.X - boundingBox.Min.X;
                if(a < 0.0f && b > 0.0f)
                {
                    if (Math.Abs(a) < Math.Abs(b))
                    {
                        x2 = a;
                    }
                    else
                    {
                        x2 = b;
                    }
                }

                a = this.Box.Min.Y - boundingBox.Min.Y;
                b = this.Box.Min.Y - boundingBox.Max.Y;
                if (a > 0.0f && b < 0.0f)
                {
                    if (Math.Abs(a) < Math.Abs(b))
                    {
                        y1 = a;
                    }
                    else
                    {
                        y1 = b;
                    }
                }
                a = this.Box.Max.Y - boundingBox.Max.Y;
                b = this.Box.Max.Y - boundingBox.Min.Y;
                if (a < 0.0f && b > 0.0f)
                {
                    if (Math.Abs(a) < Math.Abs(b))
                    {
                        y2 = a;
                    }
                    else
                    {
                        y2 = b;
                    }
                }

                a = this.Box.Min.Z - boundingBox.Min.Z;
                b = this.Box.Min.Z - boundingBox.Max.Z;
                if (a > 0.0f && b < 0.0f)
                {
                    if (Math.Abs(a) < Math.Abs(b))
                    {
                        z1 = a;
                    }
                    else
                    {
                        z1 = b;
                    }
                }
                a = this.Box.Max.Z - boundingBox.Max.Z;
                b = this.Box.Max.Z - boundingBox.Min.Z;
                if (a < 0.0f && b > 0.0f)
                {
                    if (Math.Abs(a) < Math.Abs(b))
                    {
                        z2 = a;
                    }
                    else
                    {
                        z2 = b;
                    }
                }

                float x, y, z;
                x = y = z = 0.0f;
                x = FindValue(x1, x2);
                y = FindValue(y1, y2);
                z = FindValue(z1, z2);

                this.IntersectionVector = new Vector3(x, y, z);

                return true;
            }
            return false;
        }

        private float FindValue(float v1, float v2)
        {
            if (v1 == 0.0f)
            {
                return v2;
            }
            else if (v2 == 0.0f)
            {
                return v1;
            }
            else
            {
                return Math.Min(Math.Abs(v1), Math.Abs(v2));
            }
        }

        /// <summary>
        /// 
        /// This function updates collider so it can move when game object moves :)
        /// </summary>
        public override void UpdateCollider()
        {
            this.worldMatrix = this.MyObject.MyTransform.GetWorldMatrix();
            min = new Vector3(float.MaxValue, float.MaxValue, float.MaxValue);
            max = new Vector3(float.MinValue, float.MinValue, float.MinValue);

            for (int i = 0; i < corners.Length; ++i)
            {
                this.corners[i] = Vector3.Transform(this.initialCorners[i] * CustomScale + CustomOffset, this.worldMatrix);
                min = Vector3.Min(min, this.corners[i]);
                max = Vector3.Max(max, this.corners[i]);
            }
            this.Box = new BoundingBox(min, max);
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
