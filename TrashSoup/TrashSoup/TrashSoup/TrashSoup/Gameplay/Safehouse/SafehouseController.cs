using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;
using TrashSoup.Gameplay;

namespace TrashSoup.Gameplay.Safehouse
{
    class SafehouseController : Singleton<SafehouseController>
    {
        private int hours;
        private int minutes;
        private int enemiesLeft;
        private bool canToMapSelection;
        private float trashStashed = 0;

        public int EnemiesLeft
        {
            get
            {
                return enemiesLeft;
            }
            set
            {
                enemiesLeft = (int)MathHelper.Clamp(value, 0, int.MaxValue);
                if(enemiesLeft == 0 && canToMapSelection)
                {
                    LoadMapSelectMenu();
                }
            }
        }

        public bool CanToMapSelection
        {
            get
            {
                return this.canToMapSelection;
            }
            set
            {
                this.canToMapSelection = value;
                if(this.canToMapSelection && enemiesLeft == 0)
                {
                    LoadMapSelectMenu();
                }
            }
        }

        public void SetExitTime(int h, int m)
        {
            this.hours = h;
            this.minutes = m;
        }

        public int[] GetExitTime()
        {
            int[] time = new int[2];
            time[0] = this.hours;
            time[1] = this.minutes;
            return time;
        }

        public float TrashStashed
        {
            get { return this.trashStashed; }
            set { this.trashStashed = value; }
        }

        public void LoadMapSelectMenu()
        {
            SaveManager.Instance.XmlPath = "../../../../TrashSoupContent/Scenes/mapSelection.xml";
            SaveManager.Instance.LoadFileAction();
        }
    }
}
