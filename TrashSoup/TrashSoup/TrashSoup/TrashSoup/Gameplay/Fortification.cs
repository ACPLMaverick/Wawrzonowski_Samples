using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Schema;
using System.Xml.Serialization;
using TrashSoup.Engine;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Audio;

namespace TrashSoup.Gameplay
{
    public class Fortification : ObjectComponent, IXmlSerializable
    {
        #region constants

        private const int TYPE_COUNT = 3;
        private const int PART_IN_TYPE_COUNT = 3;

        #endregion

        #region enums

        public enum FortificationType
        {
            METAL_WIRE_SNARES,
            WOOD1_WIRE_SNARES,
            WOOD2_TRAPWIRE_SNARES
        }

        public enum FortificationState
        {
            STATE_EMPTY = -1,
            STATE_FIRST,
            STATE_SECOND,
            STATE_THIRD
        }

        #endregion

        #region variables

        private static uint[] PartPrices = 
        {
            // type0
            10,
            20,
            50,
            // type1
            5,
            15,
            35,
            // type2
            7,
            20,
            40
        };

        private static uint[] PartTimes = 
        {
            // type0
            3,
            4,
            8,
            // type1
            2,
            4,  //25
            8,  //40
            // type2
            2,
            6,
            10
        };

        private static uint[] PartHealths = 
        {
            // type0
            250,
            250,
            250,
            // type1
            75,
            100,
            225,
            // type2
            75,
            325,
            200
        };

        private static string[] PartModels = 
        {
            // type0
            "Models/Home/Fortifications/metal_fortif",
            "Models/Home/Fortifications/wire_fortif",
            "Models/Home/Fortifications/trap_wnyki_double",
            // type1
            "Models/Home/Fortifications/wood_fortif",
            "Models/Home/Fortifications/wire_fortif",
            "Models/Home/Fortifications/trap_wnyki",
            // type2
            "Models/Home/Fortifications/wood_fortif2",
            "Models/Home/Fortifications/trap_wire",
            "Models/Home/Fortifications/trap_wnyki_double"
        };

        private static Vector3[] PartTranslations = 
        {
            // type0
            new Vector3(0.0f, 2.5f, 0.0f),
            new Vector3(0.0f, 1.0f, -1.2f),
            new Vector3(-1.4f, 0.5f, -1.1f),
            // type1
            new Vector3(0.0f, 2.5f, 0.0f),
            new Vector3(-0.3f, 1.0f, -0.75f),
            new Vector3(0.6f, 0.5f, -1.1f),
            // type2
            new Vector3(0.0f, 2.7f, 0.0f),
            new Vector3(0.0f, 2.9f, -0.1f),
            new Vector3(1.5f, 0.7f, -0.7f)
        };

        private static Vector3[] PartRotations = 
        {
            // type0
            new Vector3(0.2f, MathHelper.PiOver2, MathHelper.PiOver2),
            new Vector3(0.0f, 0.15f, 0.0f),
            new Vector3(0.0f, MathHelper.Pi - 0.6f, 0.0f),
            // type1
            new Vector3(0.0f, MathHelper.PiOver2, 0.0f),
            new Vector3(0.0f, 0.0f, 0.0f),
            new Vector3(0.0f, MathHelper.PiOver4, 0.0f),
            // type2
            new Vector3(0.0f, MathHelper.PiOver2, 0.0f),
            new Vector3(0.0f, 0.0f, 0.2f),
            new Vector3(0.0f, 0.3f, 0.0f)
        };

        private static float[] PartScales = 
        {
            // type0
            3.5f,
            2.7f,
            1.5f,
             // type1
            3.0f,
            2.7f,
            1.5f,
             // type2
            4.0f,
            7.5f,
            1.5f
        };

        private static float[] PartGlosses = 
        {
            // type0
            0.8f,
            0.5f,
            0.5f,
             // type1
            0.2f,
            0.5f,
            0.5f,
             // type2
            0.2f,
            0.5f,
            0.5f
        };

