using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Content;

namespace SkinningModelLibrary
{
    /// <summary>
    /// Combines all the data needed to render and animate a skinned object
    /// </summary>
    public class SkinningData
    {
        #region properties

        [ContentSerializer]
        public Dictionary<string, AnimationClip> AnimationClips { get; set; }

        [ContentSerializer]
        public List<Matrix> BindPose { get; private set; }

        [ContentSerializer]
        public List<Matrix> InverseBindPose { get; private set; }

        [ContentSerializer]
        public List<int> SkeletonHierarchy { get; private set; }

        [ContentSerializer]
        public Dictionary<string, int> BoneNameToID { get; private set; }

        #endregion

        #region methods

        public SkinningData(Dictionary<string, AnimationClip> animationClips,
                            List<Matrix> bindPose, List<Matrix> inverseBindPose,
                            List<int> skeletonHierarchy, Dictionary<string, int> boneNameToID)
        {
            this.AnimationClips = animationClips;
            this.BindPose = bindPose;
            this.InverseBindPose = inverseBindPose;
            this.SkeletonHierarchy = skeletonHierarchy;
            this.BoneNameToID = boneNameToID;
        }

        private SkinningData()
        {
        }

        #endregion
    }
}
