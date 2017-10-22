using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Graphics;
using TrashSoup;
using TrashSoup.Gameplay;
using System.Xml.Serialization;
using System.IO;

namespace TrashSoup.Engine
{
    class SingleRandom : Singleton<SingleRandom>
    {
        public Random rnd = new Random();
    }

    public class ResourceManager : Singleton<ResourceManager>
    {
        #region Constants

        public const int DIRECTIONAL_MAX_LIGHTS = 3;
        public const int POINT_MAX_LIGHTS_PER_OBJECT = 10; 

        #endregion

        #region Variables
        public Scene CurrentScene;
        public Dictionary<string, Model> Models = new Dictionary<string, Model>();
        public Dictionary<string, Model> Animations = new Dictionary<string, Model>();
        public Dictionary<string, Texture2D> Textures = new Dictionary<string, Texture2D>();
        public Dictionary<string, TextureCube> TexturesCube = new Dictionary<string, TextureCube>();
        public Dictionary<string, SpriteFont> Fonts = new Dictionary<string, SpriteFont>();
        public Dictionary<string, Effect> Effects = new Dictionary<string, Effect>();
        public Dictionary<string, Material> Materials = new Dictionary<string,Material>();
        public List<Cue> Sounds = new List<Cue>();
        public Dictionary<string, PostEffect> PostEffects = new Dictionary<string, PostEffect>();

        public bool ImmediateStop = false;
        #endregion

        #region Methods

        public ResourceManager()
        {

        }