        private static Vector3 NotBuiltColor = new Vector3(0.8f, 0.8f, 1.0f);
        private static Vector3 BuiltColor = new Vector3(0.1f, 1.0f, 0.2f);

        private GameObject triggerEnemyObj;
        private GameObject triggerPlayerObj;

        private HideoutStash stashComponent;
        private PlayerController pController;

        private Texture2D interactionTexture;
        private Vector2 interactionTexturePos = new Vector2(0.475f, 0.775f);

        private int currentID;

        private bool playerInTrigger;
        private bool actionHelper;
        private bool soundHelper;

        private SoundEffectInstance buildSound;

        #endregion

        #region properties

        public int CurrentID 
        { 
            get
            {
                return currentID;
            }
            set
            {
                if(InRange(value) || value == -1)
                {
                    currentID = value;
                }
                else
                {
                    throw new InvalidOperationException("Value given to CurrentID is invalid!");
                }
            }
        }
        public int NextID
        {
            get
            {
                return CurrentID + 1;
            }
        }

        public FortificationPart[] Parts { get; private set; }
        public FortificationType MyType { get; set; }
        
        /// <summary>
        /// This property works only in runtime!!!
        /// </summary>
        public uint CurrentHealth 
        {
            get
            {
                return Parts[0].Health + Parts[1].Health + Parts[2].Health;
            }
            set
            {
                if (CurrentID == -1)
                    return;

                int difference = (int)value - (int)CurrentHealth;

                while(CurrentID >= 0)
                {
                    if (Parts[CurrentID].Health + difference > 0 && Parts[CurrentID].Health + difference <= Parts[CurrentID].MaxHealth)
                    {
                        Parts[CurrentID].Health += (uint)difference;
                        return;
                    }
                    else if (Parts[CurrentID].Health + difference > Parts[CurrentID].MaxHealth)  // a miracle has happened!
                    {
                        Parts[CurrentID].Health = Parts[CurrentID].MaxHealth;
                        return;
                    }
                    else // destruction of one or more parts may occur
                    {
                        Parts[CurrentID].Health = 0;
                        difference += (int)Parts[CurrentID].Health;
                        DestroyCurrent();
                    }
                }
                
            }
        }

        #endregion

        #region methods

        public Fortification(GameObject mObj)
            : base(mObj)
        {
            Start();
        }

        public Fortification(GameObject mObj, Fortification ff)
            : base(mObj, ff)
        {
            this.MyType = ff.MyType;
            this.CurrentID = ff.CurrentID;

            Start();
        }

