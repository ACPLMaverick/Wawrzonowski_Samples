using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework.Content;

namespace SkinningModelLibrary
{
    /// <summary>
    /// Holds all the keyframes needed to describe a single animation.
    /// </summary>
    public class AnimationClip
    {
        #region properties

        [ContentSerializer]
        public TimeSpan Duration { get; private set; }

        [ContentSerializer]
        public List<Keyframe> Keyframes { get; private set; }

        #endregion

        #region methods

        public AnimationClip(TimeSpan duration, List<Keyframe> keyframes)
        {
            this.Duration = duration;
            this.Keyframes = keyframes;
        }

        private AnimationClip()
        {
        }

        #endregion
    }
}
