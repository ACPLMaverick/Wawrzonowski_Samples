using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace TrashSoup.Engine
{
    public struct RectangleWS
    {
        public Vector3 Min;
        public Vector3 Max;

        public RectangleWS(Vector3 min, Vector3 max)
        {
            this.Min = min;
            this.Max = max;
        }

        public bool IntersectsPlane(ref Plane plane)
        {
            Vector3 planePoint = plane.Normal * plane.D;
            Vector3[] points = new Vector3[4];
            Vector3 pointBR = new Vector3(this.Min.X + Math.Abs(this.Max.X - this.Min.X), this.Min.Y, this.Min.Z);
            Vector3 pointTL = new Vector3(this.Max.X - Math.Abs(this.Max.X - this.Min.X), this.Max.Y, this.Max.Z);

            points[0] = Vector3.Normalize(planePoint - this.Min);
            points[1] = Vector3.Normalize(planePoint - this.Max);
            points[2] = Vector3.Normalize(planePoint - pointBR);
            points[3] = Vector3.Normalize(planePoint - pointTL);

            for (int i = 0; i < 4; ++i)
            {
                float angle = Math.Abs((float)Math.Acos(MathHelper.Clamp(Vector3.Dot(plane.Normal, points[i]), -1.0f, 1.0f)));

                if (angle >= MathHelper.PiOver2)
                {
                    return true;
                }
            }

            return false;
        }
    }

    public class QuadTreeNode
    {
        #region properties

        public QuadTreeNode Parent { get; set; }
        public List<GameObject> Objects { get; set; }
        public RectangleWS Rect;
        public QuadTreeNode ChildTL { get; set; }
        public QuadTreeNode ChildTR { get; set; }
        public QuadTreeNode ChildBR { get; set; }
        public QuadTreeNode ChildBL { get; set; }
        
        #endregion

        #region methods
        public QuadTreeNode(QuadTreeNode parent, RectangleWS rect)
        {
            this.Parent = parent;
            this.Objects = new List<GameObject>();
            this.Rect = rect;
            this.ChildTL = null;
            this.ChildTR = null;
            this.ChildBR = null;
            this.ChildBL = null;
        }

        public RectangleWS GetRectForChildTL()
        {
            float distX = Math.Abs(Rect.Max.X - Rect.Min.X);
            float distZ = Math.Abs(Rect.Max.Z - Rect.Min.Z);

            distX /= 2.0f;
            distZ /= 2.0f;

            return new RectangleWS(new Vector3(Rect.Min.X, Rect.Min.Y, Rect.Min.Z + distZ), new Vector3(Rect.Max.X - distX, Rect.Max.Y, Rect.Max.Z));
        }

        public RectangleWS GetRectForChildTR()
        {
            float distX = Math.Abs(Rect.Max.X - Rect.Min.X);
            float distZ = Math.Abs(Rect.Max.Z - Rect.Min.Z);

            distX /= 2.0f;
            distZ /= 2.0f;

            return new RectangleWS(new Vector3(Rect.Min.X + distX, Rect.Min.Y, Rect.Min.Z + distZ), Rect.Max);
        }

        public RectangleWS GetRectForChildBR()
        {
            float distX = Math.Abs(Rect.Max.X - Rect.Min.X);
            float distZ = Math.Abs(Rect.Max.Z - Rect.Min.Z);

            distX /= 2.0f;
            distZ /= 2.0f;

            return new RectangleWS(new Vector3(Rect.Min.X + distX, Rect.Min.Y, Rect.Min.Z), new Vector3(Rect.Max.X, Rect.Max.Y, Rect.Max.Z - distZ));
        }

        public RectangleWS GetRectForChildBL()
        {
            float distX = Math.Abs(Rect.Max.X - Rect.Min.X);
            float distZ = Math.Abs(Rect.Max.Z - Rect.Min.Z);

            distX /= 2.0f;
            distZ /= 2.0f;

            return new RectangleWS(Rect.Min, new Vector3(Rect.Max.X - distX, Rect.Max.Y, Rect.Max.Z - distZ));
        }
        #endregion
    }

    public class QuadTree
    {
        #region constants
        private const uint PLANE_COUNT = 4;
        #endregion

        #region variables
        private QuadTreeNode root;
        private float sceneSize;
        private Dictionary<uint, GameObject> objs;
        private List<GameObject> dynamicObjects;
        private List<GameObject> lastDraws;
        private Plane[] planesToCheck;
        Stack<QuadTreeNode> nodesToCheck;
        #endregion

        #region methods
        public QuadTree(Dictionary<uint, GameObject> objs, float sceneSize)
        {
            this.objs = objs;
            this.sceneSize = sceneSize;
            this.dynamicObjects = new List<GameObject>();
            this.lastDraws = new List<GameObject>();
            this.planesToCheck = new Plane[PLANE_COUNT];
            this.nodesToCheck = new Stack<QuadTreeNode>();
            root = new QuadTreeNode(null, new RectangleWS(new Vector3(-sceneSize / 2.0f, 0.0f, -sceneSize / 2.0f), new Vector3(sceneSize / 2.0f, 0.0f, sceneSize / 2.0f)));
        }


        public void Update()
        {
            int count = dynamicObjects.Count;
            RectangleWS currentRect;
            QuadTreeNode current;
            for(int i = 0; i < count; ++i)
            {
                if(dynamicObjects[i].MyNode == null)
                {
                    this.Add(dynamicObjects[i]);
                }

                currentRect = GenerateRectangle(dynamicObjects[i]);
                current = dynamicObjects[i].MyNode;

                if(CheckIfObjectFits(ref current.Rect, ref currentRect))
                {
                    // we still fit - continue
                    continue;
                }
                else
                {
                    if (current.Parent == null)
                    {
                        continue;
                    }
                    // remove ourselves
                    current.Objects.Remove(dynamicObjects[i]);
                    dynamicObjects[i].MyNode = null;

                    // check parents
                    nodesToCheck.Clear();
                    nodesToCheck.Push(current.Parent);
                    while(nodesToCheck.Count > 0)
                    {
                        current = nodesToCheck.Pop();

                        // check if we fit
                        if(current != null && CheckIfObjectFits(ref current.Rect, ref currentRect))
                        {
                            // YESS! but let's check if we fit in children first
                            if(current.ChildBL != null)
                            {
                                if (CheckIfObjectFits(ref current.ChildBL.Rect, ref currentRect))
                                {
                                    nodesToCheck.Push(current.ChildBL);
                                    continue;
                                }
                            }
                            else
                            {
                                RectangleWS tempRect = current.GetRectForChildBL();
                                if(CheckIfObjectFits(ref tempRect, ref currentRect))
                                {
                                    current.ChildBL = new QuadTreeNode(current, tempRect);
                                    nodesToCheck.Push(current.ChildBL);
                                    continue;
                                }
                            }
                            
                            if(current.ChildBR != null)
                            {
                                if (CheckIfObjectFits(ref current.ChildBR.Rect, ref currentRect))
                                {
                                    nodesToCheck.Push(current.ChildBR);
                                    continue;
                                }
                            }
                            else
                            {
                                RectangleWS tempRect = current.GetRectForChildBR();
                                if (CheckIfObjectFits(ref tempRect, ref currentRect))
                                {
                                    current.ChildBR = new QuadTreeNode(current, tempRect);
                                    nodesToCheck.Push(current.ChildBR);
                                    continue;
                                }
                            }

                            if(current.ChildTL != null)
                            {
                                if (CheckIfObjectFits(ref current.ChildTL.Rect, ref currentRect))
                                {
                                    nodesToCheck.Push(current.ChildTL);
                                    continue;
                                }
                            }
                            else
                            {
                                RectangleWS tempRect = current.GetRectForChildTL();
                                if (CheckIfObjectFits(ref tempRect, ref currentRect))
                                {
                                    current.ChildTL = new QuadTreeNode(current, tempRect);
                                    nodesToCheck.Push(current.ChildTL);
                                    continue;
                                }
                            }

                            if(current.ChildTR != null)
                            {
                                if (CheckIfObjectFits(ref current.ChildTR.Rect, ref currentRect))
                                {
                                    nodesToCheck.Push(current.ChildTR);
                                    continue;
                                }
                            }
                            else
                            {
                                RectangleWS tempRect = current.GetRectForChildTR();
                                if (CheckIfObjectFits(ref tempRect, ref currentRect))
                                {
                                    current.ChildTR = new QuadTreeNode(current, tempRect);
                                    nodesToCheck.Push(current.ChildTR);
                                    continue;
                                }
                            }

                            // we don't fit in children, so that's the end of our beautiful journey
                            current.Objects.Add(dynamicObjects[i]);
                            dynamicObjects[i].MyNode = current;
                            break;

                        }
                        else if(current != null)
                        {
                            // nope, let's push parent
                            nodesToCheck.Push(current.Parent);
                        }
                    }
                }
            }
        }

        public void Draw(Camera cam, Effect effect, GameTime gameTime)
        {
            planesToCheck[0] = cam.Bounds.Near;
            planesToCheck[1] = cam.Bounds.Far;
            planesToCheck[2] = cam.Bounds.Left;
            planesToCheck[3] = cam.Bounds.Right;

            lastDraws.Clear();

            for(int i = 0; i < PLANE_COUNT; ++i)
            {
                planesToCheck[i].Normal.X = - planesToCheck[i].Normal.X;
                planesToCheck[i].Normal.Y = -planesToCheck[i].Normal.Y;
                planesToCheck[i].Normal.Z = -planesToCheck[i].Normal.Z;
                //planesToCheck[i].D = planesToCheck[i].D + 0.5f;
                planesToCheck[i].Normal = Vector3.Normalize(planesToCheck[i].Normal);
            }

            QuadTreeNode current;
            nodesToCheck.Clear();
            nodesToCheck.Push(root);

            uint ctr = 0;

            while(nodesToCheck.Count > 0)
            {
                // pop from stack
                current = nodesToCheck.Pop();

                // draw all objects of current node

                if(current.Objects.Count != 0)
                {
                    int cCount = current.Objects.Count;
                    for (int i = 0; i < cCount; ++i )
                    {
                        if (!current.Objects[i].DrawLast)
                        {
                            current.Objects[i].Draw(cam, effect, gameTime);
                        }
                        else
                        {
                            lastDraws.Add(current.Objects[i]);
                        }

                        ++ctr;
                    }
                }

                // check collision for each child node
                if(current.ChildBL != null && 
                    current.ChildBL.Rect.IntersectsPlane(ref planesToCheck[0]) &&
                    current.ChildBL.Rect.IntersectsPlane(ref planesToCheck[1]) &&
                    current.ChildBL.Rect.IntersectsPlane(ref planesToCheck[2]) &&
                    current.ChildBL.Rect.IntersectsPlane(ref planesToCheck[3]))
                {
                    nodesToCheck.Push(current.ChildBL);
                }

                if (current.ChildBR != null &&
                    current.ChildBR.Rect.IntersectsPlane(ref planesToCheck[0]) &&
                    current.ChildBR.Rect.IntersectsPlane(ref planesToCheck[1]) &&
                    current.ChildBR.Rect.IntersectsPlane(ref planesToCheck[2]) &&
                    current.ChildBR.Rect.IntersectsPlane(ref planesToCheck[3]))
                {
                    nodesToCheck.Push(current.ChildBR);
                }

                if (current.ChildTL != null &&
                    current.ChildTL.Rect.IntersectsPlane(ref planesToCheck[0]) &&
                    current.ChildTL.Rect.IntersectsPlane(ref planesToCheck[1]) &&
                    current.ChildTL.Rect.IntersectsPlane(ref planesToCheck[2]) &&
                    current.ChildTL.Rect.IntersectsPlane(ref planesToCheck[3]))
                {
                    nodesToCheck.Push(current.ChildTL);
                }

                if (current.ChildTR != null &&
                    current.ChildTR.Rect.IntersectsPlane(ref planesToCheck[0]) &&
                    current.ChildTR.Rect.IntersectsPlane(ref planesToCheck[1]) &&
                    current.ChildTR.Rect.IntersectsPlane(ref planesToCheck[2]) &&
                    current.ChildTR.Rect.IntersectsPlane(ref planesToCheck[3]))
                {
                    nodesToCheck.Push(current.ChildTR);
                }
            }

            //Cause there was exception (index out of range)
             for (int i = 0; i < lastDraws.Count; ++i)
             {
                 lastDraws[i].Draw(cam, effect, gameTime);
             }

            //if (effect == null)
            //    Debug.Log("QUADTREE: Objects drawn: " + ctr.ToString() + ", objects total: " + ResourceManager.Instance.CurrentScene.ObjectsDictionary.Count.ToString());
        }

        private bool CheckIfObjectFits(ref RectangleWS rectOut, ref RectangleWS rectIn)
        {
            if (rectIn.Min.X >= rectOut.Min.X &&
                rectIn.Min.Z >= rectOut.Min.Z &&
                rectIn.Max.X <= rectOut.Max.X &&
                rectIn.Max.Z <= rectOut.Max.Z)
            {
                return true;
            }
            else return false;
        }

        public void Add(GameObject obj)
        {
            QuadTreeNode current;
            QuadTreeNode parent;
            bool finished;

            current = root;
            parent = null;
            finished = false;

            RectangleWS rectObj = GenerateRectangle(obj);

            uint ctr = 0;
            while (!finished)
            {
                ++ctr;

                // generate rects for leaf nodes
                RectangleWS childTLRect = current.GetRectForChildTL();
                RectangleWS childTRRect = current.GetRectForChildTR();
                RectangleWS childBRRect = current.GetRectForChildBR();
                RectangleWS childBLRect = current.GetRectForChildBL();

                // check if we fit in any
                if (CheckIfObjectFits(ref childTLRect, ref rectObj))
                {
                    // yay we fit, let's generate that child if necessary and proceed further
                    if (current.ChildTL == null)
                    {
                        current.ChildTL = new QuadTreeNode(current, childTLRect);
                        current.ChildTL.Parent = current;
                    }
                    parent = current;
                    current = current.ChildTL;
                    continue;
                }
                else if (CheckIfObjectFits(ref childTRRect, ref rectObj))
                {
                    if (current.ChildTR == null)
                    {
                        current.ChildTR = new QuadTreeNode(current, childTRRect);
                        current.ChildTR.Parent = current;
                    }
                    parent = current;
                    current = current.ChildTR;
                    continue;
                }
                else if (CheckIfObjectFits(ref childBRRect, ref rectObj))
                {
                    if (current.ChildBR == null)
                    {
                        current.ChildBR = new QuadTreeNode(current, childBRRect);
                        current.ChildBR.Parent = current;
                    }
                    parent = current;
                    current = current.ChildBR;
                    continue;
                }
                else if (CheckIfObjectFits(ref childBLRect, ref rectObj))
                {
                    if (current.ChildBL == null)
                    {
                        current.ChildBL = new QuadTreeNode(current, childBLRect);
                        current.ChildBL.Parent = current;
                    }
                    parent = current;
                    current = current.ChildBL;
                    continue;
                }
                else
                {
                    // holy shit we dont fit in any so let's add ourselves to current node
                    current.Objects.Add(obj);
                    obj.MyNode = current;
                    if(obj.Dynamic)
                    {
                        dynamicObjects.Add(obj);
                    }
                    finished = true;
                    Debug.Log("Object " + obj.Name + " is on level " + ctr.ToString());

                    foreach(GameObject child in obj.GetChildren())
                    {
                        Add(child);
                    }

                    break;
                }
            }
        }

        public void Remove(GameObject obj)
        {
            if(obj.MyNode != null)
            {
                Stack<GameObject> objStack = new Stack<GameObject>();
                objStack.Push(obj);
                GameObject temp;
                while(objStack.Count > 0)
                {
                    temp = objStack.Pop();
                    if(temp.MyNode != null)
                    {
                        temp.MyNode.Objects.Remove(obj);
                        temp.MyNode = null;
                    }

                    foreach(GameObject child in temp.GetChildren())
                    {
                        objStack.Push(child);
                    }
                }
                
            }
            else
            {
                Debug.Log("QUADTREE: Something went srsly wrong while removing object");
            }
        }

        public QuadTreeNode GetRoot()
        {
            return root;
        }

        public void AddDynamic(GameObject obj)
        {
            if(!dynamicObjects.Contains(obj) && obj.MyNode != null)
            {
                dynamicObjects.Add(obj);
            }
        }

        public void RemoveDynamic(GameObject obj)
        {
            if (dynamicObjects.Contains(obj) && obj.MyNode != null)
            {
                dynamicObjects.Remove(obj);
            }
        }

        private RectangleWS GenerateRectangle(GameObject obj)
        {
            RectangleWS rectObj = new RectangleWS();
            SphereCollider tempSphereCollider;
            BoxCollider tempBoxCollider;

            if (obj.MyCollider is BoxCollider)
            {
                tempBoxCollider = (BoxCollider)obj.MyCollider;

                rectObj.Min = tempBoxCollider.Box.Min;
                rectObj.Max = tempBoxCollider.Box.Max;
                rectObj.Min.Y = 0.0f;
                rectObj.Max.Y = 0.0f;
                //if(rectObj.Min.Z > rectObj.Max.Z)
                //{
                //    rectObj.Max.Z = -rectObj.Max.Z;
                //    rectObj.Min.Z = -rectObj.Min.Z;
                //}
            }
            else if (obj.MyCollider is SphereCollider)
            {
                tempSphereCollider = (SphereCollider)obj.MyCollider;
                rectObj.Min = Vector3.Zero;
                rectObj.Max = Vector3.Zero;

                rectObj.Min.X = tempSphereCollider.Sphere.Center.X - tempSphereCollider.Sphere.Radius;
                rectObj.Min.Z = tempSphereCollider.Sphere.Center.Z - tempSphereCollider.Sphere.Radius;
                rectObj.Max.X = tempSphereCollider.Sphere.Center.X + tempSphereCollider.Sphere.Radius;
                rectObj.Max.Z = tempSphereCollider.Sphere.Center.Z + tempSphereCollider.Sphere.Radius;
            }
            else
            {
                Debug.Log("QUADTREE: Collider not found for object: " + obj.Name + ", ID " + obj.UniqueID.ToString());
                rectObj.Min = root.Rect.Min;
                rectObj.Max = root.Rect.Max;

                return rectObj;
            }

            //float diffX = rectObj.Max.X - rectObj.Min.X;
            //float diffZ = rectObj.Max.Z - rectObj.Min.Z;

            //if (diffX > diffZ)
            //{
            //    float diffTotal = (diffX - diffZ) / 2.0f;
            //    rectObj.Min.Z -= diffTotal;
            //    rectObj.Max.Z += diffTotal;
            //}
            //else if (diffZ > diffX)
            //{
            //    float diffTotal = (diffZ - diffX) / 2.0f;
            //    rectObj.Min.X -= diffTotal;
            //    rectObj.Max.X += diffTotal;
            //}

            return rectObj;
        }

        private bool CheckIntersection(ref Plane plane, ref RectangleWS rect)
        {
            Vector3 planePoint = plane.Normal * plane.D;
            Vector3 vecMin = planePoint - rect.Min;
            Vector3 vecMax = planePoint - rect.Max;

            vecMin = Vector3.Normalize(vecMin);
            vecMax = Vector3.Normalize(vecMax);

            float angleMin = Math.Abs( (float) Math.Acos((double)Vector3.Dot(plane.Normal, vecMin)));
            float angleMax = Math.Abs( (float) Math.Acos((double)Vector3.Dot(plane.Normal, vecMax)));

            bool ifMin = angleMin >= MathHelper.PiOver2;
            bool ifMax = angleMax >= MathHelper.PiOver2;

            if(angleMin >= MathHelper.PiOver2 || angleMax >= MathHelper.PiOver2)
            {
                return true;
            }
            else return false;
        }
        #endregion
    }
}