        public override void Update(GameTime gameTime)
        {
            // solve building and fixing

            if(playerInTrigger && InRange(NextID))
            {
                GUIManager.Instance.DrawTexture(this.interactionTexture, this.interactionTexturePos, 0.05f, 0.05f);

                if (InputHandler.Instance.Action())
                {
                    if (actionHelper)
                    {
                        // fixing current one
                        if (InRange(CurrentID) && Parts[CurrentID].Health < Parts[CurrentID].MaxHealth)
                        {
                            if(Safehouse.SafehouseController.Instance.TrashStashed >= (int)Parts[CurrentID].PricePerMs)
                            {
                                if (!soundHelper)
                                {
                                    CommonActionStart();
                                }

                                Parts[CurrentID].BuildUp(gameTime);
                                //Debug.Log("HideoutStash: Fixing level " + CurrentID.ToString() + ", on " + Parts[CurrentID].Health.ToString() + "/" + Parts[CurrentID].MaxHealth.ToString() + " HP");

                                if (Parts[CurrentID].Health >= Parts[CurrentID].MaxHealth)
                                {
                                    Parts[CurrentID].Health = Parts[CurrentID].MaxHealth;
                                    actionHelper = false;
                                }
                            }
                            else
                            {
                                Debug.Log(this.MyObject.Name + " HideoutStash: Haha nie stac cie");
                                actionHelper = false;

                                if (soundHelper)
                                {
                                    CommonActionEnd();
                                }
                            }
                        }
                        // building further if build is in progress
                        else if (Parts[NextID].State == FortificationPart.PartState.BUILDING)
                        {
                            // check if we built it
                            if (Parts[NextID].Health >= Parts[NextID].MaxHealth)
                            {
                                Debug.Log("HideoutStash: Fortification level " + NextID.ToString() + " has been built.");
                                Parts[NextID].Health = Parts[NextID].MaxHealth;
                                Parts[NextID].State = FortificationPart.PartState.BUILT;
                                ++CurrentID;
                                if (InRange(NextID))
                                {
                                    Parts[NextID].State = FortificationPart.PartState.NEXT_BUILD;
                                    Parts[NextID].Visible = true;
                                }
                                    

                                actionHelper = false;
                            }
                            else
                            {
                                if (Safehouse.SafehouseController.Instance.TrashStashed >= (int)Parts[NextID].PricePerMs)
                                {
                                    if (!soundHelper)
                                    {
                                        CommonActionStart();
                                    }
                                    // update building of that part
                                    Parts[NextID].BuildUp(gameTime);
                                }
                                else
                                {
                                    Debug.Log(this.MyObject.Name + " HideoutStash: Haha nie stac cie");
                                    actionHelper = false;

                                    if (soundHelper)
                                    {
                                        CommonActionEnd();
                                    }
                                }
                            }
                        }
                        // we dont need to fix nor no build is in progress - acquire new build
                        else
                        {
                            // check if we can even build
                            if (Safehouse.SafehouseController.Instance.TrashStashed >= Parts[NextID].Price)
                            {
                                Parts[NextID].State = FortificationPart.PartState.BUILDING;
                            }
                            else
                            {
                                Debug.Log(this.MyObject.Name + " HideoutStash: Haha nie stac cie");
                                actionHelper = false;
                            }
                        }
                    }
                    else
                    {
                        if (soundHelper)
                        {
                            CommonActionEnd();
                        }
                    }
                }
                else
                {
                    if (soundHelper)
                    {
                        CommonActionEnd();
                    }

                    actionHelper = true;
                }
            }
            else
            {
                if (soundHelper)
                {
                    CommonActionEnd();
                }
            }

            

            /////////////////////////////////

            // solve attacking and damage


            //////////////////////////////

            // teting

            //if(InputManager.Instance.GetKeyboardButtonDown(Microsoft.Xna.Framework.Input.Keys.OemPlus))
            //{
            //    CurrentHealth += 10;
            //    Debug.Log("Fortification: New Health " + CurrentHealth.ToString());
            //}

            //if (InputManager.Instance.GetKeyboardButtonDown(Microsoft.Xna.Framework.Input.Keys.OemMinus))
            //{
            //    CurrentHealth -= 10;
            //    Debug.Log("Fortification: New Health " + CurrentHealth.ToString());
            //}
        }

        public override void Draw(Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, GameTime gameTime)
        {
        }

        protected override void Start()
        {
            Parts = new FortificationPart[PART_IN_TYPE_COUNT];
            MyType = (FortificationType)0;
            CurrentID = -1;
        }

