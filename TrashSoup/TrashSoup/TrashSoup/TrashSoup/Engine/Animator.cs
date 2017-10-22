using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SkinningModelLibrary;

namespace TrashSoup.Engine
{
    public class Animator : ObjectComponent, IXmlSerializable
    {
        #region variables

        public Dictionary<string, AnimationPlayer> animationPlayers = new Dictionary<string, AnimationPlayer>();
        protected bool ifInterpolate = false;
        protected int currentInterpolationTimeMS = 0;
        protected float interDirection = 1.0f;

        protected float currentInterpolation;
        protected float sentInterpolation;

        protected bool ifHelperInterpolate = false;
        protected int hCurrentInterpolationTimeMS = 0;
        protected float hInterDirection = 1.0f;
        protected float hCurrentInterpolation = 0.0f;

        #endregion

        #region properties

        public Model BaseAnim { get; set; }
        public SkinningData SkinningData { get; set; }
        public AnimatorState CurrentState { get; set; }
        public AnimatorState NewState { get; set; }
        public AnimatorState ThirdState { get; set; }
        public Dictionary<string, AnimatorState> AvailableStates { get; set; }

        public float CurrentInterpolation 
        { 
            get
            {
                return currentInterpolation;
            }
            set
            {
                if(!this.ifInterpolate)
                {
                    currentInterpolation = value;
                }
                else
                {
                    sentInterpolation = value;
                }
            }
        }

        #endregion

        #region methods

        public Animator(GameObject go) : base(go)
        {
            AvailableStates = new Dictionary<string, AnimatorState>();
            this.CurrentInterpolation = 0.0f;
        }

        public Animator(GameObject go, Model baseAnim) : base(go)
        {
            this.CurrentInterpolation = 0.0f;
            BaseAnim = baseAnim;
            SkinningData = (baseAnim.Tag as object[])[0] as SkinningData;
            if (SkinningData == null) throw new InvalidOperationException("LOD 0 doesn't contain skinning data tag");
            AvailableStates = new Dictionary<string, AnimatorState>();
            CurrentState = null;
            NewState = null;
        }

        public Animator(GameObject go, Animator anim) : base(go)
        {
            this.CurrentInterpolation = anim.CurrentInterpolation;
            this.BaseAnim = anim.BaseAnim;
            this.SkinningData = anim.SkinningData;
            this.AvailableStates = new Dictionary<string, AnimatorState>();
            foreach(AnimatorState astate in anim.AvailableStates.Values)
            {
                this.AvailableStates.Add(astate.Name, astate);
            }
            this.animationPlayers = new Dictionary<string, AnimationPlayer>();
            foreach(AnimationPlayer ap in anim.animationPlayers.Values)
            {
                this.animationPlayers.Add(ap.AnimationKey, ap);
            }
            this.CurrentState = anim.CurrentState;
            this.NewState = null;
        }

        public override void Update(GameTime gameTime)
        {
            if(ifInterpolate)
            {
                CalculateInterpolationAmount(gameTime, this.currentInterpolationTimeMS, this.interDirection, ref this.currentInterpolation);
                if(currentInterpolation >= 1.0f)
                {
                    this.CurrentState = NewState;
                    this.NewState = null;
                    this.currentInterpolation = 0.0f;
                    this.currentInterpolationTimeMS = 0;
                    this.ifInterpolate = false;
                }
                else if(currentInterpolation <= 0.0f)
                {
                    this.interDirection = 1.0f;
                    this.NewState = null;
                    this.currentInterpolation = 0.0f;
                    this.currentInterpolationTimeMS = 0;
                    this.ifInterpolate = false;
                }
            }

            if (ifHelperInterpolate)
            {
                //Debug.Log(this.hCurrentInterpolation.ToString() + " " + gameTime.TotalGameTime.Seconds.ToString());
                CalculateInterpolationAmount(gameTime, this.hCurrentInterpolationTimeMS, this.hInterDirection, ref this.hCurrentInterpolation);
                if (hCurrentInterpolation >= 1.0f && this.hInterDirection == 1.0f) // we've interpolated TO and playing this animation atm
                {
                    this.ifHelperInterpolate = false;
                }
                else if(hCurrentInterpolation <= 0.0f && this.hInterDirection == -1.0f)  // we've ended this animation permanently
                {
                    this.ThirdState.Animation.StopClip();
                    this.ThirdState = null;
                    this.hCurrentInterpolation = 0.0f;
                    this.hCurrentInterpolationTimeMS = 0;
                    this.hInterDirection = 1.0f;
                    this.ifHelperInterpolate = false;
                }
            }

            if (CurrentState != null)
            {
                CurrentState.Update(gameTime);
            }
            if (NewState != null)
            {
                NewState.Update(gameTime);
            }
            if (ThirdState != null)
            {
                ThirdState.Update(gameTime);
            }

            if(ThirdState != null && !ifHelperInterpolate)
            {
                // if animation is about to finish, let's interpolate back to original two states
                // first let's check time we are in now and compare it with "Back" animation transition time
                TimeSpan backTime = ThirdState.GetTimeByTransition(CurrentState);
                if(backTime == TimeSpan.Zero)
                {
                    Debug.Log("ANIMATOR ERROR: ThirdState cannot make a transition to CurrentState, because the latter is not on its transition list.");
                    return;
                }

                TimeSpan currentTime = ThirdState.Animation.CurrentTime;
                TimeSpan durationTime = ThirdState.Animation.CurrentClip.Duration;

                if(currentTime >= (durationTime - backTime))
                {
                    // now launch interpolation back
                    this.hInterDirection = -1.0f;
                    this.hCurrentInterpolationTimeMS = (int) backTime.TotalMilliseconds;
                    this.ifHelperInterpolate = true;
                    this.hCurrentInterpolation = 0.9999f;
                }
            }

            //Debug.Log((CurrentState != null ? CurrentState.Name : "null") + " " +
               // (NewState != null ? NewState.Name : "null") + " " + gameTime.TotalGameTime.Seconds.ToString());
        }

