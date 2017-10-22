using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay.Weapons
{
    public class Hammer : Weapon
    {
        public Hammer(GameObject obj) : base(obj)
        {
            Durability = 35;
            Damage = 18;
            OffsetPosition = new Vector3(0.1f, 0, 0.05f);
            OffsetRotation = new Vector3(0, 0, -30);
            Type = WeaponType.HEAVY;
            IsCraftable = true;
            CraftingCost = 50;
            Name = "Hammer";
            ParticleTexturePaths = new string[] 
            {
                "Textures/Particles/Particle_metal01",
                "Textures/Particles/Particle_metal02"
            };
            DestroyCueName = "metalHit";
        }

        public Hammer(GameObject obj, Hammer h) : base(obj, h)
        {
            Durability = h.Durability;
            Damage = h.Damage;
            OffsetPosition = h.OffsetPosition;
            OffsetRotation = h.OffsetRotation;
            Type = WeaponType.HEAVY;
            IsCraftable = true;
            CraftingCost = 50;
            Name = "Hammer";
            ParticleTexturePaths = h.ParticleTexturePaths;
            DestroyCueName = "metalHit";
        }
    }
}
