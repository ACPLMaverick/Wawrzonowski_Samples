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
    public class LightPoint : GameObject, IXmlSerializable
    {
        #region constants

        const float POINT_CAM_NEAR_PLANE = 0.1f;
        const float POINT_CAM_FAR_PLANE = 40.0f;

        #endregion

        #region variables

        private Vector3 lightColor;
        private Vector3 lightSpecular;
        private const float attenuation = 0.15f;

        #endregion

        #region properties

        public Vector3 LightColor
        {
            get
            {
                if (Enabled)
                {
                    return lightColor;
                }
                else
                {
                    return new Vector3(0.0f, 0.0f, 0.0f);
                }
            }
            set
            {
                lightColor = value;
            }
        }
        public Vector3 LightSpecularColor
        {
            get
            {
                if (Enabled)
                {
                    return lightSpecular;
                }
                else
                {
                    return new Vector3(0.0f, 0.0f, 0.0f);
                }
            }
            set
            {
                lightSpecular = value;
            }
        }

        public float Attenuation 
        { 
            get
            {
                return attenuation * MyTransform.Scale;
            }
            set
            {
                
            }
        }
        public bool CastShadows { get; set; }
        public RenderTargetCube ShadowMapRenderTarget512 { get; set; }
        public Camera[] Cameras { get; set; }
        public List<GameObject> AffectedObjects { get; private set; }

        #endregion

        #region methods

        public LightPoint(uint uniqueID, string name)
            : base(uniqueID, name)
        {
            this.AffectedObjects = new List<GameObject>();
        }

        public LightPoint(uint uniqueID, string name, Vector3 lightColor, Vector3 lightSpecularColor, float attenuation, bool castShadows)
            : this(uniqueID, name)
        {
            this.AffectedObjects = new List<GameObject>();
            this.LightColor = lightColor;
            this.LightSpecularColor = lightSpecularColor;
            this.Attenuation = attenuation;
            this.CastShadows = castShadows;
        }

        public void MultiplyAttenuationByScale()
        {
            if (MyTransform != null)
            {
                this.Attenuation *= this.MyTransform.Scale;
            }
            else
            {
                Debug.Log("LightPoint: No Transfrom attached");
            }
        }

        public void GenerateShadowMap()
        {
            if (!this.CastShadows || TrashSoupGame.Instance.ActualRenderTarget != TrashSoupGame.Instance.DefaultRenderTarget || Cameras == null || !this.Enabled)
            {
                return;
            }

            Effect myShadowEffect = ResourceManager.Instance.Effects[@"Effects\ShadowMapUnnormalizedEffect"];
            SamplerState sp = TrashSoupGame.Instance.GraphicsDevice.SamplerStates[0];
            BlendState bs = TrashSoupGame.Instance.GraphicsDevice.BlendState;
            TrashSoupGame.Instance.GraphicsDevice.SamplerStates[0] = SamplerState.PointWrap;
            TrashSoupGame.Instance.GraphicsDevice.BlendState = BlendState.Opaque;

            myShadowEffect.Parameters["LightPos"].SetValue(MyTransform.Position);

            for(int i = 0; i < 6 ; ++i)
            {
                TrashSoupGame.Instance.GraphicsDevice.SetRenderTarget(ShadowMapRenderTarget512, (CubeMapFace)i);
                TrashSoupGame.Instance.GraphicsDevice.Clear(Color.Black);
                ResourceManager.Instance.CurrentScene.DrawAll(Cameras[i], myShadowEffect, TrashSoupGame.Instance.TempGameTime, false);
            }
            TrashSoupGame.Instance.ActualRenderTarget = TrashSoupGame.Instance.DefaultRenderTarget;
            if (sp != null)
                TrashSoupGame.Instance.GraphicsDevice.SamplerStates[0] = sp;
            else
                TrashSoupGame.Instance.GraphicsDevice.SamplerStates[0] = SamplerState.LinearWrap;
            TrashSoupGame.Instance.GraphicsDevice.BlendState = bs;
        }

        public void SetupShadowRender()
        {
            this.Cameras = new Camera[6];

            Vector3 pos = this.MyTransform.PositionGlobal;

            this.Cameras[0] = new Camera(0, "", Vector3.Zero, pos, Vector3.Right, Vector3.Up,
                MathHelper.PiOver2, 1.0f, POINT_CAM_NEAR_PLANE, POINT_CAM_FAR_PLANE);
            this.Cameras[1] = new Camera(0, "", Vector3.Zero, pos, Vector3.Left, Vector3.Up,
                MathHelper.PiOver2, 1.0f, POINT_CAM_NEAR_PLANE, POINT_CAM_FAR_PLANE);
            this.Cameras[2] = new Camera(0, "", Vector3.Zero, pos, Vector3.Up, Vector3.Backward,
                MathHelper.PiOver2, 1.0f, POINT_CAM_NEAR_PLANE, POINT_CAM_FAR_PLANE);
            this.Cameras[3] = new Camera(0, "", Vector3.Zero, pos, Vector3.Down, Vector3.Forward,
                MathHelper.PiOver2, 1.0f, POINT_CAM_NEAR_PLANE, POINT_CAM_FAR_PLANE);
            this.Cameras[4] = new Camera(0, "", Vector3.Zero, pos, Vector3.Forward, Vector3.Up,
                MathHelper.PiOver2, 1.0f, POINT_CAM_NEAR_PLANE, POINT_CAM_FAR_PLANE);
            this.Cameras[5] = new Camera(0, "", Vector3.Zero, pos, Vector3.Backward, Vector3.Up,
                MathHelper.PiOver2, 1.0f, POINT_CAM_NEAR_PLANE, POINT_CAM_FAR_PLANE); 

            this.ShadowMapRenderTarget512 = new RenderTargetCube(
                        TrashSoupGame.Instance.GraphicsDevice,
                        1024,
                        false,
                        SurfaceFormat.Vector2,
                        TrashSoupGame.Instance.GraphicsDevice.PresentationParameters.DepthStencilFormat,
                        TrashSoupGame.Instance.GraphicsDevice.PresentationParameters.MultiSampleCount,
                        RenderTargetUsage.DiscardContents
                        );

            this.MyTransform.PositionChanged += new Transform.PositionChangedEventHandler(UpdateCameras);
        }

        private void UpdateCameras(object sender, EventArgs e)
        {
            foreach(Camera cam in Cameras)
            {
                cam.Translation = ((Transform)sender).PositionGlobal;
                cam.Update(null);
            }
        }

        public override void OnTriggerEnter(GameObject otherGO)
        {
            if(!otherGO.LightsAffecting.Contains(this))
            {
                otherGO.LightsAffecting.Add(this);
                this.AffectedObjects.Add(otherGO);
            }
            
            base.OnTriggerEnter(otherGO);
        }

        public override void OnTriggerExit(GameObject otherGO)
        {
            if (otherGO.LightsAffecting.Contains(this))
            {
                otherGO.LightsAffecting.Remove(this);
                this.AffectedObjects.Remove(otherGO);
            }

            base.OnTriggerExit(otherGO);
        }

        public System.Xml.Schema.XmlSchema GetSchema() { return null; }

        public void ReadXml(System.Xml.XmlReader reader)
        {
            CastShadows = reader.ReadElementContentAsBoolean("CastShadows", "");
            reader.ReadStartElement("Color");
            LightColor = new Vector3(reader.ReadElementContentAsFloat("R", ""),
                reader.ReadElementContentAsFloat("G", ""),
                reader.ReadElementContentAsFloat("B", ""));
            reader.ReadEndElement();

            reader.ReadStartElement("SpecularColor");
            LightSpecularColor = new Vector3(reader.ReadElementContentAsFloat("R", ""),
                reader.ReadElementContentAsFloat("G", ""),
                reader.ReadElementContentAsFloat("B", ""));
            reader.ReadEndElement();

            Attenuation = reader.ReadElementContentAsFloat("Attenuation", "");

            base.ReadXml(reader);

            this.MyCollider = new SphereCollider(this, true);
            this.MyPhysicalObject = new PhysicalObject(this, 0.0f, 0.0f, false);
        }

        public void WriteXml(System.Xml.XmlWriter writer)
        {
            writer.WriteElementString("CastShadows", XmlConvert.ToString(CastShadows));

            writer.WriteStartElement("Color");
            writer.WriteElementString("R", XmlConvert.ToString(LightColor.X));
            writer.WriteElementString("G", XmlConvert.ToString(LightColor.Y));
            writer.WriteElementString("B", XmlConvert.ToString(LightColor.Z));
            writer.WriteEndElement();

            writer.WriteStartElement("SpecularColor");
            writer.WriteElementString("R", XmlConvert.ToString(LightSpecularColor.X));
            writer.WriteElementString("G", XmlConvert.ToString(LightSpecularColor.Y));
            writer.WriteElementString("B", XmlConvert.ToString(LightSpecularColor.Z));
            writer.WriteEndElement();

            writer.WriteElementString("Attenuation", XmlConvert.ToString(Attenuation));

            base.WriteXml(writer);
        }

        #endregion
    }
}
