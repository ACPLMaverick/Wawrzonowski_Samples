using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay.Weapons
{
    public class Machete : Weapon
    {
        public Machete(GameObject obj) : base(obj)
        {
            Durability = 50;
            Damage = 25;
            OffsetPosition = new Vector3(0.04f, 0, 0.03f);
            OffsetRotation = new Vector3(0, -60, 0);
            Type = WeaponType.MEDIUM;
            IsCraftable = true;
            CraftingCost = 100;
            Name = "Machete";

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

        public Machete(GameObject obj, Machete m) : base(obj, m)
        {
            Durability = m.Durability;
            Damage = m.Damage;
            OffsetPosition = m.OffsetPosition;
            OffsetRotation = m.OffsetRotation;
            Type = WeaponType.MEDIUM;
            IsCraftable = true;
            CraftingCost = 100;
            Name = "Machete";

            ParticleTexturePaths = m.ParticleTexturePaths;
            DestroyCueName = "metalHit";
        }
    }
}
