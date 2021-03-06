﻿using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;

namespace TrashSoup.Gameplay
{
    class PlayerHUD : ObjectComponent
    {
        private string[] messages = new string[] { "#trashsoup: check dis out!", "#trashsoup: love my bf! Eric <3", "#trashsoup: TUL is awesome!", "#trashsoup: he's gonna win!", "#trashsoup: nailed it!", "#trashsoup: I love him <3", "#trashsoup: so #yolo !", "#trashsoup: more on social media!" };

        private Texture2D hpTexture;
        private Texture2D hpBGTexture;
        private Texture2D heartTexture;
        private Texture2D popularitySadTexture;
        private Texture2D populartiyHappyTexture;
        private Texture2D popularityBGTexture;
        private Texture2D popularityFillTexture;
        private Texture2D backpackTexture;
        private Texture2D burgerTexture;
        private SpriteFont equipmentFont;
        private Texture2D liveBGTexture;
        private Texture2D liveTexture;

        private Texture2D bgTexture;

        //Positioning, like CSS -,-'
        private Vector2 hpPos = new Vector2(0.09f, 0.0875f);
        private Vector2 heartPos = new Vector2(0.06f, 0.05f);
        private Vector2 popularityPos = new Vector2(0.05f, 0.9f);
        private Vector2 popularityFacePos = new Vector2(0.03f, 0.875f);
        private Vector2 backpackPos = new Vector2(0.75f, 0.075f);
        private Vector2 burgerPos = new Vector2(0.87f, 0.065f);
        private Vector2 eqTextPos = new Vector2(0.79f, 0.085f);
        private Vector2 foodTextPos = new Vector2(0.91f, 0.085f);
        private Vector2 liveBGPos = new Vector2(0.001f, 0.94f);
        private Vector2 livePos = new Vector2(0.01f, 0.9475f);
        private Vector2 dayInfoPos = new Vector2(0.15f, 0.82f);
        private Vector2 initialLiveTextPos = new Vector2(0.395f, 0.9475f);
        private Vector2 liveTextPos;

        private Vector2 bgPosition = new Vector2(0.732f, 0.065f);

        private string currentLiveText = "";
        private int index = 0;

        private PlayerController myPlayerController;

        private float maxHP = PlayerController.MAX_HEALTH;
        private float currentHP = 0.0f;
        private float maxWidth = 0.2f;
        private float maxPopularity = PlayerController.MAX_POPULARITY;
        private float currentPopularity = 0.0f;
        private int currentJunkCount = 0;
        private int currentFoodCount = 0;
        private int maxJunkCount = Equipment.MAX_JUNK_CAPACITY;
        private int maxFoodCount = Equipment.MAX_FOOD_CAPACITY;
        private Equipment myEq;
        private Color junkColor = Color.White;
        private Color foodColor = Color.White;

        private float timer = 0.0f;
        private float changeMessageCooldown = 8.0f;
        private int currentMessage = 0;

        private bool drawLive = false;
        private float popularityFirstStepAmount = 30.0f;
        private float popularitySecondStepAmount = 60.0f;
        private bool popularityFirstStepReached = false;
        private bool popularitySecondStepReached = false;
        private bool popularityThirdStepReached = false;
        private float popularityThirdStepAmount = 90.0f;

        private SoundEffect popularityFirstStepApplause;
        private SoundEffect popularitySecondStepApplause;
        private SoundEffect popularityThirdStepApplause;

        private SoundEffectInstance popularityFirstEffectInstance;
        private SoundEffectInstance popularitySecondEffectInstance;
        private SoundEffectInstance popularityThirdEffectInstance;

        /***COMMENTARY***/
        //private SoundEffect welcomeSoundEffect;
        //private SoundEffectInstance welcomeEffectInstance;
        //////////////////
        
        private Color collectedSthColor = Color.Yellow;
        private Color looseSthColor = Color.DarkRed;

