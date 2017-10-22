using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Content.Pipeline;
using Microsoft.Xna.Framework.Content.Pipeline.Graphics;
using Microsoft.Xna.Framework.Content.Pipeline.Processors;
using SkinningModelLibrary;

namespace SkinnedModelPipeline
{
    [ContentProcessor(DisplayName = "Custom Model Processor - TrashSoup")]
    public class CustomModelProcessor : ModelProcessor
    {
        /// <summary>
        /// Embeds animation data into ModelContent
        /// </summary>
        /// <param name="input"></param>
        /// <param name="context"></param>
        /// <returns></returns>
        public override ModelContent Process(NodeContent input, ContentProcessorContext context)
        {
            // Chain to teh base ModelProcessor class so it can convert the model data
            ModelContent model = base.Process(input, context);


            // Read material names from Material property
            List<SkinnedModelLibrary.MaterialModel> mats = new List<SkinnedModelLibrary.MaterialModel>();
            List<string> tmpNames = new List<string>();
            foreach (ModelMeshContent mmc in model.Meshes)
            {
                foreach (ModelMeshPartContent mmpc in mmc.MeshParts)
                {
                    if (!tmpNames.Contains(mmpc.Material.Name))
                    {
                        SkinnedModelLibrary.MaterialModel m = new SkinnedModelLibrary.MaterialModel();
                        m.MaterialName = mmpc.Material.Name;
                        for (int i = 0; i < SkinnedModelLibrary.MaterialModel.TEXTURE_COUNT && i < mmpc.Material.Textures.Count; ++i)
                            m.MaterialTextureNames[i] = mmpc.Material.Textures.ElementAt(i).Value.Filename;
                        mats.Add(m);
                    }
                    tmpNames.Add(mmpc.Material.Name);
                }
            }

            // Store our custon animation data in the Tag property of the model
            object[] intoTag = new object[3];
            intoTag[0] = null;
            intoTag[1] = mats;
            intoTag[2] = tmpNames;
            model.Tag = intoTag;

            return model;
        }

        public override bool PremultiplyTextureAlpha
        {
            get
            {
                return false;
            }
            set
            {
                
            }
        }

        public override bool GenerateMipmaps
        {
            get
            {
                return true;
            }
            set
            {
                
            }
        }
    }
}
