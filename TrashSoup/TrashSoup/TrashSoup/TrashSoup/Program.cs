using System;

namespace TrashSoup
{
#if WINDOWS || XBOX
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main(string[] args)
        {
            using (TrashSoupGame game = new TrashSoupGame())
            {
                game.Run();
            }
        }
    }
#endif
}

