using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml.Serialization;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System.Xml;

namespace TrashSoup.Engine
{
    public class CustomModel : ObjectComponent, IXmlSerializable
    {
        #region staticVariables

        public enum LODStateEnum
        {
            HI,
            MED,
            LO
        }

        public const uint LOD_COUNT = 3;

        #endregion

        #region variables

        private string contentPath;

        #endregion

        #region properties

        public LODStateEnum LODState { get; set; }
        public Model[] LODs { get; set; }
        //public List<String> Paths { get; set; }
        public String[] Paths { get; set; }
        // material doesn't change with LOD, but with MeshPart !!!
        public List<Material> Mat { get; set; }
        public bool LodControlled { get; set; }

        #endregion

        #region methods
        public CustomModel() 
        {
            this.LODs = new Model[LOD_COUNT];
            this.Paths = new String[LOD_COUNT];
            //this.Paths = new List<string>();
            this.Mat = new List<Material>();
            this.LodControlled = true;
        }

        public CustomModel(GameObject obj) : base(obj)
        {
            this.LODState = LODStateEnum.HI;
            this.LODs = new Model[LOD_COUNT];
            this.Paths = new String[LOD_COUNT];
            //this.Paths = new List<String>();
            for (int i = 0; i < LOD_COUNT; i++)
            {
                this.LODs[i] = null;
                this.Paths[i] = "";
            }
            this.Mat = new List<Material>();
            this.LodControlled = true;
        }

        public CustomModel(GameObject obj, Model[] lods, List<Material> matList)
            : this(obj)
        {
            for(int i = 0; i < LOD_COUNT; i++)
            {
                if(lods[i] != null)
                {
                    this.LODs[i] = lods[i];
                    this.Paths[i] = ResourceManager.Instance.Models.FirstOrDefault(x => x.Value == lods[i]).Key;
                    //this.Paths.Add(ResourceManager.Instance.Models.FirstOrDefault(x => x.Value == lods[i]).Key);
                }
                else
                {
                    this.LODs[i] = lods[0];
                    this.Paths[i] = "";
                }
            }
            Mat = matList;

            if (this.Mat.Count != this.LODs[0].Meshes.Count)
            {
                Debug.Log("CustomModel ERROR: Number of materials is not the same as the number of MeshParts!!!");
            }
        }

        public CustomModel(GameObject obj, CustomModel cm) : base(obj)
        {
            this.LODs = new Model[LOD_COUNT];
            this.Paths = new String[LOD_COUNT];
            //this.Paths = new List<String>();
            this.Mat = new List<Material>();
            for(int i = 0; i < LOD_COUNT; ++i)
            {
                this.LODs[i] = cm.LODs[i];
                this.Paths[i] = cm.Paths[i];
            }
            //foreach(string path in cm.Paths)
            //{
            //    this.Paths.Add(path);
            //}
            foreach(Material m in cm.Mat)
            {
                this.Mat.Add(m);
            }
            this.LODState = LODStateEnum.HI;
        }

        public override void Update(GameTime gameTime)
        {
            if(this.Enabled)
            {
                // do nothing currently
            }
        }

        public override void Draw(Camera cam, Effect effect, GameTime gameTime)
        {
            if(this.Visible)
            {
                Camera camera;
                    if (cam == null)
                        camera = ResourceManager.Instance.CurrentScene.Cam;
                    else
                        camera = cam;

                GraphicsDevice device = TrashSoupGame.Instance.GraphicsDevice;

                float distance = Math.Abs(Vector3.Distance((camera.Position + camera.Translation), MyObject.MyTransform.GetWorldMatrix().Translation));
                if(!ResourceManager.Instance.CurrentScene.Params.UseLods || !this.LodControlled || this.LODs[1] == null)
                {
                    LODState = LODStateEnum.HI;
                }
                else if((distance >= ResourceManager.Instance.CurrentScene.Params.Lod1Distance && 
                    distance < ResourceManager.Instance.CurrentScene.Params.Lod2Distance) || this.LODs[2] == null)
                {
                    LODState = LODStateEnum.MED;
                }
                else if((distance >= ResourceManager.Instance.CurrentScene.Params.Lod2Distance))
                {
                    LODState = LODStateEnum.LO;
                }
                else
                {
                    LODState = LODStateEnum.HI;
                }

                Model mod = LODs[(uint)LODState];
                if(mod != null)
                {
                    

                    Transform transform = MyObject.MyTransform;
                    Matrix[] bones = null;
                    Matrix socketMatrix = Matrix.Identity;

                    if (MyObject.MyAnimator != null)
                    {
                        bones = MyObject.MyAnimator.GetSkinTransforms();
                    }

                    int ctr = 0;
                    foreach (ModelMesh mm in mod.Meshes)
                    {
                        for (int i = 0; i < mm.MeshParts.Count; ++i)
                        {
                            if(this.Mat.Count == 0 || this.Mat.Count < ctr - 1)
                            {
                                return;
                            }
                            this.Mat[ctr].UpdateEffect(
                                 effect,
                                 mm.ParentBone.Transform * transform.GetWorldMatrix(), 
                                 (mm.ParentBone.Transform * transform.GetWorldMatrix()) * camera.ViewProjMatrix,
                                 ResourceManager.Instance.CurrentScene.AmbientLight,
                                 ResourceManager.Instance.CurrentScene.DirectionalLights,
                                 MyObject.LightsAffecting,
                                 ResourceManager.Instance.CurrentScene.GetDirectionalShadowMap(),
                                 ResourceManager.Instance.CurrentScene.GetPointLight0ShadowMap(),
                                 camera.Position + camera.Translation,
                                 camera.Bounds,
                                 bones,
                                 gameTime);
                            mm.MeshParts[i].Effect = this.Mat[ctr].MyEffect;
                            this.Mat[ctr].FlushMaterialEffect();
                            ++ctr;
                            if(ctr >= this.Mat.Count)
                            {
                                break;
                            }
                        }

                        mm.Draw();

                        if (ctr >= this.Mat.Count)
                        {
                            break;
                        }
                    }
                }
            }
        }

