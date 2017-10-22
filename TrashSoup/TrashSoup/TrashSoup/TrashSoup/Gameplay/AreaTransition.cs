using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Schema;
using System.Xml.Serialization;
using TrashSoup.Engine;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;

namespace TrashSoup.Gameplay
{
    public class AreaTransition : ObjectComponent, IXmlSerializable
    {
        #region constants

        private static string MESSAGE_0 = "What do you want to do?";
        private static string MESSAGE_1 = "What do you want to do?";
        private static string MESSAGE_2 = "It's getting late. \nYou need to go back as quick as possible.";
        private static string MESSAGE_0_0 = "Go back";
        private static string MESSAGE_0_1 = "Stay here";
        private static string MESSAGE_0_2 = "Deposit trash";
        private static string MESSAGE_1_0 = " Go back";
        private static string MESSAGE_1_1 = " Stay here";
        private static string MESSAGE_2_0 = "OK";

        #endregion

        #region variables

        private static Vector2 messagePosition = new Vector2(0.2f, 0.35f);
        private static Vector2 messageDimenstions = new Vector2(0.6f, 0.15f);

        private PlayerTime cTime;
        private PlayerController pController;
        private HideoutStash stash;
        private Equipment equipment;
        private Cue music;
        private MessageBox[] mBoxes = new MessageBox[3];
        private GameObject colliderHelper;
        private bool activationHelper;
        private int cMessageBox = 0;

        private double actionTimer = 0;
        private double actionTimerUpper = 0;
        private delegate void ActionDelegate();
        private event ActionDelegate ActionEvent;

        private PlayerTime playerTime;

        private bool drawSwitch = true;

        #endregion

        #region properties

        public string NextScenePath { get; set; }

        #endregion

        #region methods

        public AreaTransition(GameObject obj) : base(obj)
        {
            Start();
        }

        public AreaTransition(GameObject obj, AreaTransition at) : base(obj, at)
        {
            Start();
        }

        public override void Update(GameTime gameTime)
        {
            if (!TrashSoupGame.Instance.EditorMode)
            {
                if (actionTimer < actionTimerUpper)
                {
                    actionTimer += gameTime.ElapsedGameTime.TotalMilliseconds;
                }
                else if (actionTimer > actionTimerUpper)
                {
                    ActionEvent();

                    ActionEvent = null;

                    actionTimerUpper = 0;
                    actionTimer = 0;
                }

                if (!mBoxes[cMessageBox].Active)
                {
                    // solve cMessageBox
                    int hours = cTime.Hours;
                    if (hours >= 22 && drawSwitch)  // prompt player immediately to return
                    {
                        cMessageBox = 2;
                        cTime.Stopped = true;
                        ActivateMessageBox();
                    }
                    else if (hours >= 21) // disable deposit stash options
                    {
                        cMessageBox = 1;
                    }
                    else
                    {
                        cMessageBox = 0;
                    }
                }
                else
                {
                    // draw current message box as it is active
                    mBoxes[cMessageBox].Draw(null);
                    mBoxes[cMessageBox].Update(gameTime);

                    // check prompt response and proceed accordingly
                    if (mBoxes[cMessageBox].Response != 0)
                    {
                        DectivateMessageBox();

                        if (mBoxes[cMessageBox].Response == 1)   // go back to hideout
                        {
                            if(this.playerTime != null)
                            {
                                Safehouse.SafehouseController.Instance.SetExitTime(playerTime.Hours, playerTime.Minutes);
                            }
                            actionTimer = 0;
                            actionTimerUpper = 500;
                            GUIManager.Instance.FadeIn(Color.Black, 500);
                            if(music != null)
                                music.Stop(AudioStopOptions.Immediate);
                            ActionEvent += new ActionDelegate(LoadToNextLevel);

                            if(cMessageBox == 2)
                            {
                                drawSwitch = false;
                            }
                        }
                        //else if (mBoxes[cMessageBox].Response == 2) // do actually nothing
                        //{
                        //    // nothing
                        //}
                        else if (mBoxes[cMessageBox].Response == 3) // initiate stash deposit
                        {
                            actionTimer = 0;
                            actionTimerUpper = 500;
                            GUIManager.Instance.FadeIn(Color.Black, 500);
                            ActionEvent += new ActionDelegate(StashStuff);
                            GameManager.Instance.MovementEnabled = false;
                        }
                    }
                }   
            }
        }

        public override void Draw(Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, GameTime gameTime)
        {
            
        }

        protected override void Start()
        {

        }

