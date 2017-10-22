using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework.Audio;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay
{
    class TrashTrigger : ObjectComponent
    {
        #region constants

        private const float PICKING_SPEED = 0.0003f;

        #endregion

        #region variables

        private GameObject myTrash;
        private PlayerController player;
        private int trashCount = 1;

        private bool collisionWithPlayer = false;
        private Texture2D interactionTexture;
        private Vector2 interactionTexturePos = new Vector2(0.475f, 0.775f);

        private Cue foundCue;
        private bool foundIsPlaying = false;

        public bool Picked = false;
        private bool playerAnim = false;
        private float pickCounter = 0.0f;

        #endregion

        #region methods

        public TrashTrigger(GameObject go) : base(go)
        {

        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            if(this.collisionWithPlayer && !Picked)
            {
                GUIManager.Instance.DrawTexture(this.interactionTexture, this.interactionTexturePos, 0.05f, 0.05f);

                if (InputHandler.Instance.Action())
                {
                    if (!playerAnim)
                    {
                        playerAnim = true;
                        player.StartOtherState("PickTrash");
                    }
                    pickCounter += PICKING_SPEED * (float)gameTime.ElapsedGameTime.TotalMilliseconds;
                }
                else
                {
                    if (playerAnim)
                    {
                        playerAnim = false;
                        player.StopOtherState();
                    }
                }
                
                if(pickCounter >= 1.0f)
                {
                    this.MyObject.Enabled = false;
                    if(player != null)
                    {
                        player.Equipment.AddJunk(this.trashCount);
                        foundCue.Play();
                        player.CollectedTrash = true;
                        player.CollectedFakeTime = gameTime.TotalGameTime.TotalSeconds;
                        Picked = true;
                    }

                    if (playerAnim)
                    {
                        playerAnim = false;
                        player.StopOtherState();
                    }
                }
            }
            else if(playerAnim)
            {
                playerAnim = false;
                player.StopOtherState();
            }
        }

        public override void Draw(Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, Microsoft.Xna.Framework.GameTime gameTime)
        {

        }

        protected override void Start()
        {
        }

        public override void Initialize()
        {
            this.interactionTexture = ResourceManager.Instance.LoadTexture(@"Textures/HUD/x_button");
            if(!TrashSoupGame.Instance.EditorMode)
                this.foundCue = AudioManager.Instance.GetCue("found");
            base.Initialize();
        }

        public override void OnTriggerEnter(GameObject other)
        {
            if(other.UniqueID == 1)
            {
                player = (PlayerController)other.GetComponent<PlayerController>();
                this.collisionWithPlayer = true;
            }
            base.OnTriggerEnter(other);
        }

        public override void OnTriggerExit(GameObject other)
        {
            if(other.UniqueID == 1)
            {
                this.collisionWithPlayer = false;
            }
            base.OnTriggerExit(other);
        }

        public void Init(GameObject myTrash, int trashCount)
        {
            this.myTrash = myTrash;
            this.trashCount = trashCount;
        }

        public override void ReadXml(System.Xml.XmlReader reader)
        {
            reader.ReadStartElement();
            base.ReadXml(reader);

            reader.ReadEndElement();
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            base.WriteXml(writer);
        }

        #endregion

    }
}