        public override void Draw(Camera cam, Effect effect, GameTime gameTime)
        {
            // draw nothing
        }

        protected override void Start()
        {
            // do nothing again
        }

        public Matrix[] GetSkinTransforms()
        {
            Matrix[] toReturn = GetTransformsInterpolated((CurrentState != null) ? CurrentState.Animation.GetSkinTransforms() : null,
                (NewState != null) ? NewState.Animation.GetSkinTransforms() : null, currentInterpolation);

            if(ThirdState != null)
            {
                return GetTransformsInterpolated(toReturn, ThirdState.Animation.GetSkinTransforms(), hCurrentInterpolation);
            }
            else
            {
                return toReturn;
            }
        }

        public Matrix[] GetWorldTransforms()
        {
            Matrix[] toReturn = GetTransformsInterpolated((CurrentState != null) ? CurrentState.Animation.GetWorldTransforms() : null,
                (NewState != null) ? NewState.Animation.GetWorldTransforms() : null, currentInterpolation);

            if (ThirdState != null)
            {
                return GetTransformsInterpolated(toReturn, ThirdState.Animation.GetWorldTransforms(), hCurrentInterpolation);
            }
            else
            {
                return toReturn;
            }
        }

        public void AddAnimationClip(KeyValuePair<string, AnimationClip> newClip)
        {
            if(!this.SkinningData.AnimationClips.ContainsKey(newClip.Key))
            {
                this.SkinningData.AnimationClips.Add(newClip.Key, newClip.Value);
            }
            this.animationPlayers.Add(newClip.Key, new AnimationPlayer(this.SkinningData, newClip.Key));
        }

        public void RemoveAnimatonClip(string key)
        {
            this.SkinningData.AnimationClips.Remove(key);
            animationPlayers.Remove(key);
        }

        public AnimationClip GetAnimationClip(string key)
        {
            return SkinningData.AnimationClips[key];
        }

        public AnimationPlayer GetAnimationPlayer(string key)
        {
            if(animationPlayers.ContainsKey(key))
            {
                return animationPlayers[key];
            }
            return null;
        }

        public void StartAnimation()
        {
            CurrentState.Animation.StartClip();
        }

        public void StopAnimation()
        {
            CurrentState.Animation.StopClip();
        }

        public void PauseAnimation()
        {
            CurrentState.Animation.PauseClip();
        }

        public void ChangeState(string stateName)
        {
            AnimatorState newS = null;
            TimeSpan tmpMS = TimeSpan.Zero;

            KeyValuePair<TimeSpan, AnimatorState> tmpPair = this.CurrentState.GetTransitionAndTimeByName(stateName);
            newS = tmpPair.Value;
            tmpMS = tmpPair.Key;

            if (newS == null)
            {
                Debug.Log("ANIMATOR ERROR: Transition not found in current state's dictionary");
                return;
            }

            if (newS.Type == AnimatorState.StateType.SINGLE)
            {
                //tmpTransforms = GetSkinTransforms();
                newS.IsFinished = false;
                this.ThirdState = newS;
                this.ifHelperInterpolate = true;
                this.hCurrentInterpolationTimeMS = (int)tmpMS.TotalMilliseconds;
                this.hCurrentInterpolation = 0.0f;
            }
            else
            {
                this.NewState = newS;
                ifInterpolate = true;
                currentInterpolation = 0.0f;
                this.currentInterpolationTimeMS = (int)tmpMS.TotalMilliseconds;
            }

            newS.Animation.StopClip();
            newS.Animation.StartClip();
        }

        public void ChangeState(string stateName, float startInterp)
        {
            AnimatorState newS = null;
            TimeSpan tmpMS = TimeSpan.Zero;

            KeyValuePair<TimeSpan, AnimatorState> tmpPair = this.CurrentState.GetTransitionAndTimeByName(stateName);
            newS = tmpPair.Value;
            tmpMS = tmpPair.Key;

            if (newS == null)
            {
                Debug.Log("ANIMATOR ERROR: ChangeState: Transition not found in current state's dictionary");
                return;
            }

            this.NewState = newS;
            ifInterpolate = true;
            currentInterpolation = startInterp;
            newS.Animation.StopClip();
            newS.Animation.StartClip();
        }

