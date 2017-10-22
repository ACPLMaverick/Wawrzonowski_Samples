using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.Serialization;
using System.Xml.Serialization;
using System.Xml;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace TrashSoup.Engine
{
    public class SceneParams : IXmlSerializable
    {
        #region properties
        public uint UniqueID { get; set; }
        public string Name { get; set; }
        public Vector2 Wind { get; set; }
        public DateTime Time { get; set; }
        public float TimeMultiplier { get; set; }
        public float MaxSize { get; set; }
        public float Lod1Distance { get; set; }
        public float Lod2Distance { get; set; }
        public bool Shadows { get; set; }
        public bool SoftShadows { get; set; }
        public bool Bloom { get; set; }
        public bool UseGraph { get; set; }
        public bool UseLods { get; set; }
        #endregion

        #region methods
        public SceneParams(uint uniqueID, string name)
        {
            this.UniqueID = uniqueID;
            this.Name = name;
        }

        public SceneParams(uint uniqueID, string name, Vector2 wind, DateTime time, float timeM, float maxSize, float lod1distance, float lod2distance, bool shadows, bool softShadows, bool bloom, bool graph, bool lods)
            : this(uniqueID, name)
        {
            this.Wind = wind;
            this.Time = time;
            this.TimeMultiplier = timeM;
            this.MaxSize = maxSize;
            this.Lod1Distance = lod1distance;
            this.Lod2Distance = lod2distance;
            this.Shadows = shadows;
            this.SoftShadows = softShadows;
            this.Bloom = bloom;
            this.UseGraph = graph;
            this.UseLods = lods;
        }

        public System.Xml.Schema.XmlSchema GetSchema()
        {
            return null;
        }

        public void ReadXml(System.Xml.XmlReader reader)
        {
            reader.MoveToContent();
            reader.ReadStartElement();

            this.UniqueID = (uint)reader.ReadElementContentAsInt("UniqueID", "");
            this.Name = reader.ReadElementString("Name", "");

            this.MaxSize = reader.ReadElementContentAsFloat("MaxSize", "");
            this.Lod1Distance = reader.ReadElementContentAsFloat("Lod1Distance", "");
            this.Lod2Distance = reader.ReadElementContentAsFloat("Lod2Distance", "");
            this.Shadows = reader.ReadElementContentAsBoolean("Shadows", "");
            this.SoftShadows = reader.ReadElementContentAsBoolean("SoftShadows", "");
            this.Bloom = reader.ReadElementContentAsBoolean("Bloom", "");
            this.UseGraph = reader.ReadElementContentAsBoolean("UseGraph", "");
            this.UseLods = reader.ReadElementContentAsBoolean("UseLods", "");

            if(reader.Name == "Wind")
            {
                reader.ReadStartElement();
                Wind = new Vector2(reader.ReadElementContentAsFloat("X", ""),
                    reader.ReadElementContentAsFloat("Y", ""));
                reader.ReadEndElement();
            }

            reader.ReadEndElement();
        }

        public void WriteXml(System.Xml.XmlWriter writer)
        {
            writer.WriteElementString("UniqueID", UniqueID.ToString());
            writer.WriteElementString("Name", Name);

            writer.WriteElementString("MaxSize", XmlConvert.ToString(MaxSize));
            writer.WriteElementString("Lod1Distance", XmlConvert.ToString(Lod1Distance));
            writer.WriteElementString("Lod2Distance", XmlConvert.ToString(Lod2Distance));
            writer.WriteElementString("Shadows", XmlConvert.ToString(Shadows));
            writer.WriteElementString("SoftShadows", XmlConvert.ToString(SoftShadows));
            writer.WriteElementString("Bloom", XmlConvert.ToString(Bloom));
            writer.WriteElementString("UseGraph", XmlConvert.ToString(UseGraph));
            writer.WriteElementString("UseLods", XmlConvert.ToString(UseLods));

            writer.WriteStartElement("Wind");
            writer.WriteElementString("X", XmlConvert.ToString(Wind.X));
            writer.WriteElementString("Y", XmlConvert.ToString(Wind.Y));
            writer.WriteEndElement();
        }
        #endregion
    }

    public class Scene : IXmlSerializable
    {
        #region constants

        const int BLUR_FACTOR = 2;
        const float BLUR_FACTOR_IN = 0.4f;

        #endregion

        #region variables

        private bool ifRenderShadows;
        private RenderTarget2D globalShadowsRenderTarget;
        private RenderTarget2D tempRenderTarget01;
        private Matrix deferredOrthoMatrix;
        private List<GameObject> runtimeAdded;
        private List<GameObject> runtimeRemoved;
        private List<GameObject> drawLasts = new List<GameObject>();

        #endregion

        #region properties

        public SceneParams Params { get; set; }
        public Camera Cam { get; set; }
        public EditorCamera EditorCam { get; set; }

        public LightAmbient AmbientLight { get; set; }
        public LightDirectional[] DirectionalLights { get; set; }
        public List<LightPoint> PointLights { get; set; }
        public Dictionary<uint, GameObject> ObjectsDictionary { get; set; }
        public QuadTree ObjectsQT { get; set; }
        public PostEffect CurrentPostEffect { get; set; }
        // place for bounding sphere tree

        #endregion

        #region methods
        public Scene()
        {
            DirectionalLights = new LightDirectional[ResourceManager.DIRECTIONAL_MAX_LIGHTS];
            for (int i = 0; i < ResourceManager.DIRECTIONAL_MAX_LIGHTS; ++i)
                DirectionalLights[i] = null;
            PointLights = new List<LightPoint>();

            ObjectsDictionary = new Dictionary<uint, GameObject>();
            ObjectsQT = new QuadTree(ObjectsDictionary, 3000.0f);
            runtimeAdded = new List<GameObject>();
            runtimeRemoved = new List<GameObject>();

            globalShadowsRenderTarget = new RenderTarget2D(
                        TrashSoupGame.Instance.GraphicsDevice,
                        TrashSoupGame.Instance.Window.ClientBounds.Width / BLUR_FACTOR,
                        TrashSoupGame.Instance.Window.ClientBounds.Height / BLUR_FACTOR,
                        false,
                        TrashSoupGame.Instance.GraphicsDevice.PresentationParameters.BackBufferFormat,
                        TrashSoupGame.Instance.GraphicsDevice.PresentationParameters.DepthStencilFormat,
                        TrashSoupGame.Instance.GraphicsDevice.PresentationParameters.MultiSampleCount,
                        RenderTargetUsage.DiscardContents
                        );

            tempRenderTarget01 = new RenderTarget2D(
                        TrashSoupGame.Instance.GraphicsDevice,
                        TrashSoupGame.Instance.Window.ClientBounds.Width / BLUR_FACTOR,
                        TrashSoupGame.Instance.Window.ClientBounds.Height / BLUR_FACTOR,
                        false,
                        TrashSoupGame.Instance.GraphicsDevice.PresentationParameters.BackBufferFormat,
                        TrashSoupGame.Instance.GraphicsDevice.PresentationParameters.DepthStencilFormat,
                        TrashSoupGame.Instance.GraphicsDevice.PresentationParameters.MultiSampleCount,
                        RenderTargetUsage.DiscardContents
                        );

            deferredOrthoMatrix = Matrix.CreateOrthographicOffCenter(0.0f, (float)TrashSoupGame.Instance.Window.ClientBounds.Width,
                    (float)TrashSoupGame.Instance.Window.ClientBounds.Height, 0, 0, 1);

            ifRenderShadows = true;

            // TEMPORARY!!!!!!!!!!
            CurrentPostEffect = ResourceManager.Instance.PostEffects["Default"];
            ///////////////////////////////////
        }
        public Scene(SceneParams par) : this()
        {
            this.Params = par;           
        }

        public void AddObject(GameObject obj)
        {
            if (this.ObjectsDictionary.ContainsKey(obj.UniqueID))
            {
                Debug.Log("That ID exists");
            }
            this.ObjectsDictionary.Add(obj.UniqueID, obj);
            if(Params.UseGraph)
            {
                ObjectsQT.Add(obj);
            }
        }

        public void AddObjectRuntime(GameObject obj)
        {
            runtimeAdded.Add(obj);
            obj.Initialize();
        }

        public void DeleteObjectRuntime(GameObject obj)
        {
            if(runtimeAdded.Contains(obj))
            {
                runtimeAdded.Remove(obj);
            }
            else
            {
                runtimeRemoved.Add(obj);
            }
        }

        public bool DeleteObject(uint uniqueID)
        {
            GameObject temp;
            bool placek = ObjectsDictionary.TryGetValue(uniqueID, out temp);
            if(temp != null)
            {
                if(Params.UseGraph)
                {
                    ObjectsQT.Remove(temp);
                }
                ObjectsDictionary.Remove(uniqueID);
                return true;
            }
            else return false;
        }

        public GameObject GetObject(uint uniqueID)
        {
            if (!ObjectsDictionary.Keys.Contains(uniqueID))
                return null;
            return ObjectsDictionary[uniqueID];
        }

        public GameObject GetObject(string name)
        {
            foreach(KeyValuePair<uint, GameObject> pair in ObjectsDictionary)
            {
                if(pair.Value.Name == name)
                {
                    return pair.Value;
                }
            }
            return null;
        }

        public List<GameObject> GetObjectsOfType(Type type)
        {
            List<GameObject> ret = new List<GameObject>();
            int count = ObjectsDictionary.Count;
            GameObject temp;
            Stack<GameObject> objStack = new Stack<GameObject>();
            for(int i = 0; i < count; ++i)
            {
                objStack.Push(ObjectsDictionary.ElementAt(i).Value);

                while(objStack.Count > 0)
                {
                    temp = objStack.Pop();

                    if (temp.GetType() == type)
                    {
                        ret.Add(temp);
                    }

                    List<GameObject> children = temp.GetChildren();
                    int cCount = children.Count;
                    for(int j = 0; j < cCount; ++j)
                    {
                        objStack.Push(children[j]);
                    }
                }
            }

            return ret;
        }

        public List<ObjectComponent> GetComponentsOfType(Type type)
        {
            List<ObjectComponent> ret = new List<ObjectComponent>();
            int count = ObjectsDictionary.Count;
            GameObject temp;
            ObjectComponent tempC;
            Stack<GameObject> objStack = new Stack<GameObject>();
            for (int i = 0; i < count; ++i)
            {
                objStack.Push(ObjectsDictionary.ElementAt(i).Value);

                while(objStack.Count > 0)
                {
                    temp = objStack.Pop();
                    if (temp.MyAnimator != null && temp.MyAnimator.GetType() == type)
                    {
                        ret.Add(temp.MyAnimator);
                    }
                    if (temp.MyCarrierSocket != null && temp.MyCarrierSocket.GetType() == type)
                    {
                        ret.Add(temp.MyCarrierSocket);
                    }
                    if (temp.MyCollider != null && temp.MyCollider.GetType() == type)
                    {
                        ret.Add(temp.MyCollider);
                    }
                    if (temp.MyPhysicalObject != null && temp.MyPhysicalObject.GetType() == type)
                    {
                        ret.Add(temp.MyPhysicalObject);
                    }
                    if (temp.MyTransform != null && temp.MyTransform.GetType() == type)
                    {
                        ret.Add(temp.MyTransform);
                    }
                    int countC = temp.Components.Count;
                    for (int j = 0; j < countC; ++j)
                    {
                        tempC = temp.Components[j];
                        if (tempC.GetType() == type)
                        {
                            ret.Add(tempC);
                        }
                    }

                    List<GameObject> children = temp.GetChildren();
                    int cCount = children.Count;
                    for (int j = 0; j < cCount; ++j)
                    {
                        objStack.Push(children[j]);
                    }
                }
            }

            return ret;
        }

        public void UpdateAll(GameTime gameTime)
        {
            ObjectsQT.Update();

            Params.Time = Params.Time.AddMilliseconds(Params.TimeMultiplier * gameTime.ElapsedGameTime.TotalMilliseconds);

            //[vansten] Added testing code for physics simulation
            AI.BehaviorTree.BehaviorTreeManager.Instance.Update(gameTime);
            foreach (GameObject obj in ObjectsDictionary.Values)
            {
                obj.Update(gameTime);
            }
            Cam.Update(gameTime);

            if(runtimeRemoved.Count > 0 || runtimeAdded.Count > 0)
            {
                SolveRuntimeAdditions();
            }
        }

        // draws all gameobjects linearly
        public void DrawAll(Camera cam, Effect effect, GameTime gameTime, bool ifGenerateShadowMaps)
        {
            if(ifGenerateShadowMaps)
            {
                if(Params.Shadows)
                {
                    if (DirectionalLights[0] != null && DirectionalLights[0].CastShadows)
                        DirectionalLights[0].GenerateShadowMap();

                    if (PointLights.Count > 0 && PointLights[0].CastShadows)
                    {
                        PointLights[0].GenerateShadowMap();
                    }

                    if(Params.SoftShadows && ifRenderShadows)
                    {
                        RenderGlobalBlurredShadows();
                        ifRenderShadows = false;
                    }
                }
            }
            
            if(Params.UseGraph)
            {
                ObjectsQT.Draw(cam != null ? cam : this.Cam, effect, gameTime);
            }
            else
            {
                drawLasts.Clear();
                foreach (GameObject obj in ObjectsDictionary.Values)
                {
                    if(obj.DrawLast)
                    {
                        drawLasts.Add(obj);
                    }
                    else
                    {
                        obj.Draw(cam, effect, gameTime);
                    }
                }

                int lCount = drawLasts.Count;
                for(int i = 0; i < lCount; ++i)
                {
                    drawLasts[i].Draw(cam, effect, gameTime);
                }
            }

            if(ifGenerateShadowMaps && !ifRenderShadows)
            {
                ifRenderShadows = true;
            }
        }

        public Texture GetGlobalShadowMap()
        {
            if (this.Params.SoftShadows && (TrashSoupGame.Instance.ActualRenderTarget == TrashSoupGame.Instance.DefaultRenderTarget))
            {
                return (Texture)globalShadowsRenderTarget;
            }

            if(DirectionalLights[0] != null)
            {
                if(DirectionalLights[0].CastShadows)
                {
                    return (Texture)DirectionalLights[0].ShadowMapRenderTarget;
                }
            }
            return null;
        }

        public Texture GetDirectionalShadowMap()
        {
            if (DirectionalLights[0] != null)
            {
                if (DirectionalLights[0].CastShadows)
                {
                    return (Texture)DirectionalLights[0].ShadowMapRenderTarget;
                }
            }
            return null;
        }

        public TextureCube GetPointLight0ShadowMap()
        {
            if (this.Params.SoftShadows && (TrashSoupGame.Instance.ActualRenderTarget == TrashSoupGame.Instance.DefaultRenderTarget))
            {
                return null;
            }

            if(PointLights.Count > 0)
            {
                if(PointLights[0] != null && PointLights[0].CastShadows)
                {
                    return PointLights[0].ShadowMapRenderTarget512;
                }
            }
            return null;
        }

        public void RemovePointLight(LightPoint lp)
        {
            if(!PointLights.Contains(lp))
            {
                return;
            }

            foreach(GameObject obj in lp.AffectedObjects)
            {
                obj.LightsAffecting.Remove(lp);
            }

            PointLights.Remove(lp);
            PhysicsManager.Instance.RemoveCollider(lp.MyCollider);
            PhysicsManager.Instance.RemovePhysicalObject(lp);
        }

        private void SolveRuntimeAdditions()
        {
            int aCount = runtimeAdded.Count;
            int rCount = runtimeRemoved.Count;

            for(int i = 0; i < aCount; ++i)
            {
                AddObject(runtimeAdded[i]);
            }
            for (int i = 0; i < rCount; ++i)
            {
                DeleteObject(runtimeRemoved[i].UniqueID);
            }

            runtimeAdded.Clear();
            runtimeRemoved.Clear();
        }

        private void RenderGlobalBlurredShadows()
        {
            Effect myBlurEffect = ResourceManager.Instance.Effects[@"Effects\POSTBlurEffect"];
            Effect myShadowBlurredEffect = ResourceManager.Instance.Effects[@"Effects\ShadowMapBlurredEffect"];

            TrashSoupGame.Instance.ActualRenderTarget = globalShadowsRenderTarget;
            TrashSoupGame.Instance.GraphicsDevice.Clear(Color.Black);
            ResourceManager.Instance.CurrentScene.DrawAll(null, myShadowBlurredEffect, TrashSoupGame.Instance.TempGameTime, false);
            TrashSoupGame.Instance.ActualRenderTarget = TrashSoupGame.Instance.DefaultRenderTarget;

            myBlurEffect.Parameters["WorldViewProj"].SetValue(deferredOrthoMatrix);
            myBlurEffect.Parameters["ScreenWidth"].SetValue(BLUR_FACTOR_IN * (float)TrashSoupGame.Instance.Window.ClientBounds.Width);
            myBlurEffect.Parameters["ScreenHeight"].SetValue(BLUR_FACTOR_IN * (float)TrashSoupGame.Instance.Window.ClientBounds.Height);

            myBlurEffect.CurrentTechnique = myBlurEffect.Techniques["BlurHorizontal"];

            TrashSoupGame.Instance.ActualRenderTarget = tempRenderTarget01;
            TrashSoupGame.Instance.GraphicsDevice.Clear(Color.Black);
            SpriteBatch batch = TrashSoupGame.Instance.GetSpriteBatch();
            batch.Begin(SpriteSortMode.Texture, TrashSoupGame.Instance.GraphicsDevice.BlendState,
                TrashSoupGame.Instance.GraphicsDevice.SamplerStates[1], TrashSoupGame.Instance.GraphicsDevice.DepthStencilState,
                TrashSoupGame.Instance.GraphicsDevice.RasterizerState, myBlurEffect);

            batch.Draw(globalShadowsRenderTarget, new Rectangle(0, 0, TrashSoupGame.Instance.Window.ClientBounds.Width, TrashSoupGame.Instance.Window.ClientBounds.Height), Color.White);

            batch.End();

            TrashSoupGame.Instance.ActualRenderTarget = TrashSoupGame.Instance.DefaultRenderTarget;

            myBlurEffect.CurrentTechnique = myBlurEffect.Techniques["BlurVertical"];

            TrashSoupGame.Instance.ActualRenderTarget = globalShadowsRenderTarget;
            TrashSoupGame.Instance.GraphicsDevice.Clear(Color.Black);

            batch.Begin(SpriteSortMode.Texture, TrashSoupGame.Instance.GraphicsDevice.BlendState,
                TrashSoupGame.Instance.GraphicsDevice.SamplerStates[1], TrashSoupGame.Instance.GraphicsDevice.DepthStencilState,
                TrashSoupGame.Instance.GraphicsDevice.RasterizerState, myBlurEffect);

            batch.Draw(tempRenderTarget01, new Rectangle(0, 0, TrashSoupGame.Instance.Window.ClientBounds.Width, TrashSoupGame.Instance.Window.ClientBounds.Height), Color.White);

            batch.End();

            TrashSoupGame.Instance.ActualRenderTarget = TrashSoupGame.Instance.DefaultRenderTarget;

            //System.IO.FileStream stream = new System.IO.FileStream("Dupa.jpg", System.IO.FileMode.Create);
            //globalShadowsRenderTarget.SaveAsJpeg(stream, 1024, 1024);
            //stream.Close();
        }

        public System.Xml.Schema.XmlSchema GetSchema()
        {
            return null;
        }

        public void ReadXml(System.Xml.XmlReader reader)
        {
            reader.MoveToContent();
            reader.ReadStartElement();

            ResourceManager.Instance.CurrentScene.ObjectsDictionary = new Dictionary<uint, GameObject>();
            ResourceManager.Instance.CurrentScene.PointLights = new List<LightPoint>();
            DirectionalLights = new LightDirectional[ResourceManager.DIRECTIONAL_MAX_LIGHTS];

            if(reader.Name == "SceneParams")
            {
                Params = new SceneParams(0, "null");
                (Params as IXmlSerializable).ReadXml(reader);
            }

            if (reader.Name == "AmbientLight")
            {
                AmbientLight = new LightAmbient(0, "null");
                (AmbientLight as IXmlSerializable).ReadXml(reader);
            }

            int ctr = 0;
            reader.ReadStartElement("DirectionalLights");
            while (reader.NodeType != System.Xml.XmlNodeType.EndElement)
            {
                if (reader.Name == "DirectionalLight")
                {
                    reader.ReadStartElement();
                    if(reader.Name != "null")
                    {
                        LightDirectional obj = new LightDirectional(0, "");
                        (obj as IXmlSerializable).ReadXml(reader);
                        DirectionalLights[ctr] = obj;
                        ++ctr;
                    }
                    else
                    {
                        reader.ReadElementString("null", "");
                        reader.ReadEndElement();
                        ++ctr;
                    }
                }
            }
            reader.ReadEndElement();

            reader.ReadStartElement("PointLights");
            while(reader.NodeType != System.Xml.XmlNodeType.EndElement)
            {
                if(reader.Name == "PointLight")
                {
                    reader.ReadStartElement();
                    LightPoint pl = new LightPoint(0, "");
                    ResourceManager.Instance.CurrentScene.PointLights.Add(pl);
                    (pl as IXmlSerializable).ReadXml(reader);

                    if (pl.CastShadows) pl.SetupShadowRender();
                    reader.ReadEndElement();
                }
                else
                {
                    reader.ReadElementString("null", "");
                }
            }
            reader.ReadEndElement();

            reader.ReadStartElement("ObjectsDictionary");
            while (reader.NodeType != System.Xml.XmlNodeType.EndElement)
            {
                if (reader.Name == "GameObject")
                {
                    reader.ReadStartElement();
                    GameObject obj = null;
                    uint key = (uint)reader.ReadElementContentAsInt("GameObjectKey", "");
                    if (ResourceManager.Instance.CurrentScene.ObjectsDictionary.TryGetValue(key, out obj))
                    {
                        Debug.Log("GameObject successfully loaded - " + obj.Name);
                        GameObject tmp = null;
                        (tmp as IXmlSerializable).ReadXml(reader);
                    }
                    else
                    {
                        obj = new GameObject(0, "");
                        ResourceManager.Instance.CurrentScene.ObjectsDictionary.Add(key, obj);
                        (obj as IXmlSerializable).ReadXml(reader);
                        Debug.Log("New Gameobject successfully loaded - " + obj.Name);
                    }
                }
                reader.ReadEndElement();
            }
            reader.ReadEndElement();

            if (reader.Name == "Camera")
            {
                Cam = new Camera(0, "null", Vector3.Zero, Vector3.Zero, Vector3.Zero, Vector3.Zero, MathHelper.Pi / 3.0f, 1.0f, 0.1f, 2000.0f);
                reader.ReadStartElement();
                (Cam as IXmlSerializable).ReadXml(reader);
            }

            ObjectsDictionary = ResourceManager.Instance.CurrentScene.ObjectsDictionary;
            PointLights = ResourceManager.Instance.CurrentScene.PointLights;

            reader.ReadEndElement();
        }

        public void WriteXml(System.Xml.XmlWriter writer)
        {
            writer.WriteStartElement("SceneParams");
            (Params as IXmlSerializable).WriteXml(writer);
            writer.WriteEndElement();

            writer.WriteStartElement("AmbientLight");
            (AmbientLight as IXmlSerializable).WriteXml(writer);
            writer.WriteEndElement();

            writer.WriteStartElement("DirectionalLights");
            for (int i = 0; i < ResourceManager.DIRECTIONAL_MAX_LIGHTS; ++i )
            {
                writer.WriteStartElement("DirectionalLight");
                if (DirectionalLights[i] != null)
                    (DirectionalLights[i] as IXmlSerializable).WriteXml(writer);
                else
                    writer.WriteElementString("null","");
                writer.WriteEndElement();
            }
            writer.WriteEndElement();

            writer.WriteStartElement("PointLights");
            if (PointLights.Count > 0)
            {
                foreach (LightPoint pl in PointLights)
                {
                    writer.WriteStartElement("PointLight");
                    (pl as IXmlSerializable).WriteXml(writer);
                    writer.WriteEndElement();
                }   
            }
            else
            {
                writer.WriteElementString("null", "");
            }
            writer.WriteEndElement();

            writer.WriteStartElement("ObjectsDictionary");
            for (int i = 0; i < ObjectsDictionary.Count; ++i )
            {
                writer.WriteStartElement("GameObject");
                writer.WriteElementString("GameObjectKey", ObjectsDictionary.Keys.ElementAt(i).ToString());
                (ObjectsDictionary.Values.ElementAt(i) as IXmlSerializable).WriteXml(writer);
                writer.WriteEndElement();
            }
            writer.WriteEndElement();

            writer.WriteStartElement("Camera");
            (Cam as IXmlSerializable).WriteXml(writer);
            writer.WriteEndElement();

        }
        #endregion
    }
}