        protected override void Start()
        {
        }

        public void GetBoneMatrix(string name, out Matrix mat)
        {
            SkinningModelLibrary.SkinningData data = (SkinningModelLibrary.SkinningData)((object[])this.LODs[0].Tag)[0];
            if(data == null)
            {
                mat = Matrix.Identity;
                Debug.Log("CUSTOMMODEL: Error, trying to get bone matrix for non-skinned model.");
                return;
            }

            Dictionary<string, int> dic = data.BoneNameToID;
            int id;
            bool result = dic.TryGetValue(name, out id);
            if(!result)
            {
                mat = Matrix.Identity;
                Debug.Log("CUSTOMMODEL: Given bone name not present in bone dictionary.");
                return;
            }

            if(MyObject.MyAnimator == null)
            {
                mat = data.BindPose[id];
            }
            else
            {
                mat = MyObject.MyAnimator.GetWorldTransforms()[id];
            }
        }

        //protected virtual void FlipZAxis()
        //{
        //    for(int i = 0; i < LOD_COUNT; i++)
        //    {
        //        if(LODs[i] != null)
        //        {
        //            foreach (ModelMesh mm in LODs[i].Meshes)
        //            {
        //                Matrix trans = mm.ParentBone.Transform;
        //                trans.Forward = new Vector3(
        //                    trans.Forward.X,
        //                    trans.Forward.Y,
        //                    -trans.Forward.Z
        //                    );
        //                mm.ParentBone.Transform = trans;
        //            }
        //        }
        //    }
        //}

        public override System.Xml.Schema.XmlSchema GetSchema()
        {
            return null;
        }

