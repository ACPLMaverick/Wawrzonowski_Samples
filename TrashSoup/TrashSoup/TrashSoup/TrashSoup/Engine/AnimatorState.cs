using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SkinningModelLibrary;
using Microsoft.Xna.Framework;

namespace TrashSoup.Engine
{
    public class AnimatorState
    {
        #region enums

        public enum StateType
        {
            SINGLE,
            LOOPING
        }

        #endregion

        #region variables

        /// <summary>
        /// A transition is described by a time it takes and an another animation that we transite to.
        /// </summary>
        protected List<AnimatorState> transitions;
        protected List<TimeSpan> transitionTimes;

        #endregion

        #region properties

        public string Name { get; set; }
        public AnimationPlayer Animation { get; set; }
        public StateType Type { get; set; }
        public bool IsFinished { get; set; }

        #endregion

        #region methods

        public AnimatorState()
        {
            this.transitions = new List<AnimatorState>();
            this.transitionTimes = new List<TimeSpan>();
            this.AddTransition(this, TimeSpan.Zero);
            this.Type = StateType.LOOPING;
            this.IsFinished = false;
        }

        public AnimatorState(string name, AnimationPlayer animation) : this()
        {
            this.Name = name;
            this.Animation = animation;
            this.Animation.StartClip();
        }

        public AnimatorState(string name, AnimationPlayer animation, StateType type) : this(name, animation)
        {
            this.Type = type;
        }

        public void Update(GameTime gameTime)
        {
            if(this.Type == StateType.SINGLE)
            {
                if(Animation.CurrentTime.TotalMilliseconds >= Animation.GetDuration().TotalMilliseconds - gameTime.ElapsedGameTime.TotalMilliseconds * 1.1)
                {
                    Animation.StopClip();
                    this.IsFinished = true;
                }
            }
            Animation.Update(gameTime.ElapsedGameTime, true, Matrix.Identity);
        }

        public List<AnimatorState> GetTransitions()
        {
            return this.transitions;
        }

        public List<TimeSpan> GetTransitionTimes()
        {
            return this.transitionTimes;
        }

        public void AddTransition(AnimatorState state, TimeSpan span)
        {
            this.transitions.Add(state);
            this.transitionTimes.Add(span);
        }

        public TimeSpan GetTimeByTransition(AnimatorState state)
        {
            int i = transitions.IndexOf(state);
            if (i == -1) return default(TimeSpan);
            else return transitionTimes[i];
        }

        public KeyValuePair<TimeSpan, AnimatorState> GetTransitionAndTimeByName(string name)
        {
            for(int i = 0; i < transitions.Count; ++i)
            {
                if(transitions[i].Name.Equals(name))
                {
                    return new KeyValuePair<TimeSpan, AnimatorState>(transitionTimes[i], transitions[i]);
                }
            }

            return new KeyValuePair<TimeSpan,AnimatorState>(default(TimeSpan), null);
        }

        #endregion
    }
}