        public override void Initialize()
        {
            MyObject.DrawLast = true;
            int typeNumber = (int)MyType;

            this.interactionTexture = ResourceManager.Instance.LoadTexture(@"Textures/HUD/x_button");

            for (int i = 0; i < PART_IN_TYPE_COUNT; ++i )
            {
                GameObject fortPart = new GameObject(MyObject.UniqueID + (uint)MyObject.Name.GetHashCode() + (uint)i, MyObject.Name + "FortificationPart" + (i).ToString());

                int tN = i + TYPE_COUNT * typeNumber;

                List<Material> mMats = ResourceManager.Instance.LoadBasicMaterialsFromModel(
                    ResourceManager.Instance.LoadModel(PartModels[tN]), ResourceManager.Instance.LoadEffect(@"Effects\NormalEffect"));

                foreach(Material mat in mMats)
                {
                    mat.Glossiness = PartGlosses[tN];
                }

                MyObject.AddChild(fortPart);

                CustomModel mod = new CustomModel(fortPart, new Model[] { ResourceManager.Instance.LoadModel(PartModels[tN]), null, null }, mMats);
                fortPart.Components.Add(mod);
                fortPart.MyTransform = new Transform(fortPart, PartTranslations[tN], Vector3.Up, PartRotations[tN], PartScales[tN]);
                fortPart.MyCollider = new BoxCollider(fortPart);
                fortPart.Tags.Add("Fortification");

                FortificationPart p = new FortificationPart(fortPart);
                p.MaxHealth = PartHealths[tN];
                p.Health = 0;
                p.Price = PartPrices[tN];
                p.TimeToBuild = PartTimes[tN];
                fortPart.Components.Add(p);

                p.Initialize();

                p.State = FortificationPart.PartState.PENDING;
                Parts[i] = p;
            }

            if(CurrentID == (int)FortificationState.STATE_EMPTY)
            {
                Parts[0].State = FortificationPart.PartState.NEXT_BUILD;
            }
            else if (CurrentID == (int)FortificationState.STATE_FIRST)
            {
                Parts[0].State = FortificationPart.PartState.BUILT;
                Parts[0].Health = Parts[0].MaxHealth;
                Parts[1].State = FortificationPart.PartState.NEXT_BUILD;
            }
            else if (CurrentID == (int)FortificationState.STATE_SECOND)
            {
                Parts[0].State = FortificationPart.PartState.BUILT;
                Parts[0].Health = Parts[0].MaxHealth;
                Parts[1].State = FortificationPart.PartState.BUILT;
                Parts[1].Health = Parts[1].MaxHealth;
                Parts[2].State = FortificationPart.PartState.NEXT_BUILD;
            }
            else if (CurrentID == (int)FortificationState.STATE_THIRD)
            {
                Parts[0].State = FortificationPart.PartState.BUILT;
                Parts[0].Health = Parts[0].MaxHealth;
                Parts[1].State = FortificationPart.PartState.BUILT;
                Parts[1].Health = Parts[1].MaxHealth;
                Parts[2].State = FortificationPart.PartState.BUILT;
                Parts[2].Health = Parts[2].MaxHealth;
            }

            triggerEnemyObj = new GameObject(MyObject.UniqueID + (uint)MyObject.Name.GetHashCode() + (uint)PART_IN_TYPE_COUNT, MyObject.Name + "FortificationTriggerEnemy");

            MyObject.AddChild(triggerEnemyObj);

            triggerEnemyObj.MyTransform = new Transform(triggerEnemyObj);
            triggerEnemyObj.MyTransform.Position = new Vector3(0.0f, 0.0f, 2.0f);
            triggerEnemyObj.MyTransform.Scale = 2.0f;
            triggerEnemyObj.MyCollider = new BoxCollider(triggerEnemyObj, true);

            triggerEnemyObj.OnTriggerEnterEvent += new GameObject.OnTriggerEnterEventHandler(OnTriggerEnterEnemyHandler);
            triggerEnemyObj.OnTriggerExitEvent += new GameObject.OnTriggerExitEventHandler(OnTriggerExitEnemyHandler);

            ///

            triggerPlayerObj = new GameObject(MyObject.UniqueID + (uint)MyObject.Name.GetHashCode() + (uint)PART_IN_TYPE_COUNT + 1, MyObject.Name + "FortificationTriggerPlayer");

            MyObject.AddChild(triggerPlayerObj);

            triggerPlayerObj.MyTransform = new Transform(triggerPlayerObj);
            triggerPlayerObj.MyTransform.Position = new Vector3(0.0f, 0.0f, 0.0f);
            triggerPlayerObj.MyTransform.Scale = 5.0f;
            triggerPlayerObj.MyCollider = new SphereCollider(triggerPlayerObj, true);

            triggerPlayerObj.OnTriggerEnterEvent += new GameObject.OnTriggerEnterEventHandler(OnTriggerEnterPlayerHandler);
            triggerPlayerObj.OnTriggerExitEvent += new GameObject.OnTriggerExitEventHandler(OnTriggerExitPlayerHandler);

            for (int i = 0; i < PART_IN_TYPE_COUNT; ++i )
            {
                triggerEnemyObj.MyCollider.IgnoredColliders.Add(Parts[i].MyObject.MyCollider);
                triggerPlayerObj.MyCollider.IgnoredColliders.Add(Parts[i].MyObject.MyCollider);
            }
            triggerEnemyObj.MyCollider.IgnoredColliders.Add(ResourceManager.Instance.CurrentScene.ObjectsDictionary[1].MyCollider);

            GameObject player = ResourceManager.Instance.CurrentScene.ObjectsDictionary[1];
            stashComponent = (HideoutStash)player.GetComponent<HideoutStash>();
            pController = (PlayerController)player.GetComponent<PlayerController>();

            SoundEffect se = TrashSoupGame.Instance.Content.Load<SoundEffect>(@"Audio/Ambient/building");
            buildSound = se.CreateInstance();
            buildSound.IsLooped = true;

            base.Initialize();
        }

