using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.IO;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Content.Pipeline;
using Microsoft.Xna.Framework.Content.Pipeline.Graphics;
using Microsoft.Xna.Framework.Content.Pipeline.Processors;
using SkinningModelLibrary;

namespace SkinnedModelPipeline
{
    [ContentProcessor(DisplayName = "Skinned Custom Model Processor - TrashSoup")]
    public class SkinnedModelProcessor : CustomModelProcessor
    {
        #region properties

        [DefaultValue(MaterialProcessorDefaultEffect.SkinnedEffect)]
        public override MaterialProcessorDefaultEffect DefaultEffect
        {
            get
            {
                return MaterialProcessorDefaultEffect.SkinnedEffect;
            }
            set
            {
                
            }
        }

        #endregion

        #region methods

        /// <summary>
        /// Embeds animation data into ModelContent
        /// </summary>
        /// <param name="input"></param>
        /// <param name="context"></param>
        /// <returns></returns>
        public override ModelContent Process(NodeContent input, ContentProcessorContext context)
        {
            ValidateMesh(input, null);

            // find the skeletion

            BoneContent skeleton = MeshHelper.FindSkeleton(input);

            if (skeleton == null) throw new InvalidContentException("Input skeleton not found");

            // We don't want to have to worry about different parts of the model being in different
            // coordinate systems, so let's just bake everything
            FlattenTransforms(input, skeleton);

            // Read the bind pose and skeleton hierarchy data
            IList<BoneContent> bones = MeshHelper.FlattenSkeleton(skeleton);

            if(bones.Count > SkinnedEffect.MaxBones)
            {
                throw new InvalidContentException(string.Format(
                    "Skeleton has {0} bones, but the maximum supported is {1}.",
                    bones.Count, SkinnedEffect.MaxBones
                    ));
            }

            List<Matrix> bindPose = new List<Matrix>();
            List<Matrix> inverseBindPose = new List<Matrix>();
            List<int> skeletonHierarchy = new List<int>();
            Dictionary<string, int> assignement = new Dictionary<string, int>();

            int ctr = 0;
            foreach(BoneContent bone in bones)
            {
                assignement.Add(bone.Name, ctr);
                bindPose.Add(bone.Transform);
                inverseBindPose.Add(Matrix.Invert(bone.AbsoluteTransform));
                skeletonHierarchy.Add(bones.IndexOf(bone.Parent as BoneContent));
                ++ctr;
            }

            // Convert animation data to our runtime format
            Dictionary<string, AnimationClip> animationClips;
            animationClips = ProcessAnimations(skeleton.Animations, bones);

            // Chain to teh base ModelProcessor class so it can convert the model data
            ModelContent model = base.Process(input, context);

            // Store our custon animation data in the Tag property of the model
            object[] intoTag;
            try
            {
                intoTag = (object[])model.Tag;
            }
            catch(InvalidCastException e)
            {
                Console.Out.WriteLine(e.Message);
                model.Tag = null;
                return model;
            }
            intoTag[0] = new SkinningData(animationClips, bindPose, inverseBindPose, skeletonHierarchy, assignement);
            model.Tag = intoTag;

            return model;
        }

        /// <summary>
        /// Converts an intermediate format content pipeline AnimationContentDictionary object
        /// to our runtime AnimationClip format
        /// </summary>
        /// <param name="animations"></param>
        /// <param name="bones"></param>
        /// <returns></returns>
        static Dictionary<string, AnimationClip> ProcessAnimations(
            AnimationContentDictionary animations, IList<BoneContent> bones)
        {
            // build up a table mapping bone names to indices
            Dictionary<string, int> boneMap = new Dictionary<string, int>();

            for(int i = 0; i < bones.Count; ++i)
            {
                string boneName = bones[i].Name;

                if(!string.IsNullOrEmpty(boneName))
                {
                    boneMap.Add(boneName, i);
                }
                
            }

            // convert each animation in turn
            Dictionary<string, AnimationClip> animationClips;
            animationClips = new Dictionary<string, AnimationClip>();

            int j = 0;
            foreach(KeyValuePair<string, AnimationContent> animation in animations)
            {
                AnimationClip processed = ProcessAnimation(animation.Value, boneMap);
                animationClips.Add("DefaultAnim" + j.ToString(), processed);
                ++j;
            }

            if(animationClips.Count == 0)
            {
                throw new InvalidContentException("Input file does not contain animations");
            }

            return animationClips;
        }

