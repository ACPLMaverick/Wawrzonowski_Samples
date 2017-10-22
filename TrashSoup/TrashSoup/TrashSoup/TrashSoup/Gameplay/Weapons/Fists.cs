using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay.Weapons
{
    public class Fists : Weapon
    {
        public Fists(GameObject obj):base(obj)
        {
            Durability = -1;
            Damage = 5;
            Type = WeaponType.FISTS;
            IsCraftable = false;
            Name = "Fists";
        }
    }
}