        private int prevJunkCount = 0;
        private int prevFoodCount = 0;
        private bool showJunkGain = false;
        private bool showJunkLoose = false;
        private bool showFoodGain = false;
        private bool showFoodLoose = false;
        private float junkGainLooseTimer = 0.0f;
        private float foodGainLooseTimer = 0.0f;
        private float junkTextSize = 1.0f;
        private float foodTextSize = 1.0f;
        private float biggerTextSize = 1.5f;

        private bool safehouse = false;

        public Cue ExplorationCue;

        public PlayerHUD(GameObject go) : base(go)
        {
            Start();
        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            if(TrashSoupGame.Instance.EditorMode)
            {
                return;
            }
            this.currentHP = this.myPlayerController.HitPoints;
            this.currentPopularity = this.myPlayerController.Popularity;

            //Drawing HP
            GUIManager.Instance.DrawTexture(this.hpBGTexture, this.hpPos, this.maxWidth, 0.0125f);
            GUIManager.Instance.DrawTexture(this.hpTexture, this.hpPos, this.maxWidth * this.currentHP / this.maxHP, 0.0125f);
            GUIManager.Instance.DrawTexture(this.heartTexture,this.heartPos, 0.04f, 0.05f);

            //Drawing popularity bar
            GUIManager.Instance.DrawTexture(this.popularityBGTexture, this.popularityPos, this.maxWidth, 0.0125f);
            GUIManager.Instance.DrawTexture(this.popularityFillTexture, this.popularityPos, this.maxWidth * this.currentPopularity / this.maxPopularity, 0.0125f);
            if(this.currentPopularity > 0.0f)
            {
                GUIManager.Instance.DrawTexture(this.populartiyHappyTexture, this.popularityFacePos, 0.03f, 0.03f);
            }
            else
            {
                GUIManager.Instance.DrawTexture(this.popularitySadTexture, this.popularityFacePos, 0.03f, 0.03f);
            }

            //Equipment
            GUIManager.Instance.DrawTexture(this.bgTexture, bgPosition, 0.23f, 0.045f);
            this.currentFoodCount = this.myEq.FoodCount;
            if(safehouse)
            {
                this.currentJunkCount = (int)Safehouse.SafehouseController.Instance.TrashStashed;
            }
            else
            {
                this.currentJunkCount = this.myEq.JunkCount;
            }
            if(!showFoodGain && this.currentFoodCount > this.prevFoodCount)
            {
                showFoodGain = true;
                showFoodLoose = false;
                foodGainLooseTimer = 0.0f;
            }
            
            if(!showFoodLoose && this.currentFoodCount < this.prevFoodCount)
            {
                showFoodLoose = true;
                showFoodGain = false;
                foodGainLooseTimer = 0.0f;
            }

            if (!showJunkGain && this.currentJunkCount > this.prevJunkCount)
            {
                showJunkGain = true;
                showJunkLoose = false;
                junkGainLooseTimer = 0.0f;
            }
            
            if (!showJunkLoose && this.currentJunkCount < this.prevJunkCount)
            {
                showJunkLoose = true;
                showJunkGain = false;
                junkGainLooseTimer = 0.0f;
            }

            this.junkColor = this.currentJunkCount == Equipment.MAX_JUNK_CAPACITY ? Color.Red : Color.White;
            this.foodTextSize = 1.0f;
            this.junkTextSize = 1.0f;
            this.foodColor = this.currentFoodCount == Equipment.MAX_FOOD_CAPACITY ? Color.Red : Color.White;
           
            if(showFoodGain)
            {
                foodGainLooseTimer += gameTime.ElapsedGameTime.Milliseconds * 0.001f;
                if(foodGainLooseTimer > 1.5f)
                {
                    showFoodGain = false;
                }
                this.foodColor = this.collectedSthColor;
                this.foodTextSize = this.biggerTextSize;
            }
            if (showFoodLoose)
            {
                foodGainLooseTimer += gameTime.ElapsedGameTime.Milliseconds * 0.001f;
                if (foodGainLooseTimer > 1.5f)
                {
                    showFoodLoose = false;
                }
                this.foodColor = this.looseSthColor;
                this.foodTextSize = this.biggerTextSize;
            }
            if (showJunkGain)
            {
                junkGainLooseTimer += gameTime.ElapsedGameTime.Milliseconds * 0.001f;
                if (junkGainLooseTimer > 1.5f)
                {
                    showJunkGain = false;
                }
                this.junkColor = this.collectedSthColor;
                this.junkTextSize = this.biggerTextSize;
            }
            if (showJunkLoose)
            {
                junkGainLooseTimer += gameTime.ElapsedGameTime.Milliseconds * 0.001f;
                if (junkGainLooseTimer > 1.5f)
                {
                    showJunkLoose = false;
                }
                this.junkColor = this.looseSthColor;
                this.junkTextSize = this.biggerTextSize;
            }

            GUIManager.Instance.DrawTexture(this.backpackTexture, this.backpackPos, 0.03f, 0.03f);
            if(safehouse)
            {
                GUIManager.Instance.DrawText(this.equipmentFont, this.currentJunkCount.ToString(), this.eqTextPos, this.junkColor, this.junkTextSize);
            }
            else
            {
                GUIManager.Instance.DrawText(this.equipmentFont, this.currentJunkCount + "/" + this.maxJunkCount, this.eqTextPos, this.junkColor, this.junkTextSize);
            }
            GUIManager.Instance.DrawTexture(this.burgerTexture, this.burgerPos, 0.035f, 0.045f);
            GUIManager.Instance.DrawText(this.equipmentFont, this.currentFoodCount + "/" + this.maxFoodCount, this.foodTextPos, this.foodColor, this.foodTextSize);

            GUIManager.Instance.DrawText(this.equipmentFont, "DAY 1", this.dayInfoPos, Color.Red);

            //Live messages drawing


            if(!this.popularityFirstStepReached && this.currentPopularity >= this.popularityFirstStepAmount)
            {
                this.popularityFirstStepReached = true;
                this.popularityFirstEffectInstance.Play();
                this.drawLive = true;
            }
            if(!this.popularitySecondStepReached && this.currentPopularity >= this.popularitySecondStepAmount)
            {
                this.popularitySecondStepReached = true;
                this.popularitySecondEffectInstance.Play();
                this.drawLive = true;
            }
            else if(!this.popularityThirdStepReached && this.currentPopularity >= this.popularityThirdStepAmount)
            {
                this.popularityThirdStepReached = true;
                this.popularityThirdEffectInstance.Play();
                this.drawLive = true;
            }

            if(this.currentPopularity < this.popularityFirstStepAmount)
            {
                this.popularityFirstStepReached = false;
            }
            if(this.currentPopularity < this.popularitySecondStepAmount)
            {
                this.popularitySecondStepReached = false;
            }
            if(this.currentPopularity < this.popularityThirdStepAmount)
            {
                this.popularityThirdStepReached = false;
            }
            

            if (this.drawLive)
            {
                GUIManager.Instance.DrawTexture(this.liveBGTexture, this.liveBGPos, 0.4f, 0.03f);
                GUIManager.Instance.DrawText(this.equipmentFont, this.currentLiveText, this.liveTextPos, Color.Black);
                GUIManager.Instance.DrawTexture(this.liveTexture, this.livePos, 0.05f, 0.02f);
                this.timer += 0.001f * gameTime.ElapsedGameTime.Milliseconds;
                this.liveTextPos.X -= 0.08f * gameTime.ElapsedGameTime.Milliseconds * 0.001f;
                if (this.index < this.messages[this.currentMessage].Length && this.timer > (0.32f * this.messages[this.currentMessage].Length / this.messages[0].Length) * (index + 1) * this.changeMessageCooldown / (float)this.messages[this.currentMessage].Length)
                {
                    this.currentLiveText += this.messages[this.currentMessage][this.index];
                    this.index += 1;
                }
                if (this.timer > this.changeMessageCooldown)
                {
                    this.currentLiveText = "";
                    this.index = 0;
                    this.liveTextPos = this.initialLiveTextPos;
                    this.timer = 0.0f;
                    this.currentMessage = SingleRandom.Instance.rnd.Next(0, this.messages.Length - 1);
                    this.drawLive = false;
                }
            }

            this.prevJunkCount = this.currentJunkCount;
            this.prevFoodCount = this.currentFoodCount;
        }

