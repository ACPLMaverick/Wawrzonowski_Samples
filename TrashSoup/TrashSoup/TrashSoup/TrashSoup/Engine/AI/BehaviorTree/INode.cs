using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrashSoup.Engine.AI.BehaviorTree
{
    public enum TickStatus { SUCCESS, FAILURE, RUNNING };

    public interface INode
    {
        void Initialize();

        TickStatus Tick(GameTime gameTime, out INode running);

        void SetBlackboard(Blackboard bb);

        void AddChild(INode node);
    }
}
