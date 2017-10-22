using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay.Weapons
{
    public class Stones : Weapon
    {
        public Stones(GameObject obj):base(obj)
        {
            Durability = 5;
            Damage = 10;
            OffsetPosition = new Vector3(0.04f, 0, 0.03f);
            OffsetRotation = new Vector3(0, -60, 0);
            Type = WeaponType.LIGHT;
            IsCraftable = true;
            CraftingCost = 15;
            Name = "Stone";

            ParticleTexturePaths = new string[] 
            {
                "Textures/Particles/Particle_stone01"
            };
            DestroyCueName = "stoneHit";
        }

        public Stones(GameObject obj, Stones s) : base(obj, s)
        {
            Durability = s.Durability;
            Damage = s.Damage;
            OffsetPosition = s.OffsetPosition;
            OffsetRotation = s.OffsetRotation;
            Type = WeaponType.LIGHT;
            IsCraftable = true;
            CraftingCost = 15;
            Name = "Stone";

            ParticleTexturePaths = s.ParticleTexturePaths;
            DestroyCueName = "stoneHit";
        }
    }
}
