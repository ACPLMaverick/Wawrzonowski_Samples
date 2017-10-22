using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace TrashSoup.Engine
{
    public class BoundingFrustumExtended : BoundingFrustum
    {
        #region constants

        public const float CLIP_MARGIN = 0.1f;

        #endregion

        #region variables

        /// <summary>
        /// This plane has no effect on either of frustum planes or the matrix. It's totally irrelevant to the frustum.
        /// </summary>
        public Plane AdditionalClip;

        #endregion

        #region methods

        public BoundingFrustumExtended(Matrix mat)
            : base(mat)
        {
            this.AdditionalClip = new Plane(new Vector4(0.0f, 0.0f, 0.0f, 0.0f));
        }

        public void ZeroAllAdditionals()
        {
            this.AdditionalClip.Normal.X = 0.0f;
            this.AdditionalClip.Normal.Y = 0.0f;
            this.AdditionalClip.Normal.Z = 0.0f;
            this.AdditionalClip.D = 0.0f;
        }

        #endregion
    }
}
