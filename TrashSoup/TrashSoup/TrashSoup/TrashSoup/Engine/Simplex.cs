using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrashSoup.Engine
{
    class Simplex
    {
        private Vector3 direction;
        private Vector3[] points;
        private int currentPointToAdd = 0;
        private int pointsAdded = 0;
        private Vector3 lastPoint;

        public Vector3 Direction
        {
            get { return direction; }
        }

        public Simplex(int maxPoints)
        {
            points = new Vector3[maxPoints];
        }

        public void Add(Vector3 point)
        {
            ++pointsAdded;
            points[currentPointToAdd] = point;
            currentPointToAdd += 1;
            currentPointToAdd %= points.Length;
            if(pointsAdded > 1)
            {
                Vector3 AB = point - lastPoint;
                Vector3 AO = -point;
                direction = Vector3.Cross(Vector3.Cross(AB, AO), AB);
            }
            lastPoint = point;
        }

        public bool ContainsOrigin()
        {
            if(pointsAdded >= 4)
            {
                Vector3 origin = Vector3.Zero;
                for(int i = 0; i < 4; ++i)
                {
                    Vector3 A = points[i];
                    Vector3 B = points[(i + 1) % 4];
                    Vector3 C = points[(i + 2) % 4];
                    Vector3 ABCCenter = (A + B + C) / 3;
                    Vector3 CenterToO = -ABCCenter;
                    Vector3 CenterToD = points[(i + 3) % 4] - ABCCenter;
                    if(Vector3.Dot(CenterToD, CenterToO) < 0.0f)
                    {
                        return false;
                    }

                }
                return true;
            }

            return false;
        }
    }
}