        public override void Draw(Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, Microsoft.Xna.Framework.GameTime gameTime)
        {

        }

        protected override void Start()
        {
            this.liveTextPos = this.initialLiveTextPos;
        }

        public override void Initialize()
        {
            this.safehouse = SaveManager.Instance.XmlPath.Contains("safehouse");

            myPlayerController = (PlayerController)this.MyObject.GetComponent<PlayerController>();
            if(myPlayerController == null)
            {
                Debug.Log("There is some error in getting player controller");
            }
            this.maxHP = this.myPlayerController.HitPoints;
            this.currentHP = this.maxHP;
            this.myEq = this.myPlayerController.Equipment;
            this.hpTexture = ResourceManager.Instance.LoadTexture(@"Textures/HUD/HP");
            this.hpBGTexture = ResourceManager.Instance.LoadTexture(@"Textures/HUD/HPBG");
            this.heartTexture = ResourceManager.Instance.LoadTexture(@"Textures/HUD/heart");
            this.popularityBGTexture = ResourceManager.Instance.LoadTexture(@"Textures/HUD/fameBG");
            this.popularityFillTexture = ResourceManager.Instance.LoadTexture(@"Textures/HUD/fame");
            this.popularitySadTexture = ResourceManager.Instance.LoadTexture(@"Textures/HUD/sad");
            this.populartiyHappyTexture = ResourceManager.Instance.LoadTexture(@"Textures/HUD/smile");
            this.backpackTexture = ResourceManager.Instance.LoadTexture(@"Textures/HUD/backpack"); ;
            this.burgerTexture = ResourceManager.Instance.LoadTexture(@"Textures/HUD/burger");
            this.equipmentFont = TrashSoupGame.Instance.Content.Load<SpriteFont>(@"Fonts/FontTest");
            this.liveBGTexture = ResourceManager.Instance.LoadTexture(@"Textures/HUD/LIVEBG");
            this.liveTexture = ResourceManager.Instance.LoadTexture(@"Textures/HUD/LIVE");

            this.bgTexture = ResourceManager.Instance.LoadTexture(@"Textures/HUD/window_red2_0");

            if (!TrashSoupGame.Instance.EditorMode)
            {
                ExplorationCue = AudioManager.Instance.GetCue("exploration");
                ExplorationCue.Play();
            }

            popularityFirstStepApplause = TrashSoupGame.Instance.Content.Load<SoundEffect>(@"Audio/Ambient/applause_1");
            popularityFirstEffectInstance = popularityFirstStepApplause.CreateInstance();
            popularitySecondStepApplause = TrashSoupGame.Instance.Content.Load<SoundEffect>(@"Audio/Ambient/applause_2");
            popularitySecondEffectInstance = popularitySecondStepApplause.CreateInstance();
            popularityThirdStepApplause = TrashSoupGame.Instance.Content.Load<SoundEffect>(@"Audio/Ambient/applause_3");
            popularityThirdEffectInstance = popularityThirdStepApplause.CreateInstance();

            base.Initialize();
        }

        public override System.Xml.Schema.XmlSchema GetSchema()
        {
            return null;
        }

        public override void ReadXml(System.Xml.XmlReader reader)
        {
            reader.ReadStartElement();

            base.ReadXml(reader);
            //MyObject = ResourceManager.Instance.CurrentScene.GetObject(tmp);

            reader.ReadEndElement();
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            base.WriteXml(writer);
        }
    }
}