        public void LoadContent(Game game)
        {
            LoadTextures();
            LoadEffects();
            AudioManager.Instance.LoadContent();
            TrashSoupGame.Instance.ReloadSpriteBatch();
            //LoadCues();

            // because it pisses me off - Mav
            //AudioManager.Instance.PlayCue(GetCueFromCueList("Track1")); //default music from tutorial, just to check that everything works xD

            // FOR TETIN
            AddModel("Models/Test/TestBox");
            AddModel("Models/Test/TestCube");
            AddModel("Models/Test/TestTerrain");
            AddModel("Models/Test/TestGuy");
            AddModel("Models/Test/TestSphere");
            AddModel("Models/Test/TestSphere_LOD1");
            AddModel("Models/Test/TestSphere_LOD2");
            AddModel("Models/Test/TestMirror");
            AddModel("Models/Weapons/Signs/StopSign");
            AddModel("Models/Weapons/Stones/brick");
            AddModel("Models/Enviro/Ground/street_cross");
            AddModel("Models/Weapons/Stones/brick_destructible/4");
            AddModel("Models/Test/TestSquarePlane");
            AddModel("Models/Test/TestSquarePlane");
            AddModel("Models/MainCharacter/MainCharacter");
            
            AddAnimation("Animations/MainCharacter/attack_Hweapon_1");
            AddAnimation("Animations/MainCharacter/attack_Hweapon_2");
            AddAnimation("Animations/MainCharacter/attack_Hweapon_3");
            AddAnimation("Animations/MainCharacter/attack_Hweapon_4");
            AddAnimation("Animations/MainCharacter/attack_Mweapon_1");
            AddAnimation("Animations/MainCharacter/attack_Mweapon_2");
            AddAnimation("Animations/MainCharacter/attack_Mweapon_3");
            AddAnimation("Animations/MainCharacter/attack_Mweapon_4");
            AddAnimation("Animations/MainCharacter/attack_Mweapon_5");
            AddAnimation("Animations/MainCharacter/block_1");
            AddAnimation("Animations/MainCharacter/bow_1");
            AddAnimation("Animations/MainCharacter/boxing_1");
            AddAnimation("Animations/MainCharacter/boxing_2");
            AddAnimation("Animations/MainCharacter/boxing_3");
            AddAnimation("Animations/MainCharacter/boxing_4");
            AddAnimation("Animations/MainCharacter/charge_Hweapon");
           // AddAnimation("Animations/MainCharacter/charge_SMweapon");
          //  AddAnimation("Animations/MainCharacter/custom_kick");
           // AddAnimation("Animations/MainCharacter/custom_slash");
          //  AddAnimation("Animations/MainCharacter/custom_whirl");
            AddAnimation("Animations/MainCharacter/dodge_1");
            AddAnimation("Animations/MainCharacter/dying_1");
            AddAnimation("Animations/MainCharacter/idle_1");
            AddAnimation("Animations/MainCharacter/idle_2");
            AddAnimation("Animations/MainCharacter/idle_Fists");
            AddAnimation("Animations/MainCharacter/idle_Hweapon");
            AddAnimation("Animations/MainCharacter/idle_SMweapon");
            AddAnimation("Animations/MainCharacter/injuries_1");
            AddAnimation("Animations/MainCharacter/injuries_2");
            AddAnimation("Animations/MainCharacter/jump");
            AddAnimation("Animations/MainCharacter/run_2");
            AddAnimation("Animations/MainCharacter/dying_1");
            AddAnimation("Animations/MainCharacter/run_Hweapon");
           // AddAnimation("Animations/MainCharacter/run_SMweapon");
            AddAnimation("Animations/MainCharacter/walk_1");
            AddAnimation("Animations/MainCharacter/walk_Hweapon");
            AddAnimation("Animations/MainCharacter/walki_SMweapon");
            AddAnimation("Animations/Enemies/Rat_attack");
            AddAnimation("Animations/Enemies/Rat_dying");
            AddAnimation("Animations/Enemies/Rat_idle");
            AddAnimation("Animations/Enemies/Rat_run");
            AddAnimation("Animations/Enemies/Rat_walk");
            AddAnimation("Animations/Test/walking_1");
            AddAnimation("Animations/Test/idle_1");
            AddAnimation("Animations/Test/jump_1");
            AddModel("Models/Enviro/Railing/Railing_1");    //Wika i Kasia

            // loading materials
            List<Material> testPlayerMats = new List<Material>();
            Material testPlayerMat = new Material("testPlayerMat", LoadEffect(@"Effects\NormalEffect"), LoadTexture(@"Textures\Test\cargo"));
            testPlayerMats.Add(testPlayerMat);
            testPlayerMat.NormalMap = LoadTexture(@"Textures\Test\cargo_NRM");
            testPlayerMat.Glossiness = 40.0f;
            testPlayerMat.Transparency = 1.0f;
            testPlayerMat.RecieveShadows = true;
            if(!this.Materials.ContainsKey(testPlayerMat.Name))
            {
                this.Materials.Add(testPlayerMat.Name, testPlayerMat);
            }

            List<Material> testPlayerMats2 = new List<Material>();
            Material testPlayerMat2 = new Material("testPlayerMat2", this.Effects[@"Effects\CubeNormalEffect"], LoadTexture(@"Textures\Test\cargo"));
            testPlayerMats2.Add(testPlayerMat2);
            testPlayerMat2.NormalMap = LoadTexture(@"Textures\Test\cargo_NRM");
            testPlayerMat2.CubeMap = LoadTextureCube(@"Textures\Skyboxes\Dusk");
            testPlayerMat2.Glossiness = 40.0f;
            testPlayerMat2.ReflectivityColor = new Vector3(1.0f, 0.0f, 1.0f);
            testPlayerMat2.ReflectivityBias = 0.7f;
            testPlayerMat2.Transparency = 0.25f;
            testPlayerMat2.RecieveShadows = true;
            if(!this.Materials.ContainsKey(testPlayerMat2.Name))
            {
                this.Materials.Add(testPlayerMat2.Name, testPlayerMat2);
            }

            List<Material> testMirrorMats = new List<Material>();
            Material testMirrorMat2 = new MirrorMaterial("testMirrorMat2", this.Effects[@"Effects\NormalEffect"]);
            testMirrorMats.Add(testMirrorMat2);
            testMirrorMat2.DiffuseMap = LoadTexture(@"Textures\Home\Furnitures\mirror_D");
            testMirrorMat2.NormalMap = LoadTexture(@"Textures\Home\Furnitures\mirror_N");
            testMirrorMat2.Glossiness = 100.0f;
            testMirrorMat2.ReflectivityBias = 0.0f;
            if (!this.Materials.ContainsKey(testMirrorMat2.Name))
            {
                this.Materials.Add(testMirrorMat2.Name, testMirrorMat2);
            }
            Material testMirrorMat = new MirrorMaterial("testMirrorMat", this.Effects[@"Effects\MirrorEffect"]);
            testMirrorMats.Add(testMirrorMat);
            testMirrorMat.DiffuseMap = LoadTexture(@"Textures\Home\Furnitures\mirror_glass");
            testMirrorMat.Glossiness = 100.0f;
            testMirrorMat.ReflectivityBias = 0.0f;
            if (!this.Materials.ContainsKey(testMirrorMat.Name))
            {
                this.Materials.Add(testMirrorMat.Name, testMirrorMat);
            }

            List<Material> testWaterMats = new List<Material>();
            Material testWaterMat = new WaterMaterial("testWaterMat", this.Effects[@"Effects\WaterEffect"]);
            testWaterMats.Add(testWaterMat);
            testWaterMat.DiffuseMap = LoadTexture(@"Textures\Test\dirtywater");
            testWaterMat.NormalMap = LoadTexture(@"Textures\Test\water");
            testWaterMat.Glossiness = 200.0f;
            testWaterMat.ReflectivityBias = 0.6f;
            if(!this.Materials.ContainsKey(testWaterMat.Name))
            {
                this.Materials.Add(testWaterMat.Name, testWaterMat);
            }

            //List<Material> playerMats = LoadBasicMaterialsFromModel(Models["Models/MainCharacter/MainCharacter"], this.Effects[@"Effects\NormalEffect"]);
            //foreach(Material mat in playerMats)
            //{
            //    mat.RecieveShadows = true;
            //}

            //List<Material> ratMats = LoadBasicMaterialsFromModel(Models["Models/MainCharacter/MainCharacter"], this.Effects[@"Effects\NormalEffect"]);
            List<Material> ratMats = new List<Material>();
            Material ratMat01 = new Material("PlayerMat", this.Effects[@"Effects\NormalEffect"]);
            ratMat01.DiffuseMap = LoadTexture(@"Models\Enemies\Mutant_body_D");
            ratMat01.NormalMap = LoadTexture(@"Models\Enemies\Mutant_body_N");
            ratMats.Add(ratMat01);
            Material ratMat02 = new Material("PlayerMatHair", this.Effects[@"Effects\NormalEffect"]);
            ratMat02.DiffuseMap = LoadTexture(@"Models\Enemies\Mutant_head_D");
            ratMat02.NormalMap = LoadTexture(@"Models\Enemies\Mutant_head_N");
            ratMats.Add(ratMat02);

            List<Material> deSign = LoadBasicMaterialsFromModel(Models["Models/Enviro/Ground/street_cross"], this.Effects[@"Effects\NormalEffect"]);

            List<Material> bb = LoadBasicMaterialsFromModel(Models["Models/Weapons/Stones/brick"], Effects[@"Effects\NormalEffect"]);

            List<Material> testTerMats = new List<Material>();
            Material testTerMat = new Material("testTerMat", this.Effects[@"Effects\NormalEffect"], LoadTexture(@"Textures\Test\metal01_d"));
            testTerMat.NormalMap = LoadTexture(@"Textures\Test\water");
            testTerMat.SpecularColor = new Vector3(0.1f, 0.1f, 0.0f);
            testTerMat.Glossiness = 10.0f;
            testTerMat.RecieveShadows = true;
            testTerMat.Unlit = true;
            if(!this.Materials.ContainsKey(testTerMat.Name))
            {
                this.Materials.Add(testTerMat.Name, testTerMat);
            }
            if(!testTerMats.Contains(testTerMat))
            {
                testTerMats.Add(testTerMat);
            }

            List<Material> testSBMats = new List<Material>();
            SkyboxMaterial testSBMat = new SkyboxMaterial("testSBMat", this.Effects[@"Effects\SkyboxEffect"]);
            testSBMat.CubeMap = LoadTextureCube(@"Textures\Skyboxes\Dawn");
            testSBMat.CubeMap1 = LoadTextureCube(@"Textures\Skyboxes\Daylight");
            testSBMat.CubeMap2 = LoadTextureCube(@"Textures\Skyboxes\Dusk");
            testSBMat.CubeMap3 = LoadTextureCube(@"Textures\Skyboxes\Night");
            testSBMat.Probes = new Vector4(0.0f, 0.0f, 0.0f, 1.0f);
            testSBMat.SpecularColor = new Vector3(0.0f, 0.0f, 0.0f);
            testSBMat.Glossiness = 100.0f;
            if(!this.Materials.ContainsKey(testSBMat.Name))
            {
                this.Materials.Add(testSBMat.Name, testSBMat);
            }
            if(!testSBMats.Contains(testSBMat))
            {
                testSBMats.Add(testSBMat);
            }


            //WIKA I KASIA Testowanie modeli
            List<Material> awsomeTestMats = new List<Material>();
            Material awsomeTestMat = new Material("awsomeTestMat", this.Effects[@"Effects\NormalEffect"], LoadTexture(@"Textures\Enviro\Railing\Railing_D"));
            awsomeTestMats.Add(awsomeTestMat);
            awsomeTestMats.Add(awsomeTestMat);
            awsomeTestMat.NormalMap = LoadTexture(@"Textures\Enviro\Railing\Railing_N");
            awsomeTestMat.Glossiness = 40.0f;
            awsomeTestMat.ReflectivityColor = new Vector3(1.0f, 0.0f, 1.0f);
            awsomeTestMat.ReflectivityBias = 0.0f;
            awsomeTestMat.Transparency = 1.0f;
            awsomeTestMat.RecieveShadows = true;
            if (!this.Materials.ContainsKey(awsomeTestMat.Name))
            {
                this.Materials.Add(awsomeTestMat.Name, awsomeTestMat);
            }


            // loading gameobjects
            GameObject testBox = new GameObject(1, "Player");
            testBox.MyTransform = new Transform(testBox, new Vector3(2.0f, 3.0f, -5.0f), new Vector3(0.0f, 0.0f, 1.0f), new Vector3(0.0f, 0.0f, 0.0f), 0.0075f);
            CustomModel skModel = new CustomModel(testBox, new Model[] { Models["Models/MainCharacter/MainCharacter"], null, null }, ratMats);
            Animator playerAnimator = new Animator(testBox, skModel.LODs[0]);
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/run_2"), "Animations/MainCharacter/run_2"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/idle_Hweapon"), "Animations/MainCharacter/idle_Hweapon"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/idle_Fists"), "Animations/MainCharacter/idle_Fists"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/idle_SMweapon"), "Animations/MainCharacter/idle_SMweapon"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/dodge_1"), "Animations/MainCharacter/dodge_1"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/boxing_4"), "Animations/MainCharacter/boxing_4"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/boxing_1"), "Animations/MainCharacter/boxing_1"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/boxing_2"), "Animations/MainCharacter/boxing_2"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/boxing_3"), "Animations/MainCharacter/boxing_3"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/attack_Hweapon_1"), "Animations/MainCharacter/attack_Hweapon_1"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/attack_Hweapon_2"), "Animations/MainCharacter/attack_Hweapon_2"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/attack_Hweapon_3"), "Animations/MainCharacter/attack_Hweapon_3"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/attack_Hweapon_4"), "Animations/MainCharacter/attack_Hweapon_4"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/attack_Mweapon_1"), "Animations/MainCharacter/attack_Mweapon_1"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/attack_Mweapon_2"), "Animations/MainCharacter/attack_Mweapon_2"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/attack_Mweapon_3"), "Animations/MainCharacter/attack_Mweapon_3"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/attack_Mweapon_4"), "Animations/MainCharacter/attack_Mweapon_4"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/attack_Mweapon_5"), "Animations/MainCharacter/attack_Mweapon_5"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/building"), "Animations/MainCharacter/building"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/dying_1"), "Animations/MainCharacter/dying_1"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/block_1"), "Animations/MainCharacter/block_1"));
            playerAnimator.AddAnimationClip(LoadAnimationFromModel(skModel.LODs[0], LoadAnimation("Animations/MainCharacter/grzebanie"), "Animations/MainCharacter/grzebanie"));
            testBox.Components.Add(skModel);
            testBox.MyAnimator = playerAnimator;
            PlayerController pc = new PlayerController(testBox);
            testBox.Components.Add(pc);
            testBox.Dynamic = true;

            HideoutStash testStash = new HideoutStash(testBox);
            testStash.CurrentTrash = 1000;
            testBox.Components.Add(testStash);

            Equipment eq = new Equipment(testBox);
            eq.JunkCount = 10;
            testBox.Components.Add(eq);

            testBox.MyPhysicalObject = new PhysicalObject(testBox, 1.0f, 0.05f, false);
            testBox.MyCollider = new BoxCollider(testBox);  //Add a box collider to test collisions

            // loading gameobjects
            GameObject rat = new GameObject(50, "Rat");
            rat.MyTransform = new Transform(rat, new Vector3(0.0f, 0.0f, 10.0f), new Vector3(0.0f, 0.0f, 1.0f), new Vector3(0.0f, 0.0f, 0.0f), 0.01f);
            CustomModel ratModel = new CustomModel(rat, new Model[] { LoadModel("Models/Enemies/Mutant01"), null, null }, ratMats);
            rat.Components.Add(ratModel);
            Animator ratAnimator = new Animator(rat, ratModel.LODs[0]);
            ratAnimator.AddAnimationClip(LoadAnimationFromModel(ratModel.LODs[0], LoadAnimation("Animations/Enemies/Mutant_attack"), "Rat_TAnim"));
            ratAnimator.AvailableStates.Add("Walk", new AnimatorState("Walk", ratAnimator.GetAnimationPlayer("Rat_TAnim"), AnimatorState.StateType.SINGLE));
            ratAnimator.CurrentState = ratAnimator.AvailableStates["Walk"];
            rat.MyAnimator = ratAnimator;
            rat.Components.Add(new Enemy(rat));
            rat.MyCollider = new SphereCollider(rat);
            rat.MyPhysicalObject = new PhysicalObject(rat);

            rat.Components.Add(new Food(rat));

            GameObject testTer = new GameObject(2, "Terrain");
            testTer.MyTransform = new Transform(testTer, new Vector3(0.0f, 0.0f, 0.0f), new Vector3(0.0f, 0.0f, 1.0f), new Vector3(0.0f, 0.0f, 0.0f), 1.0f);
            CustomModel terModel = new CustomModel(testTer, new Model[] { Models["Models/Test/TestTerrain"], null, null }, testTerMats);
            terModel.LodControlled = false;
            testTer.Components.Add(terModel);

            GameObject testBox2 = new GameObject(3, "StreettestBox2");
            testBox2.MyTransform = new Transform(testBox2, new Vector3(0.0f, -0.1f, 112.0f), new Vector3(0.0f, 0.0f, 1.0f), new Vector3(0.0f, 0.0f, 0.0f), 10.0f);
            testBox2.Components.Add(new CustomModel(testBox2, new Model[] { LoadModel("Models/Enviro/Ground/street_straight"), null, null }, testPlayerMats));
            //Billboard billboard = new Billboard(testBox2);
            //Material bbmat = new Material("billboard", Effects[@"Effects\BillboardEffect"], LoadTexture(@"Textures\Enviro\Nature\Sun"));
            //billboard.Mat = bbmat;
            //billboard.Size = new Vector2(1.0f, 1.0f);
            //testBox2.Components.Add(billboard);
            testBox2.MyCollider = new BoxCollider(testBox2, false);
            testBox2.MyPhysicalObject = new PhysicalObject(testBox2, 1.0f, 1.0f, false);
            //testBox2.Dynamic = true;
            //testBox2.MyCarrierSocket = new Socket(testBox, testBox2, null, "mixamorig:RightHand");

            GameObject testBox3 = new GameObject(5, "testBox3");
            testBox3.MyTransform = new Transform(testBox3, new Vector3(0.0f, 0.0f, 0.0f), new Vector3(0.0f, 0.0f, 1.0f), new Vector3(0.0f, 0.0f, 0.0f), 0.7f);
            testBox3.Components.Add(new CustomModel(testBox3, new Model[] { LoadModel("Models/Weapons/Maczuga/Mace"), null, null }, testPlayerMats));
            testBox3.MyCollider = new BoxCollider(testBox3, true);
            testBox3.Dynamic = true;
            testBox3.Components.Add(new Gameplay.Weapons.Hammer(testBox3));

            GameObject fuckYou = new GameObject(1144, "Fists");
            fuckYou.MyTransform = new Transform(fuckYou, Vector3.Zero, Vector3.Up, new Vector3(0.0f, 0.0f, 11.017704f), 1.0f);
            fuckYou.MyCollider = new BoxCollider(fuckYou, true);
            fuckYou.Components.Add(new Gameplay.Weapons.Fists(fuckYou));

            pc.Equipment.PickUpWeapon(testBox3);

            //ParticleSystem ps = new ParticleSystem(testBox3);
            //ps.Textures.Add(ResourceManager.Instance.LoadTexture("Textures/Particles/Particle_metal01"));
            //ps.Textures.Add(ResourceManager.Instance.LoadTexture("Textures/Particles/Particle_metal02"));
            //ps.Textures.Add(ResourceManager.Instance.LoadTexture("Textures/Particles/Particle_wood01"));
            //ps.Textures.Add(ResourceManager.Instance.LoadTexture("Textures/Particles/Particle_wood02"));
            //ps.Textures.Add(ResourceManager.Instance.LoadTexture("Textures/Particles/Particle_wood03"));
            //ps.ParticleCount = 170;
            //ps.ParticleSize = new Vector2(0.5f, 0.5f);
            //ps.LifespanSec = 5.0f;
            //ps.Wind = new Vector3(0.0f, 0.1f, 0.0f);
            //ps.FadeInTime = 0.0f;
            //ps.Offset = new Vector3(MathHelper.PiOver2);
            //ps.Speed = 10.0f;
            ////ps.DelayMs = 5000.0f;
            //ps.LoopMode = ParticleSystem.ParticleLoopMode.CONTINUOUS;
            //ps.PositionOffset = new Vector3(0.0f, 0.0f, 0.0f) * testBox3.MyTransform.Scale;
            //testBox3.Components.Add(ps);

            GameObject testMirror = new GameObject(6, "testMirror");
            testMirror.MyTransform = new Transform(testMirror, new Vector3(-10.0f, 2.0f, -10.0f), new Vector3(0.0f, 0.0f, 1.0f), new Vector3(0.0f, -MathHelper.PiOver2, 0.0f), 0.5f);
            testMirror.Components.Add(new CustomModel(testMirror, new Model[] { LoadModel("Models/Home/Furnitures/mirror"), null, null }, testMirrorMats));
            testMirror.MyCollider = new BoxCollider(testMirror, false);

            GameObject testWater = new GameObject(7, "tesWtater");
            testWater.MyTransform = new Transform(testWater, new Vector3(0.0f, -1.5f, 0.0f), new Vector3(0.0f, 1.0f, 0.0f), new Vector3(0.0f, 0.0f, 0.0f), 10.0f);
            testWater.Components.Add(new CustomModel(testWater, new Model[] { Models["Models/Test/TestSquarePlane"], null, null }, testWaterMats));

            GameObject skyBox = new GameObject(4, "skyBox");
            skyBox.MyTransform = new Transform(skyBox, new Vector3(0.0f, 0.0f, 0.0f), new Vector3(0.0f, 1.0f, 0.0f), new Vector3(0.0f, 0.0f, 0.0f), 2000.0f);
            CustomModel sbModel = new CustomModel(skyBox, new Model[] { Models["Models/Test/TestCube"], null, null }, testSBMats);
            sbModel.LodControlled = false;
            skyBox.Components.Add(sbModel);
            //DaytimeChange dc = new DaytimeChange(skyBox);
            //dc.LightDayID = 0;
            //dc.LightNightID = 1;
            //dc.SunID = 3;
            //dc.SunriseMinutes = 60 * 6;
            //dc.SunsetMinutes = 60 * 20;
            //dc.StateChangeMinutes = 120;
            //dc.HorizonOffset = 500.0f;
            //dc.TextureNames = new string[] { @"Textures\Skyboxes\Dawn", @"Textures\Skyboxes\Daylight", @"Textures\Skyboxes\Dusk", @"Textures\Skyboxes\Night" };
            //skyBox.Components.Add(dc);

            // moje na pagi
            GameObject cegla = new GameObject(14, "cegla");
            cegla.MyTransform = new Transform(cegla, new Vector3(5.0f, 0.0f, 0.0f), new Vector3(0.0f, 0.0f, 1.0f), new Vector3(0.0f, 0.0f, 0.0f), 1.0f);
            //cegla.Components.Add(new CustomModel(cegla, new Model[] { Models["Models/Weapons/Stones/brick"], null, null }, bb));
            //cegla.MyCollider = new BoxCollider(cegla);
            Fortification fort = new Fortification(cegla);
            fort.CurrentID = 0;
            fort.MyType = Fortification.FortificationType.WOOD1_WIRE_SNARES;
            cegla.Components.Add(fort);
            //GameObject pt = new GameObject(355, "PlayerTime");
            //PlayerTime ptc = new PlayerTime(pt);
            //ptc.Multiplier = 3600;
            //pt.Components.Add(ptc);

            //Wika i Kasia testowanie modeli
            GameObject awsomeTest = new GameObject(8, "testground");
            awsomeTest.MyTransform = new Transform(awsomeTest, new Vector3(-12.0f, 1.0f, -5.0f), new Vector3(0.0f, 0.0f, 1.0f), new Vector3(MathHelper.PiOver2, 0.0f, 0.0f), 1.0f);
            awsomeTest.Components.Add(new CustomModel(awsomeTest, new Model[] { Models["Models/Enviro/Railing/Railing_1"], null, null }, awsomeTestMats));
            awsomeTest.MyCollider = new BoxCollider(awsomeTest, false);

            GameObject playerTime = new GameObject(1000, "PlayerTime");
            PlayerTime pt = new PlayerTime(playerTime);
            pt.InitHours = 20;
            playerTime.Components.Add(pt);

            GameObject testTransition = new GameObject(501, "Transition");
            AreaTransition at = new AreaTransition(testTransition);
            at.NextScenePath = "../../../../TrashSoupContent/Scenes/save2.xml";
            testTransition.Components.Add(at);
            testTransition.MyTransform = new Transform(testTransition, new Vector3(-10.0f, 0.0f, 10.0f), Vector3.Up, Vector3.Zero, 5.0f);
            testTransition.MyCollider = new SphereCollider(testTransition, true);

            // adding lights
            LightAmbient amb = new LightAmbient(100, "LightAmbient", new Vector3(0.1f, 0.1f, 0.1f));
            LightDirectional ldr = new LightDirectional(101, "LightDirectional1", new Vector3(1.0f, 0.9f, 0.8f), new Vector3(1.0f, 0.8f, 0.8f), new Vector3(-1.0f, -1.0f, -1.0f), true);
            LightDirectional ldrn = new LightDirectional(102, "LightDirectional2", new Vector3(0.1f, 0.1f, 0.15f), new Vector3(0.0f, 0.1f, 0.2f), new Vector3(1.0f, 1.0f, 1.0f), true);
            //LightPoint lp1 = new LightPoint(110, "LightPoint1", new Vector3(0.0f, 1.0f, 1.0f), new Vector3(1.0f, 1.0f, 1.0f), 1.0f, false);
            //lp1.MyTransform = new Transform(lp1, new Vector3(0.0f, 1.0f, 0.0f), new Vector3(0.0f, 1.0f, 0.0f), new Vector3(0.0f, 0.0f, 0.0f), 10.0f);
            //lp1.MyCollider = new SphereCollider(lp1, true);
            //lp1.MyPhysicalObject = new PhysicalObject(lp1, 0.0f, 0.0f, false);
            //lp1.SetupShadowRender();


            // loading scene
            CurrentScene = new Scene(new SceneParams(0, "test", new Vector2(0.0f, 0.1f), new DateTime(2015, 5, 28, 12, 0, 0, 0, new System.Globalization.GregorianCalendar(), DateTimeKind.Unspecified),
                60.0f, 3000.0f, 35.0f, 50.0f, true, false, true, false, true));

            Camera cam = null;

            if(TrashSoupGame.Instance.EditorMode)
            {
                //Editor camera
                CurrentScene.EditorCam = new EditorCamera(1, "editorCam", Vector3.Transform(new Vector3(0.0f, 10.0f, -50.0f), Matrix.CreateRotationX(MathHelper.PiOver4 * 1.5f)),
                     new Vector3(0.0f, 5.0f, 5.0f), new Vector3(0.0f, 10.0f, 0.0f), new Vector3(0.0f, 1.0f, 0.0f), MathHelper.Pi / 3.0f, 0.1f, 2000.0f);
            }
                //Game camera
            cam = new Camera(1, "playerCam", Vector3.Transform(new Vector3(0.0f, 1.0f, -0.1f), Matrix.CreateRotationX(MathHelper.PiOver4 * 1.5f)) + new Vector3(0.0f, 0.4f, 0.0f),
                    new Vector3(0.0f, 0.0f, 0.0f), new Vector3(0.0f, 1.5f, 0.0f), new Vector3(0.0f, 1.0f, 0.0f), MathHelper.Pi / 3.0f,
                    (float)TrashSoupGame.Instance.Window.ClientBounds.Width / (float)TrashSoupGame.Instance.Window.ClientBounds.Height, 0.1f, 4000.0f);
            cam.Components.Add(new CameraBehaviourComponent(cam, testBox));

            CurrentScene.Cam = cam;

            //testTer.LightsAffecting.Add(lp1);

            // adding items to scene
            //testBox.AddChild(testBox3);
            CurrentScene.AddObject(skyBox);
            CurrentScene.AddObject(testTer);
            CurrentScene.AddObject(testBox);
            CurrentScene.AddObject(testBox2);
            
            CurrentScene.AddObject(testMirror);
            CurrentScene.AddObject(testWater);
            CurrentScene.AddObject(testBox3);
            CurrentScene.AddObject(awsomeTest);//Wika i kasia
            CurrentScene.AddObject(rat);
            CurrentScene.AddObject(cegla);
            CurrentScene.AddObject(playerTime);
            CurrentScene.AddObject(testTransition);
            //CurrentScene.AddObject(pt);
            CurrentScene.AddObject(fuckYou);
            CurrentScene.AmbientLight = amb;
            CurrentScene.DirectionalLights[0] = ldr;
            CurrentScene.DirectionalLights[1] = ldrn;
            //CurrentScene.PointLights.Add(lp1);

            foreach(GameObject go in this.CurrentScene.ObjectsDictionary.Values)
            {
                go.Initialize();
            }
        }

        public void UnloadContent()
        {
            UnloadContentFinal();

            GUIManager.Instance.Clear();
            ImmediateStop = true;
            //TrashSoupGame.Instance.ReloadSpriteBatch();
            LoadTextures();
            LoadEffects();

            if(TrashSoupGame.Instance.GraphicsDevice.SamplerStates[0] == null)
            {
                TrashSoupGame.Instance.GraphicsDevice.SamplerStates[0] = SamplerState.LinearWrap;
            }
        }


        public void UnloadContentFinal()
        {
            if(CurrentScene != null)
            {
                foreach (GameObject obj in CurrentScene.ObjectsDictionary.Values)
                {
                    obj.Destroy();
                }
            }
            TrashSoupGame.Instance.Content.Unload();

            CurrentScene = null;
            Models.Clear();
            Textures.Clear();
            Animations.Clear();
            TexturesCube.Clear();
            Fonts.Clear();
            Effects.Clear();
            Sounds.Clear();
            Materials.Clear();
            PostEffects.Clear();
        }

        /// <summary>
        /// 
        /// Method gets cue from our list searching for elements which name contains "name" parameter.
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        public Cue GetCueFromCueList(String name)
        {
            return Sounds.Find(x => x.Name.Contains(name));
        }

        /// <summary>
        /// 
        /// Load every sound to cue list.
        /// </summary>
        private void LoadCues()
        {
            Sounds.Add(AudioManager.Instance.GetCue("Track1"));
        }

        /// <summary>
        /// 
        /// Load every model from content to textures list
        /// IMPORTANT!!! SET TAG FOR EVERY ELEMENT
        /// </summary>
        /// <param name="game"></param>
        public void AddModel(String path)
        {
            if(!Models.ContainsKey(path))
            {
                Models.Add(path, TrashSoupGame.Instance.Content.Load<Model>(path));
            }
        }

        private void AddAnimation(String path)
        {
            if(!Animations.ContainsKey(path))
            {
                Animations.Add(path, TrashSoupGame.Instance.Content.Load<Model>(path));
            }
        }

        /// <summary>
        /// 
        /// Checks if model with certain path exists in dictionary. If yes method returns it, if not, loads it, adds
        /// to proper dictionary and returns the model.
        /// </summary>
        /// <param name="path"></param>
        /// <returns></returns>
        public Model LoadModel(String path)
        {
            Model output = null;
            if (!Models.TryGetValue(path, out output))
            {
                output = TrashSoupGame.Instance.Content.Load<Model>(path);
                Models.Add(path, output);
                Debug.Log("New model successfully loaded - " + path);
            }
            return output;
        }

        public SpriteFont LoadFont(String path)
        {
            SpriteFont output = null;

            if (!Fonts.TryGetValue(path, out output))
            {
                output = TrashSoupGame.Instance.Content.Load<SpriteFont>(path);
                Fonts.Add(path, output);
                Debug.Log("Font successfully loaded - " + path);
            }
            return output;
        }

        /// <summary>
        /// 
        /// Checks if animation with certain path exists in dictionary. If yes method returns it, if not, loads it, adds
        /// to proper dictionary and returns the animation.
        /// </summary>
        /// <param name="path"></param>
        /// <returns></returns>
        public Model LoadAnimation(String path)
        {
            Model output = null;

            if (!Animations.TryGetValue(path, out output))
            {
                output = TrashSoupGame.Instance.Content.Load<Model>(path);
                Animations.Add(path, output);
                Debug.Log("New animation successfully loaded - " + path);
            }
            return output;
        }

        public Material LoadMaterial(String path)
        {
            string newName = Path.GetFileNameWithoutExtension(path);
            Material output = new Material();
            if (!ResourceManager.Instance.Materials.TryGetValue(newName, out output))
            {
                Material tmp = new Material();
                XmlSerializer serializer = new XmlSerializer(typeof(Material));
                using (FileStream file = new FileStream(path, FileMode.Open))
                {
                    tmp = (Material)serializer.Deserialize(file);
                    tmp.Name = newName;
                }
                output = tmp;
                Debug.Log("Material successfully loaded - " + newName);
                return output;
            }
            else
            {
                XmlSerializer serializer = new XmlSerializer(typeof(Material));
                using (FileStream file = new FileStream(path, FileMode.Open))
                {
                    output = (Material)serializer.Deserialize(file);
                    output.Name = newName;
                }
                if (!ResourceManager.Instance.Materials.ContainsKey(newName))
                {
                    ResourceManager.Instance.Materials.Add(newName, output);
                }
                Debug.Log("New material successfully loaded - " + newName);
                return output;
            }
        }

        public SkyboxMaterial LoadSkyboxMaterial(String path)
        {
            string newName = Path.GetFileNameWithoutExtension(path);
            SkyboxMaterial output = new SkyboxMaterial();
            XmlSerializer serializer = new XmlSerializer(typeof(SkyboxMaterial));
            using (FileStream file = new FileStream(path, FileMode.Open))
            {
                output = (SkyboxMaterial)serializer.Deserialize(file);
                output.Name = newName;
            }
            if (!ResourceManager.Instance.Materials.ContainsKey(newName))
            {
                ResourceManager.Instance.Materials.Add(newName, output);
            }
            Debug.Log("New skybox material successfully loaded - " + newName);
            return output;
        }

        public WaterMaterial LoadWaterMaterial(String path)
        {
            string newName = Path.GetFileNameWithoutExtension(path);
            WaterMaterial output = new WaterMaterial();
            XmlSerializer serializer = new XmlSerializer(typeof(WaterMaterial));
            using (FileStream file = new FileStream(path, FileMode.Open))
            {
                output = (WaterMaterial)serializer.Deserialize(file);
                output.Name = newName;
            }
            if (!ResourceManager.Instance.Materials.ContainsKey(newName))
            {
                ResourceManager.Instance.Materials.Add(newName, output);
            }
            Debug.Log("New water material successfully loaded - " + newName);
            return output;
        }

        public MirrorMaterial LoadMirrorMaterial(String path)
        {
            string newName = Path.GetFileNameWithoutExtension(path);
            MirrorMaterial output = new MirrorMaterial();
            XmlSerializer serializer = new XmlSerializer(typeof(MirrorMaterial));
            using (FileStream file = new FileStream(path, FileMode.Open))
            {
                output = (MirrorMaterial)serializer.Deserialize(file);
                output.Name = newName;
            }
            if (!ResourceManager.Instance.Materials.ContainsKey(newName))
            {
                ResourceManager.Instance.Materials.Add(newName, output);
            }
            Debug.Log("New mirror material successfully loaded - " + newName);
            return output;
        }

        /// <summary>
        /// Used to load a single texture. If it doesn't exist in resourceManager, Content.Load is called.
        /// </summary>
        /// <param name="texturePath"></param>
        public Texture2D LoadTexture(string texturePath)
        {
            if (Textures.ContainsKey(texturePath))
                return Textures[texturePath];
            else
            {
                Texture2D newTex = TrashSoupGame.Instance.Content.Load<Texture2D>(texturePath);
                Textures.Add(texturePath, newTex);
                return newTex;
            }
        }

        /// <summary>
        /// Used to load a single cube texture. If it doesn't exist in resourceManager, Content.Load is called.
        /// </summary>
        /// <param name="texturePath"></param>
        public TextureCube LoadTextureCube(string texturePath)
        {
            if (TexturesCube.ContainsKey(texturePath))
                return TexturesCube[texturePath];
            else
            {
                TextureCube newTex = TrashSoupGame.Instance.Content.Load<TextureCube>(texturePath);
                TexturesCube.Add(texturePath, newTex);
                return newTex;
            }
        }

        /// <summary>
        /// Checks if effect with certain path exists in dictionary. If yes method returns it, if not, loads it, adds
        /// to proper dictionary and returns the effect.
        /// </summary>
        /// <param name="effectPath"></param>
        /// <returns></returns>
        public Effect LoadEffect(string effectPath)
        {
            Effect output = null;
            if (!Effects.TryGetValue(effectPath, out output))
                //Debug.Log("Effect successfully loaded - " + effectPath);
            //else
            {
                output = TrashSoupGame.Instance.Content.Load<Effect>(effectPath);
                Effects.Add(effectPath, output);
                //Debug.Log("New effect successfully loaded - " + effectPath);
            }
            return output;
        }

        /// <summary>
        /// 
        /// Load every texture from content to textures list
        /// IMPORTANT!!! SET NAME FOR EVERY ELEMENT
        /// </summary>
        /// <param name="game"></param>
        public void LoadTextures()
        {
            // Adding "default" textures for all maps containing only one pixel in one color
            uint whiteColor = 0xFFFFFFFF;
            uint normColor = 0xFFFF0F0F;
            uint blackColor = 0xFF000000;
            if (!Textures.ContainsKey("DefaultDiffuse"))
            {
                Texture2D defDiff = new Texture2D(TrashSoupGame.Instance.GraphicsDevice, 1, 1, false, SurfaceFormat.Color);
                defDiff.SetData<uint>(new uint[] { blackColor });
                Textures.Add("DefaultDiffuse", defDiff);
            }
            if (!Textures.ContainsKey("DefaultDiffuseWhite"))
            {
                Texture2D defDiffW = new Texture2D(TrashSoupGame.Instance.GraphicsDevice, 1, 1, false, SurfaceFormat.Color);
                defDiffW.SetData<uint>(new uint[] { whiteColor });
                Textures.Add("DefaultDiffuseWhite", defDiffW);
            }
            if(!Textures.ContainsKey("DefaultNormal"))
            {
                Texture2D defNrm = new Texture2D(TrashSoupGame.Instance.GraphicsDevice, 1, 1, false, SurfaceFormat.Color);
                defNrm.SetData<uint>(new uint[] { normColor });
                Textures.Add("DefaultNormal", defNrm);
            }
            if (!TexturesCube.ContainsKey("DefaultCube"))
            {
                TextureCube defCbc = new TextureCube(TrashSoupGame.Instance.GraphicsDevice, 1, false, SurfaceFormat.Color);
                defCbc.SetData<uint>(CubeMapFace.NegativeX, new uint[] { blackColor });
                defCbc.SetData<uint>(CubeMapFace.PositiveX, new uint[] { blackColor });
                defCbc.SetData<uint>(CubeMapFace.NegativeY, new uint[] { blackColor });
                defCbc.SetData<uint>(CubeMapFace.PositiveY, new uint[] { blackColor });
                defCbc.SetData<uint>(CubeMapFace.NegativeZ, new uint[] { blackColor });
                defCbc.SetData<uint>(CubeMapFace.PositiveZ, new uint[] { blackColor });
                TexturesCube.Add("DefaultCube", defCbc);
            }
            ///////////////////////////////////////////
        }

        /// <summary>
        /// 
        /// Load every effect to effect list.
        /// IMPORTANT!!! SET NAME FOR EVERY ELEMENT
        /// </summary>
        /// <param name="game"></param>
        public void LoadEffects()
        {
            if(!Effects.ContainsKey("BasicEffect"))
            {
                Effects.Add("BasicEffect", new BasicEffect(TrashSoupGame.Instance.GraphicsDevice));
            }
            if(!Effects.ContainsKey("SkinnedEffect"))
            {
                Effects.Add("SkinnedEffect", new SkinnedEffect(TrashSoupGame.Instance.GraphicsDevice));
            }

            string path = @"Effects\DefaultEffect";
            LoadEffect(path);
            path = @"Effects\NormalEffect";
            LoadEffect(path);
            path = @"Effects\SkyboxEffect";
            LoadEffect(path);
            path = @"Effects\CubeNormalEffect";
            LoadEffect(path);
            path = @"Effects\MirrorEffect";
            LoadEffect(path);
            path = @"Effects\WaterEffect";
            LoadEffect(path);
            path = @"Effects\ShadowMapEffect";
            LoadEffect(path);
            path = @"Effects\ShadowMapBlurredEffect";
            LoadEffect(path);
            path = @"Effects\POSTBlurEffect";
            LoadEffect(path);
            path = @"Effects\POSTBoxBlurEffect";
            LoadEffect(path);
            path = @"Effects\ShadowMapUnnormalizedEffect";
            LoadEffect(path);
            path = @"Effects\POSTLogBlurEffect";
            LoadEffect(path);
            path = @"Effects\BillboardEffect";
            LoadEffect(path);
            path = @"Effects\POSTDefaultEffect";
            LoadEffect(path);

            DefaultPostEffect dpe = new DefaultPostEffect("Default");
            dpe.ColorMultiplication = new Vector3(1.1f, 1.1f, 1.1f);
            PostEffects.Add("Default", dpe);
        }

        /// <summary>
        /// Loads animation clip from Model object, when we load just an animated skeleton from Animations folder
        /// </summary>
        /// <param name="model"></param>
        /// <param name="animation"></param>
        /// <param name="newName"></param>
        /// <returns></returns>
        public KeyValuePair<string, SkinningModelLibrary.AnimationClip> LoadAnimationFromModel(Model model, Model animation, string newName)
        {
            // need to extract AnimationClips from animation and save it into new SkinningData with the rest
            // of the data from original skinned Model. 
            if((model.Tag as object[])[0] == null || (animation.Tag as object[])[0] == null) throw new InvalidOperationException("Either destination model or animation is not a skinned model");
            SkinningModelLibrary.SkinningData modelData = (model.Tag as object[])[0] as SkinningModelLibrary.SkinningData;
            SkinningModelLibrary.SkinningData animationData = (animation.Tag as object[])[0] as SkinningModelLibrary.SkinningData;
            if (modelData.SkeletonHierarchy.Count != animationData.SkeletonHierarchy.Count) throw new InvalidOperationException("Model hierarchy is not the same as the animation's");

            return new KeyValuePair<string, SkinningModelLibrary.AnimationClip>(newName, animationData.AnimationClips.Values.ElementAt(0));
        }

        /// <summary>
        /// Returns list of materials that are used by every given model's ModelMeshPart
        /// Loads these materials to material library as well.
        /// Effect types can be: BasicEffect, SkinnedEffect
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        public List<Material> LoadBasicMaterialsFromModel(Model model, Effect toBeAdded)
        {
            List<Material> materials = new List<Material>();

            List<SkinnedModelLibrary.MaterialModel> materialModels = (List<SkinnedModelLibrary.MaterialModel>)(model.Tag as object[])[1];
            for (int i = 0; i < materialModels.Count; ++i )
                materialModels[i] = ProcessMaterialModelFilenames(materialModels[i]);

            List<string> materialNames = (List<string>)(model.Tag as object[])[2];

            Effect effectToAdd;

            if(toBeAdded == null)
            {
                if (model.Meshes[0].Effects[0] is BasicEffect)
                {
                    effectToAdd = Effects["BasicEffect"];
                }
                else if (model.Meshes[0].Effects[0] is SkinnedEffect)
                {
                    effectToAdd = Effects["SkinnedEffect"];
                }
                else
                {
                    Debug.Log("MATERIAL LOADING FAILED: Unrecognized effect type in model.");
                    return materials;
                }
            }
            else
            {
                effectToAdd = toBeAdded;
            }
            
            foreach(SkinnedModelLibrary.MaterialModel mm in materialModels)
            {
                Material mat = new Material(mm.MaterialName, effectToAdd);
                if (mm.MaterialTextureNames[0] != null) mat.DiffuseMap = LoadTexture(mm.MaterialTextureNames[0]);
                if (mm.MaterialTextureNames[1] != null) mat.NormalMap = LoadTexture(mm.MaterialTextureNames[1]);
                if (mm.MaterialTextureNames[2] != null) mat.CubeMap = LoadTextureCube(mm.MaterialTextureNames[2]);

                if(!this.Materials.ContainsKey(mat.Name))
                {
                    this.Materials.Add(mat.Name, mat);
                }
            }

            foreach(string matName in materialNames)
            {
                materials.Add(this.Materials[matName]);
            }

            return materials;
        }

        private SkinnedModelLibrary.MaterialModel ProcessMaterialModelFilenames(SkinnedModelLibrary.MaterialModel matModel)
        {
            string rootDir = TrashSoupGame.ROOT_DIRECTIORY;
            string pRootDir = TrashSoupGame.ROOT_DIRECTIORY_PROJECT;
            for (int i = 0; i < SkinnedModelLibrary.MaterialModel.TEXTURE_COUNT; ++i )
            {
                if (matModel.MaterialTextureNames[i] == null)
                    continue;

                string path = matModel.MaterialTextureNames[i];
                string newPath = "";
                string[] strDivided = path.Split(new char[] { '\\' });

                string fileName = (strDivided.Last()).Split(new char[] { '.' })[0];
                newPath += fileName;

                int j = strDivided.Count() - 2;     // points to pre-last element, which is first part of tha path
                string tmpPathElement = fileName;
                while (j >= 0 && !(strDivided[j]).Equals(rootDir) && !(strDivided[j]).Equals(pRootDir))
                {
                    tmpPathElement = strDivided[j];
                    newPath = tmpPathElement + "\\" + newPath;
                    --j;
                }

                matModel.MaterialTextureNames[i] = newPath;
            }
            return matModel;
        }

        #endregion
    }
}