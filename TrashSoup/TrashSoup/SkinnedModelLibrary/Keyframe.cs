using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Content;

namespace SkinningModelLibrary
{
    /// <summary>
    /// Describes the position of a single bone at a single point in time
    /// </summary>
    public class Keyframe
    {
        #region properties

        [ContentSerializer]
        public int Bone { get; private set; }

        [ContentSerializer]
        public TimeSpan Time { get; private set; }

        [ContentSerializer]
        public Matrix Transform { get; private set; }

        #endregion

        #region methods

        public Keyframe(int bone, TimeSpan time, Matrix transform)
        {
            this.Bone = bone;
            this.Time = time;
            this.Transform = transform;
        }

        private Keyframe()
        {
        }

        #endregion
    }
}
