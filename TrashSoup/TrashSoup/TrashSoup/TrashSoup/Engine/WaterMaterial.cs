using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace TrashSoup.Engine
{
    public class WaterMaterial : Material, IXmlSerializable
    {
        #region constants

        public const float REFLECTION_BUFFER_SIZE_MULTIPLIER = 0.25f;
        public const float REFRACTION_BUFFER_SIZE_MULTIPLIER = 0.5f;

        #endregion

        #region effectParameters

        EffectParameter epReflectionMap;
        EffectParameter epRefractionMap;
        EffectParameter epWindVector;
        EffectParameter epReflectViewProj;

        #endregion

        #region variables

        protected static RenderTarget2D reflectionRenderTarget;
        protected static RenderTarget2D refractionRenderTarget;

        protected Matrix reflectionMatrix;
        protected Vector2 tempWind;

        #endregion

        #region properties

        protected static RenderTarget2D ReflectionRenderTarget
        {
            get
            {
                if (reflectionRenderTarget == null)
                {
                    reflectionRenderTarget = new RenderTarget2D(
                        TrashSoupGame.Instance.GraphicsDevice,
                        (int)(TrashSoupGame.Instance.GraphicsDevice.PresentationParameters.BackBufferWidth * REFLECTION_BUFFER_SIZE_MULTIPLIER),
                        (int)(TrashSoupGame.Instance.GraphicsDevice.PresentationParameters.BackBufferHeight * REFLECTION_BUFFER_SIZE_MULTIPLIER),
                        false,
                        TrashSoupGame.Instance.GraphicsDevice.PresentationParameters.BackBufferFormat,
                        TrashSoupGame.Instance.GraphicsDevice.PresentationParameters.DepthStencilFormat,
                        TrashSoupGame.Instance.GraphicsDevice.PresentationParameters.MultiSampleCount,
                        RenderTargetUsage.DiscardContents
                        );
                }
                return reflectionRenderTarget;
            }
            set
            {
                reflectionRenderTarget = value;
            }
        }

        protected static RenderTarget2D RefractionRenderTarget
        {
            get
            {
                if (refractionRenderTarget == null)
                {
                    refractionRenderTarget = new RenderTarget2D(
                        TrashSoupGame.Instance.GraphicsDevice,
                        (int)(TrashSoupGame.Instance.GraphicsDevice.PresentationParameters.BackBufferWidth * REFRACTION_BUFFER_SIZE_MULTIPLIER),
                        (int)(TrashSoupGame.Instance.GraphicsDevice.PresentationParameters.BackBufferHeight * REFRACTION_BUFFER_SIZE_MULTIPLIER),
                        false,
                        TrashSoupGame.Instance.GraphicsDevice.PresentationParameters.BackBufferFormat,
                        TrashSoupGame.Instance.GraphicsDevice.PresentationParameters.DepthStencilFormat,
                        TrashSoupGame.Instance.GraphicsDevice.PresentationParameters.MultiSampleCount,
                        RenderTargetUsage.DiscardContents
                        );
                }
                return refractionRenderTarget;
            }
            set
            {
                refractionRenderTarget = value;
            }
        }

        protected Texture2D ReflectionMap { get; set; }
        protected Texture2D RefractionMap { get; set; }

        #endregion

        #region methods

        public WaterMaterial() : base()
        {
            
        }

        public WaterMaterial(string name, Effect effect)
            : base(name, effect)
        {

        }

        public override void UpdateEffect(Effect effect, Matrix world, Matrix worldViewProj, LightAmbient amb, LightDirectional[] dirs, List<LightPoint> points,
             Texture dSM, TextureCube point0SM, Vector3 eyeVector, BoundingFrustumExtended frustum,
             Matrix[] bones, GameTime gameTime)
        {
            if(effect != null)
            {
                base.UpdateEffect(null, new Matrix(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f),
                    new Matrix(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f),
                    null, null, null, null, null, Vector3.Zero, new BoundingFrustumExtended(Matrix.Identity), null, null);

                return;
            }

            if (!isRendering && TrashSoupGame.Instance.ActualRenderTarget == TrashSoupGame.Instance.DefaultRenderTarget && effect == null)
            {
                isRendering = true;

                DrawRefractionMap(effect, world);

                DrawReflectionMap(effect, world);

                if(!TrashSoupGame.Instance.EditorMode)
                tempWind += ResourceManager.Instance.CurrentScene.Params.Wind * (float)gameTime.ElapsedGameTime.TotalMilliseconds / 1000.0f;
                
                if(epWindVector != null)
                {
                    epWindVector.SetValue(tempWind);
                }
                
                isRendering = false;
            }

            base.UpdateEffect(effect, world, worldViewProj, amb, dirs, points, dSM, point0SM, eyeVector, frustum, bones, gameTime);
        }

        protected Vector4 CreatePlane(Matrix wm, bool clipSide)
        {
            Vector3 objectPosition, objectScale;
            Quaternion objectRotation;
            wm.Decompose(out objectScale, out objectRotation, out objectPosition);

            float planeHeight = -objectPosition.Y + 0.00001f;
            Vector3 normal = new Vector3(0.0f, 1.0f, 0.0f);

            Vector4 planeCoeffs = new Vector4(normal, planeHeight);
            if (clipSide)
                planeCoeffs *= -1.0f;

            return planeCoeffs;
        }

        protected void DrawRefractionMap(Effect effect, Matrix wm)
        {
            if (!TrashSoupGame.Instance.EditorMode)
            {
                Vector4 refractionClip = CreatePlane(wm, false);

                ResourceManager.Instance.CurrentScene.Cam.Bounds.AdditionalClip.D = refractionClip.W;
                ResourceManager.Instance.CurrentScene.Cam.Bounds.AdditionalClip.Normal.X = refractionClip.X;
                ResourceManager.Instance.CurrentScene.Cam.Bounds.AdditionalClip.Normal.Y = refractionClip.Y;
                ResourceManager.Instance.CurrentScene.Cam.Bounds.AdditionalClip.Normal.Z = refractionClip.Z;

                TrashSoupGame.Instance.ActualRenderTarget = RefractionRenderTarget;
                //bool ifShadowsAreEnabled = ResourceManager.Instance.CurrentScene.Params.Shadows;
                //ResourceManager.Instance.CurrentScene.Params.Shadows = false;
                ResourceManager.Instance.CurrentScene.DrawAll(null, effect, TrashSoupGame.Instance.TempGameTime, false);
                TrashSoupGame.Instance.ActualRenderTarget = TrashSoupGame.Instance.DefaultRenderTarget;
                //ResourceManager.Instance.CurrentScene.Params.Shadows = ifShadowsAreEnabled;

                ResourceManager.Instance.CurrentScene.Cam.Bounds.ZeroAllAdditionals();

                this.RefractionMap = RefractionRenderTarget;

                //System.IO.FileStream stream = new System.IO.FileStream("Dupa.jpg", System.IO.FileMode.Create);
                //this.RefractionMap.SaveAsJpeg(stream, 800, 480);
                //stream.Close();

                if (epRefractionMap != null)
                {
                    epRefractionMap.SetValue(RefractionMap);
                }
            }
        }

        protected void DrawReflectionMap(Effect effect, Matrix wm)
        {
            if (!TrashSoupGame.Instance.EditorMode)
            {
                Vector4 refractionClip = CreatePlane(wm, true);
                float z = wm.Translation.Y;
                Camera cCam = ResourceManager.Instance.CurrentScene.Cam;

                cCam.Bounds.AdditionalClip.D = refractionClip.W;
                cCam.Bounds.AdditionalClip.Normal.X = refractionClip.X;
                cCam.Bounds.AdditionalClip.Normal.Y = refractionClip.Y;
                cCam.Bounds.AdditionalClip.Normal.Z = refractionClip.Z;

                Matrix refl = Matrix.CreateReflection(new Plane((new Vector4(refractionClip.X, refractionClip.Y, refractionClip.X, refractionClip.W))));
                Matrix flip = Matrix.CreateReflection(new Plane(new Vector4(0.0f, 1.0f, 0.0f, 0.0f)));

                cCam.ViewMatrix = refl * cCam.ViewMatrix * flip;
                cCam.ViewProjMatrix = cCam.ViewMatrix * cCam.ProjectionMatrix;
                cCam.Bounds.Matrix = cCam.ViewProjMatrix;
                this.reflectionMatrix = wm * cCam.ViewProjMatrix;

                TrashSoupGame.Instance.ActualRenderTarget = ReflectionRenderTarget;
                ResourceManager.Instance.CurrentScene.DrawAll(null, effect, TrashSoupGame.Instance.TempGameTime, false);
                TrashSoupGame.Instance.ActualRenderTarget = TrashSoupGame.Instance.DefaultRenderTarget;

                cCam.Update(TrashSoupGame.Instance.TempGameTime);

                cCam.Bounds.ZeroAllAdditionals();

                this.ReflectionMap = ReflectionRenderTarget;

                //System.IO.FileStream stream = new System.IO.FileStream("Dupa.jpg", System.IO.FileMode.Create);
                //this.ReflectionMap.SaveAsJpeg(stream, 800, 480);
                //stream.Close();

                if (epReflectViewProj != null)
                {
                    epReflectViewProj.SetValue(reflectionMatrix);
                }

                if (epReflectionMap != null)
                {
                    epReflectionMap.SetValue(ReflectionMap);
                }   
            }
        }

        public override void AssignParamsInitialize()
        {
            base.AssignParamsInitialize();

            int pNameHash;

            int rfMap = ("ReflectionMap").GetHashCode();
            int rrMap = ("RefractionMap").GetHashCode();
            int rVP = ("ReflectViewProj").GetHashCode();
            int wv = ("WindVector").GetHashCode();

            foreach (EffectParameter p in MyEffect.Parameters)
            {
                pNameHash = p.Name.GetHashCode();
                if (pNameHash == rfMap)
                {
                    epReflectionMap = p;
                }
                else if (pNameHash == rrMap)
                {
                    epRefractionMap = p;
                }
                else if (pNameHash == rVP)
                {
                    epReflectViewProj = p;
                }
                else if (pNameHash == wv)
                {
                    epWindVector = p;
                }
            }
        }

        public System.Xml.Schema.XmlSchema GetSchema()
        {
            return null;
        }

        public void ReadXml(System.Xml.XmlReader reader)
        {
            base.ReadXml(reader);

            AssignParamsInitialize();
        }

        public void WriteXml(System.Xml.XmlWriter writer)
        {
            base.WriteXml(writer);


        }

        #endregion
    }
}
