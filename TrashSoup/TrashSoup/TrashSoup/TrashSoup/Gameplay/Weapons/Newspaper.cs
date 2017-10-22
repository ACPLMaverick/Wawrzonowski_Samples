using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay.Weapons
{
    public class Newspaper : Weapon
    {
        public Newspaper(GameObject obj) : base(obj)
        {
            Durability = 20;
            Damage = 7;
            OffsetPosition = new Vector3(0.04f, 0, 0.03f);
            OffsetRotation = new Vector3(-1.565f, 0, 0);
            Type = WeaponType.LIGHT;
            IsCraftable = false;
            Name = "Newspaper";

            ParticleTexturePaths = new string[] 
            {
                "Textures/Particles/Particle_wood01"
            };
            DestroyCueName = "stoneHit";
        }

        public Newspaper(GameObject obj, Newspaper n) : base(obj, n)
        {
            Durability = n.Durability;
            Damage = n.Damage;
            OffsetPosition = n.OffsetPosition;
            OffsetRotation = n.OffsetRotation;
            Type = WeaponType.LIGHT;
            IsCraftable = false;
            Name = "Newspaper";

            ParticleTexturePaths = n.ParticleTexturePaths;
            DestroyCueName = "stoneHit";
        }
    }
}
