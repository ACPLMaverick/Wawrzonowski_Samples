using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;

namespace TrashSoup.Engine
{
    public class Debug
    {
        public static void Log(string message)
        {
#if DEBUG
            Console.WriteLine(message);
#endif
        }
    }
}
