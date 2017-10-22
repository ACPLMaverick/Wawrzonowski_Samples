using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;
using TrashSoup.Engine.AI.BehaviorTree;

namespace TrashSoup.Gameplay.MutantAI
{
    class MutantStay : TrashSoup.Engine.AI.BehaviorTree.Action
    {
        private float timer = 0.0f;
        private float idleTime = 3.0f;
        private bool firstTime = true;

        public override Engine.AI.BehaviorTree.TickStatus Tick(Microsoft.Xna.Framework.GameTime gameTime, out Engine.AI.BehaviorTree.INode node)
        {
            if (this.blackboard.GetBool("Dead"))
            {
                node = null;
                firstTime = true;
                return TickStatus.FAILURE;
            }

            if (this.blackboard.GetBool("TargetSeen"))
            {
                firstTime = true;
                node = null;
                this.blackboard.SetBool("Idle", false);
                return TickStatus.FAILURE;
            }
            if (timer > idleTime)
            {
                firstTime = true;
                this.blackboard.SetBool("Idle", false);
                timer = 0.0f;
                node = null;
                return TickStatus.SUCCESS;
            }

            if (firstTime)
            {
                firstTime = false;
                this.blackboard.Owner.MyAnimator.ChangeState("Idle");
            }

            node = this;
            timer += gameTime.ElapsedGameTime.Milliseconds * 0.001f;
            return TickStatus.RUNNING;
        }
    }
}
