using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrashSoup.Engine
{
    public class CollisionEventArgs : EventArgs
    {
        public GameObject CollisionObj { get; set; }
        public CollisionEventArgs(GameObject collider)
        {
            CollisionObj = collider;
        }
    }
}
