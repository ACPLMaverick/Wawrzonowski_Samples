using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Serialization;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay
{
    public class DaytimeChange : ObjectComponent, IXmlSerializable
    {
        #region constants
        private const int TEXTURE_COUNT = 4;
        private const int TEXTURE_DAWN = 0;
        private const int TEXTURE_NOON = 1;
        private const int TEXTURE_DUSK = 2;
        private const int TEXTURE_NIGHT = 3;

        private const long MINUTES_MAX = 60 * 24;
        private const long MS_MAX = MINUTES_MAX * 60000;

        private const float SUN_DISTANCE = 100.0f;

        private Vector3 SUNRISE_COLOR = new Vector3(1.0f, 0.8f, 0.0f);
        private Vector3 RED_COLOR = new Vector3(1.0f, 0.3f, 0.0f);
        private Vector3 DARK_COLOR = new Vector3(0.0f, 0.0f, 0.0f);

        private Vector3 DEFAULT_SUN_DIRECTION = new Vector3(-0.1f, -0.7f, -0.4f);
        private const long SUN_TIME_OFFSET_MS = 60 * 60 * 1000;
        #endregion

        #region variables
        private GameObject sun;
        private LightDirectional lightDay;
        private LightDirectional lightNight;
        private LightDirectional lightTemp;
        private bool switched;
        private LightAmbient ambient;
        private CustomModel myModel;
        private SkyboxMaterial myMaterial;
        private Material sunMaterial;
        private TextureCube[] textures = new TextureCube[TEXTURE_COUNT];
        private PlayerTime cTime;
        private long time;
        private long prevTime;
        private Vector3 startDaylightColor;
        private Vector3 startDaylightSpecular;
        private Vector3 startNightColor;
        private Vector3 startNightSpecular;
        private Vector3 startAmbientColor;
        private Vector3 startSunDiffuse;
        private Vector3 rotationAxe;
        #endregion

        #region properties
        public uint SunID { get; set; }
        public uint LightDayID { get; set; }
        public uint LightNightID { get; set; }
        public string[] TextureNames { get; set; }
        public int SunriseMinutes { get; set; }
        public int SunsetMinutes { get; set; }
        public int StateChangeMinutes { get; set; }

        public long SunriseMS 
        {
            get
            {
                return (long)SunriseMinutes * 60000;
            }
        }
        public long SunsetMS 
        { 
            get
            {
                return (long)SunsetMinutes * 60000;
            }
        }
        public long StateChangeMS 
        { 
            get
            {
                return (long)StateChangeMinutes * 60000;
            }
        }

        public float HorizonOffset { get; set; }
        #endregion

        #region methods

        public DaytimeChange(GameObject go)
            : base(go)
        {
            rotationAxe = new Vector3(-0.2f, 0.2f, -1.0f);
            rotationAxe.Normalize();
        }

        public DaytimeChange(GameObject go, DaytimeChange cc)
            : base(go, cc)
        {
            SunID = cc.SunID;
            LightDayID = cc.LightDayID;
            LightNightID = cc.LightNightID;
            TextureNames = cc.TextureNames;
            SunriseMinutes = cc.SunriseMinutes;
            SunsetMinutes = cc.SunsetMinutes;
            StateChangeMinutes = cc.StateChangeMinutes;
            HorizonOffset = cc.HorizonOffset;
            rotationAxe = new Vector3(-0.5f, 0.2f, -1.5f);
            rotationAxe.Normalize();
        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            if (!TrashSoupGame.Instance.EditorMode)
            {
                prevTime = time;
                //time = 60 * cTime.Hours + cTime.Minutes;
                time = cTime.TotalMilliseconds % MS_MAX;
                if (time < 0 || time > MS_MAX)
                {
                    Debug.Log("DaytimeChange: Time is invalid. Clamping.");
                    time = (int)MathHelper.Clamp((float)time, 0.0f, (float)MS_MAX);
                }

                //if (time != prevTime)
                //{
                    Vector4 probes;
                    ConvertTimeToProbes(time, out probes);
                    myMaterial.Probes = probes;

                    Vector3 lightDir;
                    ConvertTimeToLightDirection(time, out lightDir);
                    lightDay.LightDirection = lightDir;
                    lightNight.LightDirection = -lightDir;

                    SetupSun(lightDir);

                    Vector3 lightCol, lightSpec, nCol, nSpec, ambCol, sunDiff;
                    ConvertTimeToDaylightColor(time, out lightCol, out lightSpec, out nCol, out nSpec, out ambCol, out sunDiff);
                    lightDay.LightColor = lightCol;
                    lightDay.LightSpecularColor = lightSpec;
                    lightNight.LightColor = nCol;
                    lightNight.LightSpecularColor = nSpec;
                    ambient.LightColor = ambCol;
                    sunMaterial.DiffuseColor = sunDiff;

                    //Debug.Log(lightCol.ToString());

                    if (time >= SunriseMS - StateChangeMS && time <= SunsetMS + StateChangeMS)
                    {
                        if (!switched)
                        {
                            lightTemp = ResourceManager.Instance.CurrentScene.DirectionalLights[0];
                            ResourceManager.Instance.CurrentScene.DirectionalLights[0] = ResourceManager.Instance.CurrentScene.DirectionalLights[1];
                            ResourceManager.Instance.CurrentScene.DirectionalLights[1] = lightTemp;
                            lightDay.Enabled = true;
                            lightNight.Enabled = false;
                            switched = true;
                        }
                    }
                    else
                    {
                        if (switched)
                        {
                            lightTemp = ResourceManager.Instance.CurrentScene.DirectionalLights[0];
                            ResourceManager.Instance.CurrentScene.DirectionalLights[0] = ResourceManager.Instance.CurrentScene.DirectionalLights[1];
                            ResourceManager.Instance.CurrentScene.DirectionalLights[1] = lightTemp;
                            lightDay.Enabled = false;
                            lightNight.Enabled = true;
                            switched = false;
                        }
                    }
                //}
            }
        }

        public override void Draw(Engine.Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, Microsoft.Xna.Framework.GameTime gameTime)
        {
            // nth
        }

        protected override void Start()
        {
            // nth
        }

        public override void Initialize()
        {
            if(TextureNames == null)
            {
                throw new ArgumentNullException("DaytimeChange: Textures not set!");
            }
            if(TextureNames.Count() < TEXTURE_COUNT)
            {
                throw new ArgumentException("DaytimeChange: Texture count less than " + TEXTURE_COUNT.ToString() + "!");
            }

            sun = ResourceManager.Instance.CurrentScene.GetObject(SunID);
            lightDay = ResourceManager.Instance.CurrentScene.DirectionalLights[LightDayID];
            lightNight = ResourceManager.Instance.CurrentScene.DirectionalLights[LightNightID];
            ambient = ResourceManager.Instance.CurrentScene.AmbientLight;

            foreach(ObjectComponent comp in this.MyObject.Components)
            {
                if(comp.GetType() == typeof(CustomModel))
                {
                    myModel = (CustomModel)comp;
                    if (myModel == null)
                        break;
                    if(myModel.Mat[0].GetType() != typeof(SkyboxMaterial))
                    {
                        throw new InvalidOperationException("DaytimeChange: Skybox's material is not SkyboxMaterial!");
                    }
                    myMaterial = (SkyboxMaterial)myModel.Mat[0];
                    break;
                }
            }

            if(sun == null || lightDay == null || lightNight == null || myModel == null || myMaterial == null || ambient == null)
            {
                throw new ArgumentNullException("DaytimeChange: Some of the objects do not exist!");
            }

            foreach (ObjectComponent comp in sun.Components)
            {
                if (comp.GetType() == typeof(Billboard))
                {
                    if(((Billboard)comp).Mat != null)
                    {
                        sunMaterial = ((Billboard)comp).Mat;
                    }
                }
            }

            startDaylightColor = lightDay.LightColor;
            startDaylightSpecular = lightDay.LightSpecularColor;
            startNightColor = lightNight.LightColor;
            startNightSpecular = lightNight.LightSpecularColor;
            startAmbientColor = ambient.LightColor;

            for (int i = 0; i < TEXTURE_COUNT; ++i )
            {
                if(TextureNames[i] != null)
                    textures[i] = ResourceManager.Instance.LoadTextureCube(TextureNames[i]);
            }

            if (textures[0] != null)
                myMaterial.CubeMap = textures[0];
            if (textures[1] != null)
                myMaterial.CubeMap1 = textures[1];
            if (textures[2] != null)
                myMaterial.CubeMap2 = textures[2];
            if (textures[3] != null)
                myMaterial.CubeMap3 = textures[3];

            GameObject pt = ResourceManager.Instance.CurrentScene.GetObject("PlayerTime");
            if(pt == null)
            {
                throw new ArgumentNullException("DaytimeChange: PlayerTime object does not exist!");
            }

            foreach(ObjectComponent comp in pt.Components)
            {
                if(comp.GetType() == typeof(PlayerTime))
                {
                    cTime = (PlayerTime)comp;
                }
            }

            if(cTime == null)
            {
                throw new ArgumentNullException("DaytimeChange: PlayerTime object has no PlayerTime component!");
            }

            long t = cTime.TotalMilliseconds;
            if (t >= SunriseMS - StateChangeMS && t <= SunsetMS + StateChangeMS)
                switched = true;
            else 
                switched = false;

            startSunDiffuse = myMaterial.DiffuseColor;

            base.Initialize();
        }

        private void ConvertTimeToProbes(long ms, out Vector4 probes)
        {
            Vector4 state0 = new Vector4(1.0f, 0.0f, 0.0f, 0.0f);
            Vector4 state1 = new Vector4(0.0f, 1.0f, 0.0f, 0.0f);
            Vector4 state2 = new Vector4(0.0f, 0.0f, 1.0f, 0.0f);
            Vector4 state3 = new Vector4(0.0f, 0.0f, 0.0f, 1.0f);
            float lerpValue;

            if (ms < ((SunriseMS - StateChangeMS) % MS_MAX) || ms > (SunsetMS + StateChangeMS))
            {
                probes = state3;
            }
            else if(ms > SunriseMS + StateChangeMS && ms < SunsetMS - StateChangeMS)
            {
                probes = state1;
            }
            else if(ms == SunriseMS)
            {
                probes = state0;
            }
            else if(ms == SunsetMS)
            {
                probes = state2;
            }
            else if(ms >= SunriseMS - StateChangeMS && ms < SunriseMS)
            {
                //state3 vs state0
                float x = (float)(SunriseMS - StateChangeMS);
                float y = (float)SunriseMS;
                lerpValue = (((float)ms) - x) / (y - x);
                probes = Vector4.Lerp(state3, state0, lerpValue);
            }
            else if(ms > SunriseMS && ms <= SunriseMS + StateChangeMS)
            {
                //state0 vs state1
                float x = (float)(SunriseMS);
                float y = (float)(SunriseMS + StateChangeMS);
                lerpValue = (((float)ms) - x) / (y - x);
                probes = Vector4.Lerp(state0, state1, lerpValue);
            }
            else if(ms >= SunsetMS - StateChangeMS && ms < SunsetMS)
            {
                //state1 vs state2
                float x = (float)(SunsetMS - StateChangeMS);
                float y = (float)(SunsetMS);
                lerpValue = (((float)ms) - x) / (y - x);
                probes = Vector4.Lerp(state1, state2, lerpValue);
            }
            else if(ms > SunsetMS && ms <= SunsetMS + StateChangeMS)
            {
                //state2 vs state3
                float x = (float)(SunsetMS);
                float y = (float)(SunsetMS + StateChangeMS);
                lerpValue = (((float)ms) - x) / (y - x);
                probes = Vector4.Lerp(state2, state3, lerpValue);
            }
            else
            {
                Debug.Log("DaytimeChange: ConvertTimeToProbes has fucked up somehow. Time given is " + ms.ToString());
                probes = new Vector4(0.0f, 0.0f, 0.0f, 0.0f);
            }
        }

        private void ConvertTimeToLightDirection(long ms, out Vector3 direction)
        {
            direction = DEFAULT_SUN_DIRECTION;   // dla minutes = 720
            //direction = Vector3.Transform(direction, Matrix.CreateRotationY(-MathHelper.PiOver4 / 1.5f));
            
            double rotation = ((double)((ms - MS_MAX / 2) % MS_MAX) / (double)MS_MAX) * MathHelper.TwoPi;
            
            direction = Vector3.Transform(direction, Matrix.CreateFromAxisAngle(rotationAxe, -(float)rotation));

            //Debug.Log(direction.ToString());
            direction.Z = -direction.Z;
            direction.Y -= HorizonOffset;
            direction.Normalize();
        }

        private void ConvertTimeToDaylightColor(long ms, out Vector3 color, out Vector3 specular, out Vector3 nColor, out Vector3 nSpecular, out Vector3 ambientColor, out Vector3 sunDiff)
        {
            ms = ms - SUN_TIME_OFFSET_MS % MS_MAX;
            float lerpValue;
            Vector3 smallAmbient = startAmbientColor * 0.25f;
            int de = 30;

            if (ms < ((SunriseMS - (StateChangeMS + de)) % MS_MAX) || ms > (SunsetMS + (StateChangeMS + de)))
            {
                // night
                color = DARK_COLOR;
                specular = DARK_COLOR;
                ambientColor = DARK_COLOR;
                nColor = startNightColor;
                nSpecular = startNightSpecular;
                sunDiff = RED_COLOR;
            }
            else if(ms >= (SunriseMS - (StateChangeMS + de)) && ms < (SunriseMS - StateChangeMS))
            {
                // night fade out
                float x = (float)(SunriseMS - (StateChangeMS + de));
                float y = (float)(SunriseMS - StateChangeMS);
                lerpValue = (((float)ms) - x) / (y - x);
                color = DARK_COLOR;
                specular = DARK_COLOR;
                ambientColor = DARK_COLOR;
                nColor = Vector3.Lerp(startNightColor, DARK_COLOR, lerpValue);
                nSpecular = Vector3.Lerp(startNightSpecular, DARK_COLOR, lerpValue);
                sunDiff = RED_COLOR;
            }
            else if (ms <= (SunsetMS + (StateChangeMS + de)) && ms > (SunsetMS + StateChangeMS))
            {
                // night fade in
                float x = (float)(SunsetMS + StateChangeMS);
                float y = (float)(SunsetMS + (StateChangeMS + de));
                lerpValue = (((float)ms) - x) / (y - x);
                color = DARK_COLOR;
                specular = DARK_COLOR;
                ambientColor = DARK_COLOR;
                nColor = Vector3.Lerp(DARK_COLOR, startNightColor, lerpValue);
                nSpecular = Vector3.Lerp(DARK_COLOR, startNightSpecular, lerpValue);
                sunDiff = RED_COLOR;
            }
            else if (ms > SunriseMS + StateChangeMS && ms < SunsetMS - StateChangeMS)
            {
                // day
                color = startDaylightColor;
                specular = startDaylightSpecular;
                ambientColor = startAmbientColor;
                nColor = DARK_COLOR;
                nSpecular = DARK_COLOR;
                sunDiff = startSunDiffuse;
            }
            else if (ms == SunriseMS || ms == SunsetMS)
            {
                // sunrise n sunset
                color = SUNRISE_COLOR;
                specular = SUNRISE_COLOR;
                ambientColor = Vector3.Lerp(startAmbientColor, DARK_COLOR, 0.5f);
                nColor = DARK_COLOR;
                nSpecular = DARK_COLOR;
                sunDiff = RED_COLOR;
            }
            else if (ms >= SunriseMS - StateChangeMS && ms < SunriseMS)
            {
                //state3 vs state0
                float x = (float)(SunriseMS - StateChangeMS);
                float y = (float)SunriseMS;
                lerpValue = (((float)ms) - x) / (y - x);
                color = Vector3.Lerp(DARK_COLOR, SUNRISE_COLOR, lerpValue);
                specular = Vector3.Lerp(DARK_COLOR, SUNRISE_COLOR, lerpValue);
                ambientColor = Vector3.Lerp(smallAmbient, startAmbientColor, lerpValue / 2.0f);
                nColor = DARK_COLOR;
                nSpecular = DARK_COLOR;
                sunDiff = RED_COLOR;
            }
            else if (ms > SunriseMS && ms <= SunriseMS + StateChangeMS)
            {
                //state0 vs state1
                float x = (float)(SunriseMS);
                float y = (float)(SunriseMS + StateChangeMS);
                lerpValue = (((float)ms) - x) / (y - x);
                color = Vector3.Lerp(SUNRISE_COLOR, startDaylightColor, lerpValue);
                specular = Vector3.Lerp(SUNRISE_COLOR, startDaylightSpecular, lerpValue);
                ambientColor = Vector3.Lerp(smallAmbient, startAmbientColor, lerpValue / 2.0f + 0.5f);
                nColor = DARK_COLOR;
                nSpecular = DARK_COLOR;
                sunDiff = Vector3.Lerp(RED_COLOR, startSunDiffuse, lerpValue);
            }
            else if (ms >= SunsetMS - StateChangeMS && ms < SunsetMS)
            {
                //state1 vs state2
                float x = (float)(SunsetMS - StateChangeMS);
                float y = (float)(SunsetMS);
                lerpValue = (((float)ms) - x) / (y - x);
                color = Vector3.Lerp(startDaylightColor, SUNRISE_COLOR, lerpValue);
                specular = Vector3.Lerp(startDaylightSpecular, SUNRISE_COLOR, lerpValue);
                ambientColor = Vector3.Lerp(startAmbientColor, smallAmbient, lerpValue / 2.0f);
                nColor = DARK_COLOR;
                nSpecular = DARK_COLOR;
                sunDiff = Vector3.Lerp(startSunDiffuse, RED_COLOR, lerpValue);
            }
            else if (ms > SunsetMS && ms <= SunsetMS + StateChangeMS)
            {
                //state2 vs state3
                float x = (float)(SunsetMS);
                float y = (float)(SunsetMS + StateChangeMS);
                lerpValue = (((float)ms) - x) / (y - x);
                color = Vector3.Lerp(SUNRISE_COLOR, DARK_COLOR, lerpValue);
                specular = Vector3.Lerp(SUNRISE_COLOR, DARK_COLOR, lerpValue);
                ambientColor = Vector3.Lerp(startAmbientColor, smallAmbient, lerpValue / 2.0f + 0.5f);
                nColor = DARK_COLOR;
                nSpecular = DARK_COLOR;
                sunDiff = RED_COLOR;
            }
            else
            {
                Debug.Log("DaytimeChange: ConvertTimeToDaylightColor has fucked up somehow. Time given is " + ms.ToString());
                color = new Vector3(1.0f, 1.0f, 1.0f);
                specular = new Vector3(1.0f, 1.0f, 1.0f);
                ambientColor = new Vector3(0.0f, 0.0f, 0.0f);
                nColor = DARK_COLOR;
                nSpecular = DARK_COLOR;
                sunDiff = DARK_COLOR;
            }
        }

        private void SetupSun(Vector3 pos)
        {
            pos.X = -pos.X;
            pos.Y = -pos.Y;
            float size = ResourceManager.Instance.CurrentScene.Params.MaxSize;
            float smallDiagonal = (size * (float) Math.Sqrt(2)) / 4.0f;
            sun.MyTransform.Position = new Vector3((pos * smallDiagonal).X, (pos * smallDiagonal).Y, (pos * smallDiagonal).Z);
        }

        public override System.Xml.Schema.XmlSchema GetSchema()
        {
            return base.GetSchema();
        }

        public override void ReadXml(System.Xml.XmlReader reader)
        {
            reader.MoveToContent();
            reader.ReadStartElement();

            base.ReadXml(reader);

            SunID = (uint)reader.ReadElementContentAsInt("SunID", "");
            LightDayID = (uint)reader.ReadElementContentAsInt("LightDayID", "");
            LightNightID = (uint)reader.ReadElementContentAsInt("LightNightID", "");

            if(reader.Name == "TextureNames")
            {
                TextureNames = new string[TEXTURE_COUNT];
                reader.ReadStartElement();
                for (int i = 0; i < TEXTURE_COUNT; ++i)
                {
                    TextureNames[i] = reader.ReadElementString("TextureName");
                }
                reader.ReadEndElement();
            }

            SunriseMinutes = reader.ReadElementContentAsInt("SunriseMinutes", "");
            SunsetMinutes = reader.ReadElementContentAsInt("SunsetMinutes", "");
            StateChangeMinutes = reader.ReadElementContentAsInt("StateChangeMinutes", "");
            HorizonOffset = reader.ReadElementContentAsFloat("HorizonOffset", "");
            
            reader.ReadEndElement();
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            base.WriteXml(writer);

            writer.WriteElementString("SunID", XmlConvert.ToString(SunID));
            writer.WriteElementString("LightDayID", XmlConvert.ToString(LightDayID));
            writer.WriteElementString("LightNightID", XmlConvert.ToString(LightNightID));
            if(TextureNames != null)
            {
                writer.WriteStartElement("TextureNames");
                for (int i = 0; i < TEXTURE_COUNT; ++i)
                {
                    writer.WriteElementString("TextureName", TextureNames[i]);
                }
                writer.WriteEndElement();
            }
            writer.WriteElementString("SunriseMinutes", XmlConvert.ToString(SunriseMinutes));
            writer.WriteElementString("SunsetMinutes", XmlConvert.ToString(SunsetMinutes));
            writer.WriteElementString("StateChangeMinutes", XmlConvert.ToString(StateChangeMinutes));
            writer.WriteElementString("HorizonOffset", XmlConvert.ToString(HorizonOffset));
        }

        #endregion
    }
}
