using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay.Weapons
{
    public class Signs : Weapon
    {
        public Signs(GameObject obj) : base(obj)
        {
            Durability = 20;
            Damage = 10;
            Type = WeaponType.LIGHT;
            IsCraftable = true;
            CraftingCost = 40;
            Name = "Razor Sign";

            ParticleTexturePaths = new string[] 
            {
                "Textures/Particles/Particle_metal01",
                "Textures/Particles/Particle_metal02",
                "Textures/Particles/Particle_wood01",
                "Textures/Particles/Particle_wood02",
                "Textures/Particles/Particle_wood03"
            };
            DestroyCueName = "metalHit";
        }
    }
}