        /// <summary>
        /// Converts an intermediate format content pipeline AnimationContent
        /// into our runtime AnimationClip format
        /// </summary>
        /// <param name="animation"></param>
        /// <param name="boneMap"></param>
        /// <returns></returns>
        static AnimationClip ProcessAnimation(AnimationContent animation, Dictionary<string, int> boneMap)
        {
            List<Keyframe> keyframes = new List<Keyframe>();

            int boneIndex;
            Matrix trans;
            // for each input animation channel
            foreach(KeyValuePair<string, AnimationChannel> channel in animation.Channels)
            {
                // look up what bone this channel is controlling
                if(!boneMap.TryGetValue(channel.Key, out boneIndex))
                {
                    throw new InvalidContentException(string.Format(
                        "Found animation for bone {0}, which is not a part of the skeleton",
                        channel.Key
                        ));
                }

                // convert the keyframe data
                foreach(AnimationKeyframe keyframe in channel.Value)
                {
                    keyframes.Add(new Keyframe(boneIndex, keyframe.Time, keyframe.Transform));
                }
            }

            // sort the merged keyframes by time
            keyframes.Sort(CompareKeyframeTimes);

            if (keyframes.Count == 0) throw new InvalidContentException("Animation has no keyframes.");
            if (animation.Duration <= TimeSpan.Zero) throw new InvalidContentException("Animation has an invalid duration.");

            return new AnimationClip(animation.Duration, keyframes);
        }

        /// <summary>
        /// For sorting keyframes into ascending time order
        /// </summary>
        /// <param name="a"></param>
        /// <param name="b"></param>
        /// <returns></returns>
        static int CompareKeyframeTimes(Keyframe a, Keyframe b)
        {
            return a.Time.CompareTo(b.Time);
        }

        /// <summary>
        /// Makes sure this mesh contains the kind of data we know how to animate
        /// </summary>
        /// <param name="node"></param>
        /// <param name="context"></param>
        /// <param name="parentBoneName"></param>
        static void ValidateMesh(NodeContent node, string parentBoneName)
        {
            MeshContent mesh = node as MeshContent;

            if(mesh != null)
            {
                // validate the mesh
                if(parentBoneName != null)
                {
                    //Debug.Log(string.Format("WARNING: Mesh {0} is a child of bone {1}. This is not supported.", mesh.Name, parentBoneName));
                }

                if(!MeshHasSkinning(mesh))
                {
                    //Debug.Log(string.Format("WARNING: Mesh {0} has no skinning information, so it has been deleted.", mesh.Name));
                    mesh.Parent.Children.Remove(mesh);
                    return;
                }
            }
            else if(node is BoneContent)
            {
                // if this is a bone, remember that we are now looking inside it
                parentBoneName = node.Name;
            }

            // Recurse
            foreach(NodeContent child in new List<NodeContent>(node.Children))
            {
                ValidateMesh(child, parentBoneName);
            }
        }

        /// <summary>
        /// Checks whether mesh contains skinning information.
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        static bool MeshHasSkinning(MeshContent mesh)
        {
            foreach(GeometryContent geometry in mesh.Geometry)
            {
                if(!geometry.Vertices.Channels.Contains(VertexChannelNames.Weights()))
                {
                    return false;
                }
            }
            return true;
        }


        /// <summary>
        /// Bakes unwanted transforms into the model geometry, so everything
        /// ends up in the same coordinate system.
        /// </summary>
        /// <param name="node"></param>
        /// <param name="skeleton"></param>
        static void FlattenTransforms(NodeContent node, BoneContent skeleton)
        {
            foreach(NodeContent child in node.Children)
            {
                // Don't process skeleton because he's special
                if (child == skeleton) continue;

                // Bake the local transform into the actual geometry
                MeshHelper.TransformScene(child, child.Transform);

                // Having baked it, we can now set the local coordinate system back to identity
                child.Transform = Matrix.Identity;

                // Recurse
                FlattenTransforms(child, skeleton);
            }
        }

        #endregion
    }
}