        public override void ReadXml(System.Xml.XmlReader reader)
        {
            reader.MoveToContent();
            reader.ReadStartElement();

            base.ReadXml(reader);

            LODState = (LODStateEnum)Enum.Parse(typeof(LODStateEnum), reader.ReadElementString("LODState", ""));

            if(reader.Name == "LODs")
            {
                reader.ReadStartElement();
                int counter = 0;
                while (reader.NodeType != System.Xml.XmlNodeType.EndElement)
                {
                    String s = reader.ReadElementString("ModelPath", "");
                    Paths[counter] = s;
                    counter++;
                }
                reader.ReadEndElement();
            }

            for(int j = 0; j<Paths.Count(); ++j)
            {
                if(Paths[j] != "")
                {
                    LODs[j] = ResourceManager.Instance.LoadModel(Paths[j]);
                }
                else
                {
                    LODs[j] = LODs[0];
                }
            }

            if(reader.Name == "Materials")
            {
                if (!TrashSoupGame.Instance.EditorMode)
                {
                    contentPath = "../../../../TrashSoupContent/Materials/";
                }
                else
                {
                    contentPath = "../../../TrashSoup/TrashSoupContent/Materials/";
                }
                reader.ReadStartElement();
                while(reader.NodeType != System.Xml.XmlNodeType.EndElement)
                {
                    if (reader.Name == "Name")
                    {

                        String newName = reader.ReadElementString("Name", "");

                        if(newName == "testSBMat2")
                        {
                            SkyboxMaterial sm = new SkyboxMaterial();
                            sm = ResourceManager.Instance.LoadSkyboxMaterial(Path.GetFullPath(contentPath) + newName + ".xml");
                            Mat.Add(sm);
                        }
                        if (newName == "testWaterMat")
                        {
                            WaterMaterial wm = new WaterMaterial();
                            wm = ResourceManager.Instance.LoadWaterMaterial(Path.GetFullPath(contentPath) + newName + ".xml");
                            Mat.Add(wm);
                        }
                        if (newName == "testMirrorMat")
                        {
                            MirrorMaterial mm = new MirrorMaterial();
                            mm = ResourceManager.Instance.LoadMirrorMaterial(Path.GetFullPath(contentPath) + newName + ".xml");
                            Mat.Add(mm);
                        }
                        if(newName != "testSBMat2" && newName != "testWaterMat" && newName != "testMirrorMat")
                        {
                            Material m = new Material();
                            m = ResourceManager.Instance.LoadMaterial(Path.GetFullPath(contentPath) + newName + ".xml");
                            Mat.Add(m);
                        }
                    }
                }
               
                reader.ReadEndElement();
            }

            this.LodControlled = reader.ReadElementContentAsBoolean("LodControlled", "");
            //ResourceManager.Instance.LoadEffects(TrashSoupGame.Instance);
            reader.ReadEndElement();
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            if (!TrashSoupGame.Instance.EditorMode)
            {
                contentPath = "../../../../TrashSoupContent/Materials/";
            }
            else
            {
                contentPath = "../../../TrashSoup/TrashSoupContent/Materials/";
            }
            base.WriteXml(writer);

            writer.WriteElementString("LODState", LODState.ToString());

            writer.WriteStartElement("LODs");
            foreach(String path in Paths)
            {
                if(path != null)
                {
                    writer.WriteElementString("ModelPath", path);
                }
                else
                {
                    writer.WriteElementString("ModelPath", "");
                }
            }
            writer.WriteEndElement();

            writer.WriteStartElement("Materials");
            foreach(Material mat in Mat)
            {
                if(mat != null)
                {
                    if(mat.Name == "testSBMat2")
                    {
                        XmlSerializer serializer = new XmlSerializer(typeof(SkyboxMaterial));
                        using (FileStream file = new FileStream(Path.GetFullPath(contentPath) + mat.Name + ".xml", FileMode.Create))
                        {
                            serializer.Serialize(file, mat);
                        }
                    }
                    if (mat.Name == "testWaterMat")
                    {
                        XmlSerializer serializer = new XmlSerializer(typeof(WaterMaterial));
                        using (FileStream file = new FileStream(Path.GetFullPath(contentPath) + mat.Name + ".xml", FileMode.Create))
                        {
                            serializer.Serialize(file, mat);
                        }
                    }
                    if (mat.Name == "testMirrorMat")
                    {
                        XmlSerializer serializer = new XmlSerializer(typeof(MirrorMaterial));
                        using (FileStream file = new FileStream(Path.GetFullPath(contentPath) + mat.Name + ".xml", FileMode.Create))
                        {
                            serializer.Serialize(file, mat);
                        }
                    }
                    if (mat.Name != "testSBMat2" && mat.Name != "testWaterMat" && mat.Name != "testMirrorMat")
                    {
                        XmlSerializer serializer = new XmlSerializer(typeof(Material));
                        using (FileStream file = new FileStream(Path.GetFullPath(contentPath) + mat.Name + ".xml", FileMode.Create))
                        {
                            serializer.Serialize(file, mat);
                        }
                    }

                    //writer.WriteStartElement("Material");

                    writer.WriteElementString("Name", mat.Name);

                    //writer.WriteElementString("EffectPath", ResourceManager.Instance.Effects.FirstOrDefault(x => x.Value == mat.MyEffect).Key);

                    //(mat as IXmlSerializable).WriteXml(writer);
                    //writer.WriteEndElement();
                }
            }
            writer.WriteEndElement();

            writer.WriteElementString("LodControlled", XmlConvert.ToString(LodControlled));
        }

        private void EffectParametersSerialization(System.Xml.XmlWriter writer, EffectParameter param)
        {
            switch(param.ParameterType)
            {
                case EffectParameterType.Bool:
                    writer.WriteElementString("Value", XmlConvert.ToString(param.GetValueBoolean()));
                    break;
                case EffectParameterType.Int32:
                    writer.WriteElementString("Value", XmlConvert.ToString(param.GetValueInt32()));
                    break;
                case EffectParameterType.Texture2D:
                    writer.WriteElementString("Value", ResourceManager.Instance.Textures.FirstOrDefault(x => x.Value == param.GetValueTexture2D()).Key);
                    break;
                case EffectParameterType.TextureCube:
                    writer.WriteElementString("Value", ResourceManager.Instance.TexturesCube.FirstOrDefault(x => x.Value == param.GetValueTextureCube()).Key);
                    break;
                case EffectParameterType.String:
                    writer.WriteElementString("Value", param.GetValueString());
                    break;
                case EffectParameterType.Single:
                    CheckParameterClass(writer, param);
                    break;
                default:
                    writer.WriteElementString("Value", param.ParameterType.ToString());
                    break;
            }
        }

