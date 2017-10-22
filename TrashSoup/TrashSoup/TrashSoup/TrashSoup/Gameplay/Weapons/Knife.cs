using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay.Weapons
{
    public class Knife : Weapon
    {
        public Knife(GameObject obj):base(obj)
        {
            Durability = 35;
            Damage = 13;
            OffsetPosition = new Vector3(0.1f, 0, 0.025f);
            OffsetRotation = new Vector3(1.0f, -1.0f, 0);
            Type = WeaponType.LIGHT;
            IsCraftable = true;
            CraftingCost = 50;
            Name = "Knife";

            ParticleTexturePaths = new string[] 
            {
                "Textures/Particles/Particle_metal01",
                "Textures/Particles/Particle_metal02"
            };
            DestroyCueName = "metalHit";
        }

        public Knife(GameObject obj, Knife k) : base(obj, k)
        {
            Durability = k.Durability;
            Damage = k.Damage;
            OffsetPosition = k.OffsetPosition;
            OffsetRotation = k.OffsetRotation;
            Type = WeaponType.LIGHT;
            IsCraftable = true;
            CraftingCost = 50;
            Name = "Knife";

            ParticleTexturePaths = k.ParticleTexturePaths;
            DestroyCueName = "metalHit";
        }
    }
}
