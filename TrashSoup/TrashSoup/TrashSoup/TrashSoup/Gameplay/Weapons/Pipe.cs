using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay.Weapons
{
    public class Pipe : Weapon
    {
        public Pipe(GameObject obj) : base(obj)
        {
            Durability = 30;
            Damage = 20;
            OffsetPosition = new Vector3(0.1f, 0, 0);
            OffsetRotation = new Vector3(0, -10, 0);
            Type = WeaponType.MEDIUM;
            IsCraftable = true;
            CraftingCost = 40;
            Name = "Pipe";

            ParticleTexturePaths = new string[] 
            {
                "Textures/Particles/Particle_metal01",
                "Textures/Particles/Particle_metal02",
            };
            DestroyCueName = "metalHit";
        }

        public Pipe(GameObject obj, Pipe p) : base(obj, p)
        {
            Durability = p.Durability;
            Damage = p.Damage;
            OffsetPosition = p.OffsetPosition;
            OffsetRotation = p.OffsetRotation;
            Type = WeaponType.MEDIUM;
            IsCraftable = true;
            CraftingCost = 40;
            Name = "Pipe";

            ParticleTexturePaths = p.ParticleTexturePaths;
            DestroyCueName = "metalHit";
        }
    }
}