        public void ChangeState(string oldState, string newState, float startInterp)
        {
            AnimatorState oldS = this.AvailableStates[oldState];
            AnimatorState newS = this.AvailableStates[newState];
            this.NewState = newS;
            this.CurrentState = oldS;
            ifInterpolate = true;
            currentInterpolation = startInterp;
            if (this.CurrentState.Animation.MyState == AnimationPlayer.animationStates.STOPPED)
            {
                this.CurrentState.Animation.StopClip();
                this.CurrentState.Animation.StartClip();
            }
                
            if (this.NewState.Animation.MyState == AnimationPlayer.animationStates.STOPPED)
            {
                this.NewState.Animation.StopClip();
                this.NewState.Animation.StartClip();
            }
                
        }

        public void SetBlendState(string stateName)
        {
            AnimatorState newS = null;
            TimeSpan tmpMS = TimeSpan.Zero;

            KeyValuePair<TimeSpan, AnimatorState> tmpPair = this.CurrentState.GetTransitionAndTimeByName(stateName);
            newS = tmpPair.Value;
            tmpMS = tmpPair.Key;

            if (newS == null)
            {
                Debug.Log("ANIMATOR ERROR: SetBlendState: Transition not found in current state's dictionary");
                return;
            }

            //if (this.Locked)
            //    return;

            this.NewState = newS;
            this.ifInterpolate = false;
            currentInterpolation = 0.0f;
            newS.Animation.StopClip();
            newS.Animation.StartClip();
        }

        public void RemoveBlendStateToCurrent()
        {
            if(this.CurrentState != null && this.NewState != null)
            {
                ChangeState(this.CurrentState.Name, this.NewState.Name, this.currentInterpolation);
                this.interDirection = -1.0f;
                int newTime = (int) this.NewState.GetTimeByTransition(this.CurrentState).TotalMilliseconds;
                this.currentInterpolationTimeMS = newTime;
            }
            else
            {
                if(CurrentState == null)
                    Debug.Log("ANIMATOR ERROR: CurrentState is NULL");
                if (NewState == null)
                    Debug.Log("ANIMATOR ERROR: NewState is NULL");
            }
        }

        public void RemoveBlendStateToNew()
        {
            if (this.CurrentState != null && this.NewState != null)
            {
                this.ChangeState(this.NewState.Name);
            }
            else
            {
                Debug.Log("ANIMATOR ERROR: Either currentState or blendState is NULL");
            }
        }

        protected void CalculateInterpolationAmount(GameTime gameTime, int time, float direction, ref float interp)
        {
            float nextAmount = ((float)gameTime.ElapsedGameTime.TotalMilliseconds) / MathHelper.Max((float)time, 1.0f);
            interp += direction * nextAmount;
        }

        protected Matrix[] GetTransformsInterpolated(Matrix[] one, Matrix[] two, float interp)
        {
            if (one == null && two != null) return two;
            else if (two == null && one != null) return one;
            else if (one == null && two == null)
            {
                Matrix[] oldMatrices = new Matrix[SkinningData.BindPose.Count];
                for (int i = 0; i < SkinningData.BindPose.Count; ++i)
                {
                    oldMatrices[i] = Matrix.Identity;
                }
                return oldMatrices;
            }
            else
            {
                Matrix[] newMatrices = new Matrix[SkinningData.BindPose.Count];

                for (int j = 0; j < SkinningData.BindPose.Count; ++j)
                {
                    newMatrices[j] = Matrix.Lerp(one[j], two[j], MathHelper.Clamp(interp, 0.0f, 1.0f));
                }

                return newMatrices;
            }
        }

        protected Matrix[] GetTransformsInterpolated(AnimationPlayer one, AnimationPlayer two, float interp)
        {
            if (one == null && two != null) return two.GetSkinTransforms();
            else if (two == null && one != null) return one.GetSkinTransforms();
            else if (one == null && two == null)
            {
                Matrix[] oldMatrices = new Matrix[SkinningData.BindPose.Count];
                for (int i = 0; i < SkinningData.BindPose.Count; ++i)
                {
                    oldMatrices[i] = Matrix.Identity;
                }
                return oldMatrices;
            }
            else
            {
                Matrix[] oneM = one.GetSkinTransforms();
                Matrix[] twoM = two.GetSkinTransforms();
                Matrix[] newMatrices = new Matrix[SkinningData.BindPose.Count];

                for (int j = 0; j < SkinningData.BindPose.Count; ++j)
                {
                    newMatrices[j] = Matrix.Lerp(oneM[j], twoM[j], MathHelper.Clamp(interp, 0.0f, 1.0f));
                }

                return newMatrices;
            }
        }

        public override System.Xml.Schema.XmlSchema GetSchema()
        {
            return base.GetSchema();
        }

        public override void ReadXml(System.Xml.XmlReader reader)
        {
            base.ReadXml(reader);
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            base.WriteXml(writer);
        }

        #endregion
    }
}
