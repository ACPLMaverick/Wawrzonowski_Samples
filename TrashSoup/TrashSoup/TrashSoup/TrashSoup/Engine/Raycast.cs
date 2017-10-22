using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;

namespace TrashSoup.Engine
{
    public class Raycast
    {
        #region variables

        private List<Collider> allColliders;
        private Vector3 currentPos;

        #endregion

        #region properties

        public Vector3 Origin { get; private set; }
        public Vector3 Direction { get; private set; }
        public float MaxDistance { get; private set; }
        public float Step { get; private set; }

        public Vector3 PositionHit { get; private set; }
        public Collider ColliderHit { get; private set; }

        #endregion

        #region metods

        /// <summary>
        /// 
        /// </summary>
        /// <param name="origin"></param>
        /// <param name="dir">Must be normalized vector</param>
        /// <param name="md"></param>
        /// <param name="stp"></param>
        public Raycast(Vector3 origin, Vector3 dir, float md, float stp)
        {
            this.Origin = origin;
            this.Direction = dir;
            this.MaxDistance = md;
            this.Step = stp;

            this.currentPos = this.Origin;

            allColliders = PhysicsManager.Instance.AllColliders;
        }

        public bool Cast(Collider ignored = null)
        {
            while (Vector3.Distance(currentPos, Origin) < MaxDistance)
            {
                currentPos += Direction * Step;

                foreach (Collider collider in allColliders)
                {
                    if(!collider.MyObject.Enabled || !collider.Enabled || collider.IsTrigger || collider == ignored)
                    {
                        continue;
                    }

                    if (collider is SphereCollider)
                    {
                        SphereCollider sc = (SphereCollider)collider;
                        if (Vector3.Distance(sc.Sphere.Center, currentPos) <= sc.Sphere.Radius)
                        {
                            PositionHit = currentPos;
                            ColliderHit = collider;
                            return true;
                        }
                    }
                    else if (collider is BoxCollider)
                    {
                        BoxCollider bc = (BoxCollider)collider;
                        if(currentPos.X >= bc.Box.Min.X &&
                            currentPos.Y >= bc.Box.Min.Y &&
                            currentPos.Z >= bc.Box.Min.Z &&
                            currentPos.X <= bc.Box.Max.X &&
                            currentPos.Y <= bc.Box.Max.Y &&
                            currentPos.Z <= bc.Box.Max.Z)
                        {
                            PositionHit = currentPos;
                            ColliderHit = collider;
                            return true;
                        }
                    }
                }
            }
            PositionHit = currentPos;
            return false;
        }

        public Collider CastForCollisionOnly()
        {
            throw new NotImplementedException();
        }

        public Collider CastForTriggerOnly()
        {
            throw new NotImplementedException();
        }

        #endregion
    }
}
