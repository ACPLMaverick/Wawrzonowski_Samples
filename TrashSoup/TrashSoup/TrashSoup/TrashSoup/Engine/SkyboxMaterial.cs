using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Serialization;

namespace TrashSoup.Engine
{
    public class SkyboxMaterial : Material, IXmlSerializable
    {
        #region effectParameters
        protected EffectParameter epCubeMap1;
        protected EffectParameter epCubeMap2;
        protected EffectParameter epCubeMap3;
        protected EffectParameter epProbes;
        #endregion

        #region properties
        public TextureCube CubeMap1 { get; set;}
        public TextureCube CubeMap2 { get; set; }
        public TextureCube CubeMap3 { get; set; }

        public Vector4 Probes { get; set; }
        #endregion

        #region methods

        public SkyboxMaterial() : base()
        {

        }

        public SkyboxMaterial(string name, Effect effect)
            : base(name, effect)
        {
            this.CubeMap1 = ResourceManager.Instance.LoadTextureCube("DefaultCube");
            this.CubeMap2 = ResourceManager.Instance.LoadTextureCube("DefaultCube");
            this.CubeMap3 = ResourceManager.Instance.LoadTextureCube("DefaultCube");
            this.Probes = new Vector4(1.0f, 0.0f, 0.0f, 0.0f);
        }

        public override void UpdateEffect(Effect effect, Microsoft.Xna.Framework.Matrix world, Microsoft.Xna.Framework.Matrix worldViewProj, 
            LightAmbient amb, LightDirectional[] dirs, List<LightPoint> points, Texture dSM, TextureCube point0SM, Microsoft.Xna.Framework.Vector3 eyeVector, 
            BoundingFrustumExtended frustum, Microsoft.Xna.Framework.Matrix[] bones, Microsoft.Xna.Framework.GameTime gameTime)
        {
            if(epCubeMap1 != null)
            {
                epCubeMap1.SetValue(CubeMap1);
            }
            if (epCubeMap2 != null)
            {
                epCubeMap2.SetValue(CubeMap2);
            }
            if (epCubeMap3 != null)
            {
                epCubeMap3.SetValue(CubeMap3);
            }
            if (epProbes != null)
            {
                epProbes.SetValue(Probes);
            }
            base.UpdateEffect(effect, world, worldViewProj, amb, dirs, points, dSM, point0SM, eyeVector, frustum, bones, gameTime);
        }

        public override void AssignParamsInitialize()
        {
            base.AssignParamsInitialize();

            int pNameHash;

            int cmp1 = ("CubeMap1").GetHashCode();
            int cmp2 = ("CubeMap2").GetHashCode();
            int cmp3 = ("CubeMap3").GetHashCode();
            int pr = ("Probes").GetHashCode();

            foreach (EffectParameter p in MyEffect.Parameters)
            {
                pNameHash = p.Name.GetHashCode();
                if (pNameHash == cmp1)
                {
                    epCubeMap1 = p;
                }
                else if (pNameHash == cmp2)
                {
                    epCubeMap2 = p;
                }
                else if (pNameHash == cmp3)
                {
                    epCubeMap3 = p;
                }
                else if (pNameHash == pr)
                {
                    epProbes = p;
                }
            }
        }

        public System.Xml.Schema.XmlSchema GetSchema()
        {
            return null;
        }

        public void ReadXml(System.Xml.XmlReader reader)
        {
            //reader.MoveToContent();
            //reader.ReadStartElement();

            base.ReadXml(reader);
            //Name = reader.ReadElementString("Name", "");
            //MyEffect = ResourceManager.Instance.LoadEffect(reader.ReadElementString("EffectPath", ""));

            if(reader.Name == "CubeMaps")
            {
                reader.ReadStartElement();
                CubeMap = ResourceManager.Instance.LoadTextureCube(reader.ReadElementString("CubeMap", ""));
                CubeMap1 = ResourceManager.Instance.LoadTextureCube(reader.ReadElementString("CubeMap1", ""));
                CubeMap2 = ResourceManager.Instance.LoadTextureCube(reader.ReadElementString("CubeMap2", ""));
                CubeMap3 = ResourceManager.Instance.LoadTextureCube(reader.ReadElementString("CubeMap3", ""));
                reader.ReadEndElement();
            }

            if(reader.Name == "Probes")
            {
                reader.ReadStartElement();
                Probes = new Vector4(reader.ReadElementContentAsFloat("X", ""),
                    reader.ReadElementContentAsFloat("Y", ""),
                    reader.ReadElementContentAsFloat("Z", ""),
                    reader.ReadElementContentAsFloat("W", ""));
                reader.ReadEndElement();
            }

            //reader.ReadStartElement("SpecularColor");
            //SpecularColor = new Vector3(reader.ReadElementContentAsFloat("X", ""),
            //                               reader.ReadElementContentAsFloat("Y", ""),
            //                               reader.ReadElementContentAsFloat("Z", ""));
            //reader.ReadEndElement();

            //Glossiness = reader.ReadElementContentAsFloat("Glossiness", "");

            AssignParamsInitialize();

            //reader.ReadEndElement();
        }

        public void WriteXml(System.Xml.XmlWriter writer)
        {
            base.WriteXml(writer);

            //writer.WriteElementString("Name", Name);
            //writer.WriteElementString("EffectPath", ResourceManager.Instance.Effects.FirstOrDefault(x => x.Value == MyEffect).Key);

            writer.WriteStartElement("CubeMaps");
            writer.WriteElementString("CubeMap", ResourceManager.Instance.TexturesCube.FirstOrDefault(x => x.Value == base.CubeMap).Key);
            writer.WriteElementString("CubeMap1", ResourceManager.Instance.TexturesCube.FirstOrDefault(x => x.Value == CubeMap1).Key);
            writer.WriteElementString("CubeMap2", ResourceManager.Instance.TexturesCube.FirstOrDefault(x => x.Value == CubeMap2).Key);
            writer.WriteElementString("CubeMap3", ResourceManager.Instance.TexturesCube.FirstOrDefault(x => x.Value == CubeMap3).Key);
            writer.WriteEndElement();

            writer.WriteStartElement("Probes");
            writer.WriteElementString("X", XmlConvert.ToString(Probes.X));
            writer.WriteElementString("Y", XmlConvert.ToString(Probes.Y));
            writer.WriteElementString("Z", XmlConvert.ToString(Probes.Z));
            writer.WriteElementString("W", XmlConvert.ToString(Probes.W));
            writer.WriteEndElement();

            //writer.WriteStartElement("SpecularColor");
            //writer.WriteElementString("X", XmlConvert.ToString(SpecularColor.X));
            //writer.WriteElementString("Y", XmlConvert.ToString(SpecularColor.Y));
            //writer.WriteElementString("Z", XmlConvert.ToString(SpecularColor.Z));
            //writer.WriteEndElement();

            //writer.WriteElementString("Glossiness", XmlConvert.ToString(Glossiness));
        }

        #endregion
    }
}
