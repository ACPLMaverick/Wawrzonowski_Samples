using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;

namespace TrashSoup.Engine
{
    public class Singleton <T> where T : new()
    {
        #region Variables

        private static T m_instance;
        private static object locker = new object();

        #endregion

        #region Properties

        public static T Instance
        {
            get
            {
                if(m_instance == null)
                {
                    lock (locker)
                    {
                        if(m_instance == null)
                        {
                            m_instance = new T();
                        }
                    }
                }

                return m_instance;
            }
        }

        #endregion
    }
}
