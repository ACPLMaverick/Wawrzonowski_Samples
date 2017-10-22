using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Serialization;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace TrashSoup.Engine
{
    // A container for textures and effects associated with the model
    public class Material : IXmlSerializable
    {
        #region enums

        #endregion

        #region effectTechniques

        EffectTechnique etUnlit;
        EffectTechnique etMain;
        EffectTechnique etMainShadows;
        EffectTechnique etSkinned;
        EffectTechnique etSkinnedShadows;
        EffectTechnique etMainBlurredShadows;
        EffectTechnique etSkinnedBlurredShadows;

        #endregion

        #region effectParameters

        protected EffectParameter epWorld;
        protected EffectParameter epWorldInverseTranspose;
        protected EffectParameter epWorldViewProj;
        protected EffectParameter epDiffuseMap;
        protected EffectParameter epNormalMap;
        protected EffectParameter epCubeMap;
        protected EffectParameter epDiffuseColor;
        protected EffectParameter epSpecularColor;
        protected EffectParameter epGlossiness;
        protected EffectParameter epReflectivityColor;
        protected EffectParameter epReflectivityBias;
        protected EffectParameter epTransparency;
        protected EffectParameter epPerPixelLighting;

        protected EffectParameter epBones;

        protected EffectParameter epAmbientLightColor;
        protected EffectParameter epDirLight0Direction;
        protected EffectParameter epDirLight0DiffuseColor;
        protected EffectParameter epDirLight0SpecularColor;
        protected EffectParameter epDirLight0ShadowMap0;
        protected EffectParameter epDirLight0WorldViewProj;
        protected EffectParameter epDirLight0WorldViewProj1;
        protected EffectParameter epDirLight0WorldViewProj2;
        protected EffectParameter epDirLight1Direction;
        protected EffectParameter epDirLight1DiffuseColor;
        protected EffectParameter epDirLight1SpecularColor;
        protected EffectParameter epDirLight2Direction;
        protected EffectParameter epDirLight2DiffuseColor;
        protected EffectParameter epDirLight2SpecularColor;

        protected EffectParameter epPointLightDiffuseColors;
        protected EffectParameter epPointLightSpecularColors;
        protected EffectParameter epPointLightPositions;
        protected EffectParameter epPointLightAttenuations;
        protected EffectParameter epPointLightCount;
        protected EffectParameter epPoint0ShadowMap;
        protected EffectParameter epSSA;

        protected EffectParameter epEyePosition;
        protected EffectParameter epBoundingFrustum;
        protected EffectParameter epCustomClippingPlane;

        #endregion

        #region variables

        protected static bool isRendering = false;

        protected static Vector3[] tempPLColors = new Vector3[ResourceManager.POINT_MAX_LIGHTS_PER_OBJECT];
        protected static Vector3[] tempPLSpeculars = new Vector3[ResourceManager.POINT_MAX_LIGHTS_PER_OBJECT];
        protected static Vector3[] tempPLPositions = new Vector3[ResourceManager.POINT_MAX_LIGHTS_PER_OBJECT];
        protected static float[] tempPLAttenuations = new float[ResourceManager.POINT_MAX_LIGHTS_PER_OBJECT];

        protected Texture2D diffuseMap;
        protected Texture2D normalMap;
        protected TextureCube cubeMap;

        protected Vector3 diffuseColor;

        protected Vector3 specularColor;
        protected float glossiness;

        protected Vector3 reflectivityColor;
        protected float reflectivityBias;

        protected float transparency;

        protected bool perPixelLighting;

        protected Vector4[] tempFrustumArray;
        protected Vector4 additionalClipPlane;

        protected BasicEffect tempBEref;
        protected SkinnedEffect tempSEref;

        protected Effect tempEffect;

        #endregion

        #region properties

        public string Name { get; set; }

        public bool Unlit { get; set; }

        public Texture2D DiffuseMap
        {
            get
            {
                return diffuseMap;
            }
            set
            {
                diffuseMap = value;
            }
        }

        public Texture2D NormalMap
        {
            get
            {
                return normalMap;
            }
            set
            {
                normalMap = value;
            }
        }

        public TextureCube CubeMap
        {
            get
            {
                return cubeMap;
            }
            set
            {
                cubeMap = value;
            }
        }

        public Vector3 DiffuseColor
        {
            get
            {
                return diffuseColor;
            }
            set
            {
                diffuseColor = value;
            }
        }

        public Vector3 SpecularColor
        {
            get
            {
                return specularColor;
            }
            set
            {
                specularColor = value;
            }
        }

        public float Glossiness
        {
            get
            {
                return glossiness;
            }
            set
            {
                glossiness = value;
            }
        }

        public Vector3 ReflectivityColor
        {
            get
            {
                return reflectivityColor;
            }
            set
            {
                reflectivityColor = value;
            }
        }

        public float ReflectivityBias       // 0 means 0% reflection, 100% texture, 1 means otherwise
        {
            get
            {
                return reflectivityBias;
            }
            set
            {
                reflectivityBias = MathHelper.Clamp(value, 0.0f, 1.0f);
            }
        }

        public float Transparency       // 1.0 means opaque
        {
            get
            {
                return transparency;
            }
            set
            {
                transparency = MathHelper.Clamp(value, 0.0f, 1.0f);
            }
        }

        public bool PerPixelLighting
        {
            get
            {
                return perPixelLighting;
            }
            set
            {
                this.perPixelLighting = value;
            }
        }

        public bool RecieveShadows { get; set; }

        public Effect MyEffect { get; set; }

        #endregion

        #region methods

        public Material()
        {

            if (ResourceManager.Instance.Textures.ContainsKey("DefaultDiffuse"))
            {
                this.DiffuseMap = ResourceManager.Instance.Textures["DefaultDiffuse"];
            }
            if (ResourceManager.Instance.Textures.ContainsKey("DefaultNormal"))
            {
                this.NormalMap = ResourceManager.Instance.Textures["DefaultNormal"];
            }
            if (ResourceManager.Instance.Textures.ContainsKey("DefaultCube"))
            {
                this.CubeMap = ResourceManager.Instance.TexturesCube["DefaultCube"];
            }
            this.DiffuseColor = new Vector3(1.0f, 1.0f, 1.0f);
            this.SpecularColor = new Vector3(1.0f, 1.0f, 1.0f);
            this.Glossiness = 50.0f;
            this.ReflectivityColor = new Vector3(1.0f, 1.0f, 1.0f);
            this.ReflectivityBias = 0.2f;
            this.Transparency = 1.0f;
            this.perPixelLighting = false;
            this.RecieveShadows = false;
            this.tempFrustumArray = new Vector4[4];
            Unlit = false;

        }

        public Material(string name, Effect effect)
        {
            this.MyEffect = effect;

            this.Name = name;

            if(ResourceManager.Instance.Textures.ContainsKey("DefaultDiffuse"))
            {
                this.DiffuseMap = ResourceManager.Instance.Textures["DefaultDiffuse"];
            }
            if (ResourceManager.Instance.Textures.ContainsKey("DefaultNormal"))
            {
                this.NormalMap = ResourceManager.Instance.Textures["DefaultNormal"];
            }
            if (ResourceManager.Instance.Textures.ContainsKey("DefaultCube"))
            {
                this.CubeMap = ResourceManager.Instance.TexturesCube["DefaultCube"];
            }
            this.DiffuseColor = new Vector3(1.0f, 1.0f, 1.0f);
            this.SpecularColor = new Vector3(1.0f, 1.0f, 1.0f);
            this.Glossiness = 50.0f;
            this.ReflectivityColor = new Vector3(1.0f, 1.0f, 1.0f);
            this.ReflectivityBias = 0.2f;
            this.Transparency = 1.0f;
            this.perPixelLighting = false;
            this.RecieveShadows = false;
            this.tempFrustumArray = new Vector4[4];
            Unlit = false;

            AssignParamsInitialize();
        }

        public Material(string name, Effect effect, Texture2D diffuse)
            : this(name, effect)
        {
            this.DiffuseMap = diffuse;
        }

        public virtual void UpdateEffect(Effect effect, Matrix world, Matrix worldViewProj, LightAmbient amb, LightDirectional[] dirs, List<LightPoint> points, Texture dSM, TextureCube point0SM, 
            Vector3 eyeVector, BoundingFrustumExtended frustum, Matrix[] bones, GameTime gameTime)
        {
            if (effect != null && tempEffect == null)
            {
                tempEffect = MyEffect;
                MyEffect = effect;
                AssignParamsInitialize();
            }

            if (epWorld != null)
            {
                epWorld.SetValue(world);
            }
            if (epWorldInverseTranspose != null)
            {
                epWorldInverseTranspose.SetValue(Matrix.Transpose(Matrix.Invert(world)));
            }
            if (epWorldViewProj != null)
            {
                epWorldViewProj.SetValue(worldViewProj);
            }
            if (epDiffuseMap != null)
            {
                epDiffuseMap.SetValue(DiffuseMap);
            }
            if (epNormalMap != null)
            {
                epNormalMap.SetValue(NormalMap);
            }
            if (epCubeMap != null)
            {
                epCubeMap.SetValue(CubeMap);
            }
            if (epDiffuseColor != null)
            {
                epDiffuseColor.SetValue(diffuseColor);
            }
            if (epSpecularColor != null)
            {
                epSpecularColor.SetValue(specularColor);
            }
            if (epGlossiness != null)
            {
                epGlossiness.SetValue(glossiness);
            }
            if (epReflectivityColor != null)
            {
                epReflectivityColor.SetValue(reflectivityColor);
            }
            if (epReflectivityBias != null)
            {
                epReflectivityBias.SetValue(reflectivityBias);
            }
            if (epTransparency != null)
            {
                epTransparency.SetValue(transparency);
            }
            if (epPerPixelLighting != null)
            {
                epPerPixelLighting.SetValue(perPixelLighting);
            }

            if (epAmbientLightColor != null && amb != null)
            {
                epAmbientLightColor.SetValue(amb.LightColor);
            }

            if(dirs != null)
            {
                if (dirs[0] != null)
                {
                    if (epDirLight0Direction != null)
                    {
                        epDirLight0Direction.SetValue(dirs[0].LightDirection);
                    }
                    if (epDirLight0DiffuseColor != null)
                    {
                        epDirLight0DiffuseColor.SetValue(dirs[0].LightColor);
                    }
                    if (epDirLight0SpecularColor != null)
                    {
                        epDirLight0SpecularColor.SetValue(dirs[0].LightSpecularColor);
                    }
                    if (dirs[0].CastShadows)
                    {
                        if (epDirLight0ShadowMap0 != null && dSM != null)
                        {
                            epDirLight0ShadowMap0.SetValue(dSM);
                        }
                        if (epDirLight0WorldViewProj != null && dirs[0].ShadowDrawCameras[0] != null)
                        {
                            epDirLight0WorldViewProj.SetValue(world * dirs[0].ShadowDrawCameras[0].ViewProjMatrix);
                        }
                        if (epDirLight0WorldViewProj1 != null && dirs[0].ShadowDrawCameras[1] != null)
                        {
                            epDirLight0WorldViewProj1.SetValue(world * dirs[0].ShadowDrawCameras[1].ViewProjMatrix);
                        }
                        if (epDirLight0WorldViewProj2 != null && dirs[0].ShadowDrawCameras[2] != null)
                        {
                            epDirLight0WorldViewProj2.SetValue(world * dirs[0].ShadowDrawCameras[2].ViewProjMatrix);
                        }
                    }
                }

                if (dirs[1] != null)
                {
                    if (epDirLight1Direction != null)
                    {
                        epDirLight1Direction.SetValue(dirs[1].LightDirection);
                    }
                    if (epDirLight1DiffuseColor != null)
                    {
                        epDirLight1DiffuseColor.SetValue(dirs[1].LightColor);
                    }
                    if (epDirLight1SpecularColor != null)
                    {
                        epDirLight1SpecularColor.SetValue(dirs[1].LightSpecularColor);
                    }
                }

                if (dirs[2] != null)
                {
                    if (epDirLight2Direction != null)
                    {
                        epDirLight2Direction.SetValue(dirs[2].LightDirection);
                    }
                    if (epDirLight2DiffuseColor != null)
                    {
                        epDirLight2DiffuseColor.SetValue(dirs[2].LightColor);
                    }
                    if (epDirLight2SpecularColor != null)
                    {
                        epDirLight2SpecularColor.SetValue(dirs[2].LightSpecularColor);
                    }
                }
            }

            if(points != null)
            {
                for (int i = 0; i < ResourceManager.POINT_MAX_LIGHTS_PER_OBJECT; ++i)
                {
                    tempPLColors[i] = new Vector3(0.0f, 0.0f, 0.0f);
                    tempPLSpeculars[i] = new Vector3(0.0f, 0.0f, 0.0f);
                    tempPLPositions[i] = new Vector3(0.0f, 0.0f, 0.0f);
                    tempPLAttenuations[i] = 0.00000001f;
                }

                int pCount = points.Count;
                if(epSSA != null)
                    epSSA.SetValue(1.0f);

                for (int i = 0; i < pCount && i < ResourceManager.POINT_MAX_LIGHTS_PER_OBJECT; ++i )
                {
                    if (points[i].CastShadows && epSSA != null)
                    {
                        epSSA.SetValue(0.0f);
                    }

                    tempPLColors[i] = points[i].LightColor;
                    tempPLSpeculars[i] = points[i].LightSpecularColor;
                    tempPLPositions[i] = points[i].MyTransform.PositionGlobal;
                    tempPLAttenuations[i] = points[i].Attenuation;
                }

                if (epPointLightDiffuseColors != null)
                {
                    epPointLightDiffuseColors.SetValue(tempPLColors);
                }
                if (epPointLightSpecularColors != null)
                {
                    epPointLightSpecularColors.SetValue(tempPLSpeculars);
                }
                if (epPointLightPositions != null)
                {
                    epPointLightPositions.SetValue(tempPLPositions);
                }
                if (epPointLightAttenuations != null)
                {
                    epPointLightAttenuations.SetValue(tempPLAttenuations);
                }
                if (epPointLightCount != null)
                {
                    epPointLightCount.SetValue(points.Count);
                }
                if (epPoint0ShadowMap != null && point0SM != null)
                {
                    epPoint0ShadowMap.SetValue(point0SM);
                }
            }

            if (epEyePosition != null)
            {
                epEyePosition.SetValue(eyeVector);
            }

            // bounding frustum
            tempFrustumArray[0].W = -frustum.Top.D + BoundingFrustumExtended.CLIP_MARGIN;
            tempFrustumArray[0].X = -frustum.Top.Normal.X;
            tempFrustumArray[0].Y = -frustum.Top.Normal.Y;
            tempFrustumArray[0].Z = -frustum.Top.Normal.Z;

            tempFrustumArray[1].W = -frustum.Bottom.D + BoundingFrustumExtended.CLIP_MARGIN;
            tempFrustumArray[1].X = -frustum.Bottom.Normal.X;
            tempFrustumArray[1].Y = -frustum.Bottom.Normal.Y;
            tempFrustumArray[1].Z = -frustum.Bottom.Normal.Z;

            tempFrustumArray[2].W = -frustum.Left.D + BoundingFrustumExtended.CLIP_MARGIN;
            tempFrustumArray[2].X = -frustum.Left.Normal.X;
            tempFrustumArray[2].Y = -frustum.Left.Normal.Y;
            tempFrustumArray[2].Z = -frustum.Left.Normal.Z;

            tempFrustumArray[3].W = -frustum.Right.D + BoundingFrustumExtended.CLIP_MARGIN;
            tempFrustumArray[3].X = -frustum.Right.Normal.X;
            tempFrustumArray[3].Y = -frustum.Right.Normal.Y;
            tempFrustumArray[3].Z = -frustum.Right.Normal.Z;

            if (epBoundingFrustum != null)
            {
                epBoundingFrustum.SetValue(tempFrustumArray);
            }

            additionalClipPlane.W = -frustum.AdditionalClip.D;
            additionalClipPlane.X = -frustum.AdditionalClip.Normal.X;
            additionalClipPlane.Y = -frustum.AdditionalClip.Normal.Y;
            additionalClipPlane.Z = -frustum.AdditionalClip.Normal.Z;

            if (epCustomClippingPlane != null)
            {
                epCustomClippingPlane.SetValue(additionalClipPlane);
            }

            if (epBones != null && bones != null)
            {
                epBones.SetValue(bones);
                this.MyEffect.CurrentTechnique = etSkinned;
            }

            // setting up techniques

            bool shadows = ResourceManager.Instance.CurrentScene.Params.Shadows;
            bool softShadows = ResourceManager.Instance.CurrentScene.Params.SoftShadows;
            if(tempBEref == null && tempSEref == null)
            {
                bool forShadows = ((dirs != null && dirs[0] != null && dirs[0].CastShadows) || (point0SM != null)) && RecieveShadows && shadows;

                if((Unlit || TrashSoupGame.Instance.EditorMode) && etUnlit != null)
                {
                    MyEffect.CurrentTechnique = etUnlit;
                }
                else if (bones != null && forShadows && etSkinnedBlurredShadows != null && softShadows)
                {
                    MyEffect.CurrentTechnique = etSkinnedBlurredShadows;
                }
                else if (bones == null && forShadows && etMainBlurredShadows != null && softShadows)
                {
                    MyEffect.CurrentTechnique = etMainBlurredShadows;
                }
                else if (bones != null && forShadows && etSkinnedShadows != null)
                {
                    MyEffect.CurrentTechnique = etSkinnedShadows;
                }
                else if (bones == null && forShadows && etMainShadows != null)
                {
                    MyEffect.CurrentTechnique = etMainShadows;
                }
                else if (bones != null && etSkinned != null)
                {
                    MyEffect.CurrentTechnique = etSkinned;
                }
                else
                {
                    MyEffect.CurrentTechnique = etMain;
                }
            }

            //////////////////////

            //////////////////////

            if (tempBEref != null)
            {
                // do shit for basicEffect
                tempBEref.PreferPerPixelLighting = perPixelLighting;
            }
            if (tempSEref != null)
            {
                // do shit for skinnedEffect
                tempSEref.PreferPerPixelLighting = perPixelLighting;
            }
        }

        public void FlushMaterialEffect()
        {
            if (tempEffect != null)
            {
                MyEffect = tempEffect;
                tempEffect = null;
                AssignParamsInitialize();
            }
        }

        public virtual void AssignParamsInitialize()
        {
            if (MyEffect == null) throw new NullReferenceException("MyEffect iz null and you tryin' to extract params from it, nigga?");

            etMain = null;
            etSkinned = null;
            etMainShadows = null;
            etSkinnedShadows = null;

            epWorld = null;
            epWorldInverseTranspose = null;
            epWorldViewProj = null;
            epDiffuseMap = null;
            epNormalMap = null;
            epCubeMap = null;
            epDiffuseColor = null;
            epSpecularColor = null;
            epGlossiness = null;
            epReflectivityColor = null;
            epReflectivityBias = null;
            epTransparency = null;
            epPerPixelLighting = null;

            epBones = null;

            epAmbientLightColor = null;
            epDirLight0Direction = null;
            epDirLight0DiffuseColor = null;
            epDirLight0SpecularColor = null;
            epDirLight0ShadowMap0 = null;
            epDirLight0WorldViewProj1 = null;
            epDirLight0WorldViewProj2 = null;
            epDirLight1Direction = null;
            epDirLight1DiffuseColor = null;
            epDirLight1SpecularColor = null;
            epDirLight2Direction = null;
            epDirLight2DiffuseColor = null;
            epDirLight2SpecularColor = null;

            epPointLightDiffuseColors = null;
            epPointLightSpecularColors = null;
            epPointLightPositions = null;
            epPointLightAttenuations = null;
            epPointLightCount = null;
            epPoint0ShadowMap = null;
            epSSA = null;

            epEyePosition = null;
            epBoundingFrustum = null;
            epCustomClippingPlane = null;

            int pNameHash;

            int hWorld = ("World").GetHashCode();
            int hWIT = ("WorldInverseTranspose").GetHashCode();
            int hWVP = ("WorldViewProj").GetHashCode();
            int hDiff = ("DiffuseMap").GetHashCode();
            int hDiff2 = ("Texture").GetHashCode();
            int hNorm = ("NormalMap").GetHashCode();
            int hCube = ("CubeMap").GetHashCode();
            int hDiffCol = ("DiffuseColor").GetHashCode();
            int hSpecCol = ("SpecularColor").GetHashCode();
            int hGloss = ("Glossiness").GetHashCode();
            int hRefl = ("ReflectivityColor").GetHashCode();
            int hReflB = ("ReflectivityBias").GetHashCode();
            int hTransp = ("Transparency").GetHashCode();
            int hBones = ("Bones").GetHashCode();
            int hAmbLCol = ("AmbientLightColor").GetHashCode();
            int hEmiss = ("EmissiveColor").GetHashCode();
            int l0dir = ("DirLight0Direction").GetHashCode();
            int l0dif = ("DirLight0DiffuseColor").GetHashCode();
            int l0spec = ("DirLight0SpecularColor").GetHashCode();
            int l0sm = ("DirLight0ShadowMap").GetHashCode();
            int l0WVP = ("DirLight0WorldViewProj").GetHashCode();
            int l0VP1 = ("DirLight0WorldViewProj1").GetHashCode();
            int l0VP2 = ("DirLight0WorldViewProj2").GetHashCode();
            int l1dir = ("DirLight1Direction").GetHashCode();
            int l1dif = ("DirLight1DiffuseColor").GetHashCode();
            int l1spec = ("DirLight1SpecularColor").GetHashCode();
            int l2dir = ("DirLight2Direction").GetHashCode();
            int l2dif = ("DirLight2DiffuseColor").GetHashCode();
            int l2spec = ("DirLight2SpecularColor").GetHashCode();
            int pDiffs = ("PointLightDiffuseColors").GetHashCode();
            int pSpecs = ("PointLightSpecularColors").GetHashCode();
            int jebactowszystko = ("ShadowSampleAddition").GetHashCode();
            int pPos = ("PointLightPositions").GetHashCode();
            int pAtts = ("PointLightAttenuations").GetHashCode();
            int pCnt = ("PointLightCount").GetHashCode();
            int p0sm = ("Point0ShadowMap").GetHashCode();
            int p0WVP = ("Point0WorldViewProj").GetHashCode();
            int eyeP = ("EyePosition").GetHashCode();
            int bs = ("BoundingFrustum").GetHashCode();
            int cCP = ("CustomClippingPlane").GetHashCode();

            etUnlit = MyEffect.Techniques["Unlit"];
            etMain = MyEffect.Techniques["Main"];
            etSkinned = MyEffect.Techniques["Skinned"];
            etMainShadows = MyEffect.Techniques["MainShadows"];
            etSkinnedShadows = MyEffect.Techniques["SkinnedShadows"];
            etMainBlurredShadows = MyEffect.Techniques["MainBlurredShadows"];
            etSkinnedBlurredShadows = MyEffect.Techniques["SkinnedBlurredShadows"];

            foreach (EffectParameter p in MyEffect.Parameters)
            {
                pNameHash = p.Name.GetHashCode();
                
                if(pNameHash == hWorld)
                {
                    epWorld = p;
                }
                else if(pNameHash == hWIT)
                {
                    epWorldInverseTranspose = p;
                }
                else if (pNameHash == hWVP)
                {
                    epWorldViewProj = p;
                }
                else if (pNameHash == hDiff)
                {
                    epDiffuseMap = p;
                }
                else if (pNameHash == hDiff2)
                {
                    epDiffuseMap = p;
                }
                else if (pNameHash == hNorm)
                {
                    epNormalMap = p;
                }
                else if (pNameHash == hCube)
                {
                    epCubeMap = p;
                }
                else if (pNameHash == hDiffCol)
                {
                    epDiffuseColor = p;
                }
                else if (pNameHash == hSpecCol)
                {
                    epSpecularColor = p;
                }
                else if (pNameHash == hGloss)
                {
                    epGlossiness = p;
                }
                else if (pNameHash == hRefl)
                {
                    epReflectivityColor = p;
                }
                else if (pNameHash == hReflB)
                {
                    epReflectivityBias = p;
                }
                else if (pNameHash == hTransp)
                {
                    epTransparency = p;
                }
                else if (pNameHash == hBones)
                {
                    epBones = p;
                }
                else if (pNameHash == hAmbLCol)
                {
                    epAmbientLightColor = p;
                }
                else if (pNameHash == hEmiss)
                {
                    epAmbientLightColor = p;
                }
                else if (pNameHash == l0dir)
                {
                    epDirLight0Direction = p;
                }
                else if (pNameHash == l0dif)
                {
                    epDirLight0DiffuseColor = p;
                }
                else if (pNameHash == l0spec)
                {
                    epDirLight0SpecularColor = p;
                }
                else if (pNameHash == l0sm)
                {
                    epDirLight0ShadowMap0 = p;
                }
                else if (pNameHash == l0WVP)
                {
                    epDirLight0WorldViewProj = p;
                }
                else if (pNameHash == l0VP1)
                {
                    epDirLight0WorldViewProj1 = p;
                }
                else if (pNameHash == l0VP2)
                {
                    epDirLight0WorldViewProj2 = p;
                }
                else if (pNameHash == l1dir)
                {
                    epDirLight1Direction = p;
                }
                else if (pNameHash == l1dif)
                {
                    epDirLight1DiffuseColor = p;
                }
                else if (pNameHash == l1spec)
                {
                    epDirLight1SpecularColor = p;
                }
                else if (pNameHash == l2dir)
                {
                    epDirLight2Direction = p;
                }
                else if (pNameHash == l2dif)
                {
                    epDirLight2DiffuseColor = p;
                }
                else if (pNameHash == l2spec)
                {
                    epDirLight2SpecularColor = p;
                }
                else if (pNameHash == pDiffs)
                {
                    epPointLightDiffuseColors = p;
                }
                else if (pNameHash == pSpecs)
                {
                    epPointLightSpecularColors = p;
                }
                else if (pNameHash == pPos)
                {
                    epPointLightPositions = p;
                }
                else if (pNameHash == pAtts)
                {
                    epPointLightAttenuations = p;
                }
                else if (pNameHash == pCnt)
                {
                    epPointLightCount = p;
                }
                else if (pNameHash == p0sm)
                {
                    epPoint0ShadowMap = p;
                }
                else if (pNameHash == jebactowszystko)
                {
                    epSSA = p;
                }
                else if (pNameHash == eyeP)
                {
                    epEyePosition = p;
                }
                else if (pNameHash == bs)
                {
                    epBoundingFrustum = p;
                }
                else if (pNameHash == cCP)
                {
                    epCustomClippingPlane = p;
                }
            }

            if (MyEffect is BasicEffect)
            {
                tempBEref = (BasicEffect)MyEffect;

                tempBEref.LightingEnabled = true;
                tempBEref.DirectionalLight0.Enabled = true;
                tempBEref.DirectionalLight0.DiffuseColor = new Vector3(0.0f, 0.0f, 0.0f);
                tempBEref.DirectionalLight0.SpecularColor = new Vector3(0.0f, 0.0f, 0.0f);
                tempBEref.DirectionalLight0.Direction = new Vector3(0.0f, 1.0f, 0.0f);
                tempBEref.DirectionalLight1.Enabled = true;
                tempBEref.DirectionalLight1.DiffuseColor = new Vector3(0.0f, 0.0f, 0.0f);
                tempBEref.DirectionalLight1.SpecularColor = new Vector3(0.0f, 0.0f, 0.0f);
                tempBEref.DirectionalLight1.Direction = new Vector3(0.0f, 1.0f, 0.0f);
                tempBEref.DirectionalLight2.Enabled = true;
                tempBEref.DirectionalLight2.DiffuseColor = new Vector3(0.0f, 0.0f, 0.0f);
                tempBEref.DirectionalLight2.SpecularColor = new Vector3(0.0f, 0.0f, 0.0f);
                tempBEref.DirectionalLight2.Direction = new Vector3(0.0f, 1.0f, 0.0f);
                tempBEref.TextureEnabled = true;
            }
            else if (MyEffect is SkinnedEffect)
            {
                tempSEref = (SkinnedEffect)MyEffect;

                tempSEref.EnableDefaultLighting();
                tempSEref.DirectionalLight0.Enabled = true;
                tempSEref.DirectionalLight1.Enabled = true;
                tempSEref.DirectionalLight2.Enabled = true;
                tempSEref.DirectionalLight0.DiffuseColor = new Vector3(0.0f, 0.0f, 0.0f);
                tempSEref.DirectionalLight0.SpecularColor = new Vector3(0.0f, 0.0f, 0.0f);
                tempSEref.DirectionalLight0.Direction = new Vector3(0.0f, 1.0f, 0.0f);
                tempSEref.DirectionalLight1.DiffuseColor = new Vector3(0.0f, 0.0f, 0.0f);
                tempSEref.DirectionalLight1.SpecularColor = new Vector3(0.0f, 0.0f, 0.0f);
                tempSEref.DirectionalLight1.Direction = new Vector3(0.0f, 1.0f, 0.0f);
                tempSEref.DirectionalLight2.DiffuseColor = new Vector3(0.0f, 0.0f, 0.0f);
                tempSEref.DirectionalLight2.SpecularColor = new Vector3(0.0f, 0.0f, 0.0f);
                tempSEref.DirectionalLight2.Direction = new Vector3(0.0f, 1.0f, 0.0f);
            }
        }

        public System.Xml.Schema.XmlSchema GetSchema() { return null; }
        public void ReadXml(System.Xml.XmlReader reader)
        {
            reader.MoveToContent();
            reader.ReadStartElement();

            Name = reader.ReadElementString("Name", "");
            MyEffect = ResourceManager.Instance.LoadEffect(reader.ReadElementString("EffectPath", ""));

            uint normColor = 0xFFFF0F0F;
            uint blackColor = 0xFF000000;
            Texture2D defDiff = null;
            if (!ResourceManager.Instance.Textures.TryGetValue("DefaultDiffuse", out defDiff))
            {
                defDiff = new Texture2D(TrashSoupGame.Instance.GraphicsDevice, 1, 1, false, SurfaceFormat.Color);
                defDiff.SetData<uint>(new uint[] { blackColor });
                ResourceManager.Instance.Textures.Add("DefaultDiffuse", defDiff);
            }
            Texture2D defNrm = null;
            if (!ResourceManager.Instance.Textures.TryGetValue("DefaultNormal", out defNrm))
            {
                defNrm = new Texture2D(TrashSoupGame.Instance.GraphicsDevice, 1, 1, false, SurfaceFormat.Color);
                defNrm.SetData<uint>(new uint[] { normColor });
                ResourceManager.Instance.Textures.Add("DefaultNormal", defNrm);
            }
            TextureCube defCbc = null;
            if (!ResourceManager.Instance.TexturesCube.TryGetValue("DefaultCube", out defCbc))
            {
                defCbc = new TextureCube(TrashSoupGame.Instance.GraphicsDevice, 1, false, SurfaceFormat.Color);
                defCbc.SetData<uint>(CubeMapFace.NegativeX, new uint[] { blackColor });
                defCbc.SetData<uint>(CubeMapFace.PositiveX, new uint[] { blackColor });
                defCbc.SetData<uint>(CubeMapFace.NegativeY, new uint[] { blackColor });
                defCbc.SetData<uint>(CubeMapFace.PositiveY, new uint[] { blackColor });
                defCbc.SetData<uint>(CubeMapFace.NegativeZ, new uint[] { blackColor });
                defCbc.SetData<uint>(CubeMapFace.PositiveZ, new uint[] { blackColor });
                ResourceManager.Instance.TexturesCube.Add("DefaultCube", defCbc);
            }

            DiffuseMap = ResourceManager.Instance.LoadTexture(reader.ReadElementString("DiffusePath", ""));
            if(reader.Name == "NormalPath")
            {
                NormalMap = ResourceManager.Instance.LoadTexture(reader.ReadElementString("NormalPath", ""));
            }
            if(reader.Name == "CubePath")
            {
                CubeMap = ResourceManager.Instance.LoadTextureCube(reader.ReadElementString("CubePath", ""));
            }

            reader.ReadStartElement("DiffuseColor");
            DiffuseColor = new Vector3(reader.ReadElementContentAsFloat("X", ""),
                                           reader.ReadElementContentAsFloat("Y", ""),
                                           reader.ReadElementContentAsFloat("Z", ""));
            reader.ReadEndElement();

            reader.ReadStartElement("SpecularColor");
            SpecularColor = new Vector3(reader.ReadElementContentAsFloat("X", ""),
                                           reader.ReadElementContentAsFloat("Y", ""),
                                           reader.ReadElementContentAsFloat("Z", ""));
            reader.ReadEndElement();

            Glossiness = reader.ReadElementContentAsFloat("Glossiness", "");

            reader.ReadStartElement("ReflectivityColor");
            ReflectivityColor = new Vector3(reader.ReadElementContentAsFloat("X", ""),
                                           reader.ReadElementContentAsFloat("Y", ""),
                                           reader.ReadElementContentAsFloat("Z", ""));
            reader.ReadEndElement();

            ReflectivityBias = reader.ReadElementContentAsFloat("ReflectivityBias", "");
            Transparency = reader.ReadElementContentAsFloat("Transparency", "");
            PerPixelLighting = reader.ReadElementContentAsBoolean("PerPixelLighting", "");
            RecieveShadows = reader.ReadElementContentAsBoolean("ReceiveShadows", "");
            Unlit = reader.ReadElementContentAsBoolean("Unlit", "");

            AssignParamsInitialize();

            if(this.GetType() == typeof(Material))
                reader.ReadEndElement();
        }

        public void WriteXml(System.Xml.XmlWriter writer)
        {
            writer.WriteElementString("Name", Name);
            writer.WriteElementString("EffectPath", ResourceManager.Instance.Effects.FirstOrDefault(x => x.Value == MyEffect).Key);

            writer.WriteElementString("DiffusePath", ResourceManager.Instance.Textures.FirstOrDefault(x => x.Value == DiffuseMap).Key);
            writer.WriteElementString("NormalPath", ResourceManager.Instance.Textures.FirstOrDefault(x => x.Value == NormalMap).Key);
            if(CubeMap != null)
            {
                writer.WriteElementString("CubePath", ResourceManager.Instance.TexturesCube.FirstOrDefault(x => x.Value == CubeMap).Key);   
            }

            writer.WriteStartElement("DiffuseColor");
            writer.WriteElementString("X", XmlConvert.ToString(DiffuseColor.X));
            writer.WriteElementString("Y", XmlConvert.ToString(DiffuseColor.Y));
            writer.WriteElementString("Z", XmlConvert.ToString(DiffuseColor.Z));
            writer.WriteEndElement();

            writer.WriteStartElement("SpecularColor");
            writer.WriteElementString("X", XmlConvert.ToString(SpecularColor.X));
            writer.WriteElementString("Y", XmlConvert.ToString(SpecularColor.Y));
            writer.WriteElementString("Z", XmlConvert.ToString(SpecularColor.Z));
            writer.WriteEndElement();

            writer.WriteElementString("Glossiness", XmlConvert.ToString(Glossiness));

            writer.WriteStartElement("ReflectivityColor");
            writer.WriteElementString("X", XmlConvert.ToString(ReflectivityColor.X));
            writer.WriteElementString("Y", XmlConvert.ToString(ReflectivityColor.Y));
            writer.WriteElementString("Z", XmlConvert.ToString(ReflectivityColor.Z));
            writer.WriteEndElement();

            writer.WriteElementString("ReflectivityBias", XmlConvert.ToString(ReflectivityBias));
            writer.WriteElementString("Transparency", XmlConvert.ToString(Transparency));
            writer.WriteElementString("PerPixelLighting", XmlConvert.ToString(PerPixelLighting));
            writer.WriteElementString("ReceiveShadows", XmlConvert.ToString(RecieveShadows));
            writer.WriteElementString("Unlit", XmlConvert.ToString(Unlit));
        }
        #endregion
    }
}
