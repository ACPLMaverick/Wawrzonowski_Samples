using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrashSoup.Gameplay.Weapons
{
    public class Brick : Stones
    {
        public Brick(TrashSoup.Engine.GameObject obj) : base(obj)
        {
            Name = "Brick";

            ParticleTexturePaths = new string[] 
            {
                "Textures/Particles/Particle_brick01"
            };
            DestroyCueName = "stoneHit";
        }
    }
}