        private void OnTriggerEnterPlayerHandler(object sender, CollisionEventArgs e)
        {
            playerInTrigger = true;

            if (InRange(NextID) && 
                Parts[NextID].State == FortificationPart.PartState.NEXT_BUILD &&
                ((InRange(CurrentID) && Parts[CurrentID].Health == Parts[CurrentID].MaxHealth) || (!InRange(CurrentID)))) Parts[NextID].Visible = true;
        }

        private void OnTriggerExitPlayerHandler(object sender, CollisionEventArgs e)
        {
            playerInTrigger = false;

            if (InRange(NextID) && Parts[NextID].State == FortificationPart.PartState.NEXT_BUILD) Parts[NextID].Visible = false;
        }

        private void OnTriggerEnterEnemyHandler(object sender, CollisionEventArgs e)
        {
            
        }

        private void OnTriggerExitEnemyHandler(object sender, CollisionEventArgs e)
        {
            
        }

        private void CommonActionStart()
        {
            soundHelper = true;
            buildSound.Play();
            pController.StartOtherState("Build");
        }

        private void CommonActionEnd()
        {
            soundHelper = false;
            buildSound.Stop(true);
            pController.StopOtherState();
        }

        private void DestroyCurrent()
        {
            Parts[CurrentID].Destroy();
            Parts[CurrentID].State = FortificationPart.PartState.NEXT_BUILD;
            if(InRange(NextID))
            {
                Parts[NextID].State = FortificationPart.PartState.PENDING;
                Parts[NextID].Health = 0;
            }
            if(playerInTrigger)
            {
                Parts[CurrentID].Visible = true;
            }

            --CurrentID;
        }

        private bool InRange(int id)
        {
            if (id >= 0 && id < PART_IN_TYPE_COUNT)
            {
                return true;
            }
            else return false;
        }

        public override XmlSchema GetSchema()
        {
            return base.GetSchema();
        }

        public override void ReadXml(XmlReader reader)
        {
            reader.MoveToContent();
            reader.ReadStartElement();

            base.ReadXml(reader);

            CurrentID = reader.ReadElementContentAsInt("CurrentID", "");

            string s = reader.ReadElementString("MyType", "");

            switch (s)
            {
                case "METAL_WIRE_SNARES":
                    MyType = FortificationType.METAL_WIRE_SNARES;
                    break;
                case "WOOD1_WIRE_SNARES":
                    MyType = FortificationType.WOOD1_WIRE_SNARES;
                    break;
                case "WOOD2_TRAPWIRE_SNARES":
                    MyType = FortificationType.WOOD2_TRAPWIRE_SNARES;
                    break;
            }

            reader.ReadEndElement();
        }

        public override void WriteXml(XmlWriter writer)
        {
            base.WriteXml(writer);

            writer.WriteElementString("CurrentID", XmlConvert.ToString(CurrentID));
            writer.WriteElementString("MyType", MyType.ToString());
        }

        #endregion
    }
}