        public override void Initialize()
        {
            GameObject pt = ResourceManager.Instance.CurrentScene.GetObject("PlayerTime");
            if (pt == null)
            {
                throw new ArgumentNullException("AreaTransition: PlayerTime object does not exist!");
            }

            cTime = (PlayerTime)pt.GetComponent<PlayerTime>();

            GameObject player = ResourceManager.Instance.CurrentScene.GetObject(1);
            pController = (PlayerController)player.GetComponent<PlayerController>();
            stash = (HideoutStash)player.GetComponent<HideoutStash>();
            equipment = pController.Equipment;
            if ((player.GetComponent<PlayerHUD>() as PlayerHUD) != null)
            music = (player.GetComponent<PlayerHUD>() as PlayerHUD).ExplorationCue;

            if (cTime == null || pController == null || stash == null || equipment == null)
            {
                throw new ArgumentNullException("AreaTransition: some required components are inexistant!");
            }

            mBoxes[0] = new MessageBox(messagePosition, messageDimenstions.X, messageDimenstions.Y);
            mBoxes[0].OptionCount = 3;
            mBoxes[0].Text = MESSAGE_0;
            mBoxes[0].OptionTexts = new string[] { MESSAGE_0_0, MESSAGE_0_1, MESSAGE_0_2 };
            mBoxes[0].Initialize();

            mBoxes[1] = new MessageBox(messagePosition, messageDimenstions.X * 0.8f, messageDimenstions.Y);
            mBoxes[1].OptionCount = 2;
            mBoxes[1].Text = MESSAGE_1;
            mBoxes[1].OptionTexts = new string[] { MESSAGE_1_0, MESSAGE_1_1 };
            mBoxes[1].Initialize();

            mBoxes[2] = new MessageBox(messagePosition, messageDimenstions.X, messageDimenstions.Y);
            mBoxes[2].OptionCount = 1;
            mBoxes[2].Text = MESSAGE_2;
            mBoxes[2].OptionTexts = new string[] { MESSAGE_2_0 };
            mBoxes[2].Initialize();

            colliderHelper = new GameObject(MyObject.UniqueID + (uint)MyObject.Name.GetHashCode() + 1, MyObject.Name + "AreaTransitionHelper");
            MyObject.AddChild(colliderHelper);
            colliderHelper.MyTransform = new Transform(colliderHelper, Vector3.Zero, Vector3.Up, Vector3.Zero, MyObject.MyTransform.Scale * 1.3f);
            colliderHelper.MyCollider = new SphereCollider(colliderHelper, true);
            colliderHelper.OnTriggerEnterEvent += new GameObject.OnTriggerEnterEventHandler(HelperEnter);
            colliderHelper.OnTriggerExitEvent += new GameObject.OnTriggerExitEventHandler(HelperExit);

            GameObject playerTimeObj = ResourceManager.Instance.CurrentScene.GetObject(355);
            if(playerTimeObj != null)
            {
                this.playerTime = (PlayerTime)playerTimeObj.GetComponent<PlayerTime>();
            }

            base.Initialize();
        }

        public override void OnTriggerEnter(GameObject other)
        {
            // show prompt
            if(!activationHelper)
            {
                ActivateMessageBox();
            }

            base.OnTriggerEnter(other);
        }

        private void ActivateMessageBox()
        {
            mBoxes[cMessageBox].Active = true;
            activationHelper = true;

            GameManager.Instance.MovementEnabled = false;
        }

        private void DectivateMessageBox()
        {
            mBoxes[cMessageBox].Active = false;

            GameManager.Instance.MovementEnabled = true;
        }

        private void LoadToNextLevel()
        {
            stash.CurrentTrash += equipment.JunkCount;
            Safehouse.SafehouseController.Instance.TrashStashed = stash.CurrentTrash + 500;
            SaveManager.Instance.SaveState(stash.CurrentTrash);
            // fade in

            SaveManager.Instance.XmlPath = this.NextScenePath;
            SaveManager.Instance.LoadFileAction();

            stash = (HideoutStash)ResourceManager.Instance.CurrentScene.GetObject(1).GetComponent<HideoutStash>();
            stash.CurrentTrash += SaveManager.Instance.LoadState();

            GUIManager.Instance.FadeClear();
        }

        private void StashStuff()
        {
            stash.CurrentTrash += equipment.JunkCount;
            equipment.JunkCount = 0;

            cTime.TotalMilliseconds += 1000 * 60 * 60;

            GUIManager.Instance.FadeOut(Color.Black, 500);
            GameManager.Instance.MovementEnabled = true;
        }

        private void HelperEnter(object sender, CollisionEventArgs e)
        {

        }

        private void HelperExit(object sender, CollisionEventArgs e)
        {
            activationHelper = false;
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

            NextScenePath = reader.ReadElementString("NextScenePath", "");

            reader.ReadEndElement();
        }

        public override void WriteXml(XmlWriter writer)
        {
            base.WriteXml(writer);

            writer.WriteElementString("NextScenePath", NextScenePath);
        }

        #endregion
    

    }
}
