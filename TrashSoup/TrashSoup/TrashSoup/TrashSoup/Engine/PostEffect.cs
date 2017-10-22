using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace TrashSoup.Engine
{
    public abstract class PostEffect
    {
        #region properties

        public Effect MyEffect { get; protected set; }
        public string Name { get; set; }

        #endregion

        #region methods

        public PostEffect(string name)
        {
            Name = name;
            LoadAssociatedEffect();
        }

        public PostEffect(string name, Effect mEffect)
        {
            Name = name;
            MyEffect = mEffect;
        }

        public abstract void UpdateEffect();
        protected abstract void LoadAssociatedEffect();

        #endregion
    }
}
