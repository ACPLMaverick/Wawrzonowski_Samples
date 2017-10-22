using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrashSoup.Engine
{
    public enum FaceType
    {
        MaxX, MinX,
        MaxY, MinY,
        MaxZ, MinZ
    }

    public class Face
    {
        #region Variables

        private Vector3 v1;
        private Vector3 v2;
        private Vector3 v3;
        private Vector3 v4;

        #endregion

        #region Properties

        public Vector3 Normal
        {
            get;
            protected set;
        }

        #endregion

        #region Methods

        public Face(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, FaceType fType)
        {
            this.v1 = v1;
            this.v2 = v2;
            this.v3 = v3;
            this.v4 = v4;
            this.Normal = Vector3.Cross(v2 - v1, v2 - v3);
            this.Normal /= this.Normal.Length();
            switch(fType)
            {
                case FaceType.MaxX:
                    if (this.Normal.X < 0.0f) this.Normal *= -1.0f;
                    break;
                case FaceType.MaxY:
                    if (this.Normal.Y < 0.0f) this.Normal *= -1.0f;
                    break;
                case FaceType.MaxZ:
                    if (this.Normal.Z < 0.0f) this.Normal *= -1.0f;
                    break;
                case FaceType.MinX:
                    if (this.Normal.X > 0.0f) this.Normal *= -1.0f;
                    break;
                case FaceType.MinY:
                    if (this.Normal.Y > 0.0f) this.Normal *= -1.0f;
                    break;
                case FaceType.MinZ:
                    if (this.Normal.Z > 0.0f) this.Normal *= -1.0f;
                    break;
            }
        }

        #endregion
    }
}
