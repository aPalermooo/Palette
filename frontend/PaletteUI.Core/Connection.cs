using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PaletteUI.Core
{
    /// <summary>
    /// Handles all logic for connecting to backend process
    /// </summary>
    /// Singleton class
    public sealed class Connection
    {
        private string url = "localhost:18080/";


        private static readonly Connection instance = new Connection();

        static Connection() { }

        private Connection() { }

        public static Connection Instance => instance;

        /// <summary>
        /// checks if connnection is live
        /// </summary>
        public bool isLive => false;

    }
}
