using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay.Weapons
{
    public class Bottle : Weapon
    {
        public Bottle(GameObject obj):base(obj)
        {
            Durability = 1;
            Damage = 35;
            OffsetPosition = new Vector3(0.1f, 0, 0);
            OffsetRotation = new Vector3(0, 0, -60);
            Type = WeaponType.LIGHT;
            IsCraftable = false;
            Name = "Bottle";

            ParticleTexturePaths = new string[] 
            {
                "Textures/Particles/Particle_glass01"
            };
            DestroyCueName = "glassHit";
        }

        public Bottle(GameObject obj, Bottle b) : base(obj, b)
        {
            Durability = b.Durability;
            Damage = b.Damage;
            OffsetPosition = b.OffsetPosition;
            OffsetRotation = b.OffsetRotation;
            Type = WeaponType.LIGHT;
            IsCraftable = false;
            Name = "Bottle";

            ParticleTexturePaths = b.ParticleTexturePaths;
            DestroyCueName = "glassHit";
        }
    }
}