        private Effect EffectParameterDeserialization(System.Xml.XmlReader reader)
        {
            Effect newEf = null;
            String s;
            List<float> array1 = new List<float>();
            List<Vector3> array2 = new List<Vector3>();
            reader.ReadStartElement();
            while(reader.NodeType != System.Xml.XmlNodeType.EndElement)
            {
                if(reader.Name == "Name")
                {
                    s = reader.ReadElementString("Name", "");
                    switch(s)
                    {
                        case "World":
                            newEf.Parameters["World"].SetValue(this.MyObject.MyTransform.GetWorldMatrix());
                            s = reader.ReadElementString("Value", "");
                            break;
                        case "WorldViewProj":
                            break;
                        case "WorldInverseTranspose":
                            break;
                        default:
                            if(reader.Name == "Values")
                            {
                                reader.ReadStartElement();
                                while(reader.NodeType != System.Xml.XmlNodeType.EndElement)
                                {

                                }
                                reader.ReadEndElement();
                            }
                            else
                            {
                                newEf.Parameters[s].SetValue(reader.ReadElementString("Value", ""));
                            }
                            break;
                    }
                }                       
            }
            reader.ReadEndElement();
            return newEf;
        }

        private void CheckParameterClass(System.Xml.XmlWriter writer, EffectParameter param)
        {
            switch(param.ParameterClass)
            {
                case EffectParameterClass.Vector:
                    if(param.RowCount == 1)
                    {
                        if (param.Elements.Count > 0)
                        {
                            writer.WriteStartElement("VectorValues");
                            for (int i = 0; i < param.Elements.Count; ++i)
                            {
                                writer.WriteElementString("Value", XmlConvert.ToString(param.Elements[i].GetValueVector3().X));
                            }
                            writer.WriteEndElement();
                        }
                        else
                        {
                            writer.WriteElementString("Value", XmlConvert.ToString(param.GetValueVector3().X));
                        }
                    }
                    if(param.RowCount == 2)
                    {
                        writer.WriteStartElement("Vector2Values");
                        writer.WriteElementString("Value", XmlConvert.ToString(param.GetValueVector2().X));
                        writer.WriteElementString("Value", XmlConvert.ToString(param.GetValueVector2().Y));
                        writer.WriteEndElement();
                    }
                    if(param.RowCount == 3)
                    {
                        writer.WriteStartElement("Vector3Values");
                        writer.WriteElementString("Value", XmlConvert.ToString(param.GetValueVector3().X));
                        writer.WriteElementString("Value", XmlConvert.ToString(param.GetValueVector3().Y));
                        writer.WriteElementString("Value", XmlConvert.ToString(param.GetValueVector3().Z));
                        writer.WriteEndElement();
                    }
                    if(param.RowCount == 4)
                    {
                        writer.WriteStartElement("Vector4Values");
                        writer.WriteElementString("Value", XmlConvert.ToString(param.GetValueVector4().X));
                        writer.WriteElementString("Value", XmlConvert.ToString(param.GetValueVector4().Y));
                        writer.WriteElementString("Value", XmlConvert.ToString(param.GetValueVector4().Z));
                        writer.WriteElementString("Value", XmlConvert.ToString(param.GetValueVector4().W));
                        writer.WriteEndElement();
                    }
                    break;
                case EffectParameterClass.Scalar:
                    if(param.Elements.Count > 0)
                    {
                        writer.WriteStartElement("ArrayValues");
                        for(int i = 0; i<param.Elements.Count; ++i)
                        {
                            writer.WriteElementString("Value", XmlConvert.ToString(param.Elements[i].GetValueSingle()));
                        }
                        writer.WriteEndElement();
                    }
                    else
                    {
                        writer.WriteElementString("Value", XmlConvert.ToString(param.GetValueSingle()));
                    }
                    break;
                case EffectParameterClass.Matrix:
                    writer.WriteElementString("Value", "Matrix");
                    break;
                default:
                    writer.WriteElementString("Value", param.ParameterClass.ToString());
                    break;
            }
        }
        #endregion

    }
}
