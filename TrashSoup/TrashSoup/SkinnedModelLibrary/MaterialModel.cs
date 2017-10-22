using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SkinnedModelLibrary
{
    public class MaterialModel
    {
        #region constants

        public const int TEXTURE_COUNT = 3;

        #endregion

        #region properties

        public string MaterialName { get; set; }
        public string[] MaterialTextureNames { get; set; }

        #endregion

        #region methods

        public MaterialModel()
        {
            MaterialName = null;
            MaterialTextureNames = new string[TEXTURE_COUNT];
            for (int i = 0; i < TEXTURE_COUNT; ++i )
                MaterialTextureNames[i] = null;
        }

        public MaterialModel(string myName, string diffName, string normName, string cubeName)
        {
            MaterialName = myName;
            MaterialTextureNames = new string[TEXTURE_COUNT];
            MaterialTextureNames[0] = diffName;
            MaterialTextureNames[1] = normName;
            MaterialTextureNames[2] = cubeName;
        }

        #endregion
    }
}
