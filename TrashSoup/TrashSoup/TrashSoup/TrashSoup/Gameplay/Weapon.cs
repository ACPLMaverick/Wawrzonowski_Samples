using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Serialization;
using TrashSoup.Engine;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Graphics;


namespace TrashSoup.Gameplay
{
    public enum WeaponType
    {
        FISTS,
        LIGHT,
        MEDIUM,
        HEAVY
    }

    public class Weapon : ObjectComponent, IXmlSerializable
    {
        #region constants

        private const int DURABILITY_TAKEN_PER_HIT = 1;
        private const double MS_TO_DISPOSE = 1000;
        private const double MS_TO_SHATTER = 250;

        #endregion

        #region variables
        protected int durability;
        protected int damage;
        protected Vector3 offsetPosition;
        protected Vector3 offsetRotation;
        protected WeaponType type;
        protected bool isCraftable;
        protected int craftingCost;
        protected string name;
        protected bool isAttacking = false;
        protected bool isCurrent;
        protected bool disposeBoolHelper;
        protected bool shatterHelper;
        protected double shatterTimer = 0;
        private double disposeHelper;
        public double timerOn;

        private GameObject player;
        private PlayerController pc;
        private ParticleSystem ps;
        private Cue jeb;
        private CustomModel cModel;

        private LightPoint myPoint;
        #endregion

        #region properties
        public int Durability
        {
            get { return durability; }
            set { durability = value; }
        }

        public int Damage
        {
            get { return damage; }
            set { damage = value; }
        }

        public WeaponType Type
        {
            get { return type; }
            set { type = value; }
        }

        public bool IsCraftable
        {
            get { return isCraftable; }
            set { isCraftable = value; }
        }

        public int CraftingCost
        {
            get { return craftingCost; }
            set { craftingCost = value; }
        }

        public string Name
        {
            get { return name; }
            set { name = value; }
        }

        public bool IsAttacking
        {
            get { return isAttacking; }
            set { isAttacking = value; }
        }

        public bool Destroyed { get; private set; }

        public string[] ParticleTexturePaths { get; set; }
        public string DestroyCueName { get; set; }

        public Vector3 OffsetPosition
        {
            get { return offsetPosition; }
            set { offsetPosition = value; }
        }

        public Vector3 OffsetRotation
        {
            get { return offsetRotation; }
            set { offsetRotation = value; }
        }

        #endregion

        #region methods
        public Weapon(GameObject obj):base(obj)
        {
        }

        public Weapon(GameObject obj, Weapon w) : base(obj, w)
        {
            
        }

        public override void Initialize()
        {
            if(type != WeaponType.FISTS)
            {
                myPoint = new LightPoint(110, "LightPoint1", Color.Orange.ToVector3(), Color.Orange.ToVector3(), 6.0f, false);
                MyObject.AddChild(myPoint);
                myPoint.MyTransform = new Transform(myPoint, new Vector3(0.0f, 0.25f, 0.0f), new Vector3(0.0f, 1.0f, 0.0f), new Vector3(0.0f, 0.0f, 0.0f), 1.0f);
                myPoint.MyCollider = new SphereCollider(myPoint, true);
                myPoint.MyCollider.CustomScale = new Vector3(5.0f, 0.0f, 0.0f);
                myPoint.MyPhysicalObject = new PhysicalObject(myPoint, 0.0f, 0.0f, false);

                ResourceManager.Instance.CurrentScene.PointLights.Add(myPoint);

                foreach (GameObject obj in ResourceManager.Instance.CurrentScene.ObjectsDictionary.Values)
                {
                    if (obj.Name.Contains("Street") && !obj.LightsAffecting.Contains(myPoint)
                        && Vector3.Distance(MyObject.MyTransform.PositionGlobal, obj.MyTransform.PositionGlobal) <= 20.0f)
                    {
                        myPoint.AffectedObjects.Add(obj);
                        obj.LightsAffecting.Add(myPoint);
                    }
                }
            }
            

            this.player = ResourceManager.Instance.CurrentScene.GetObject(1);
            if(this.player != null)
            {
                this.pc = (PlayerController)this.player.GetComponent<PlayerController>();
            }

            if (ParticleTexturePaths != null && ParticleTexturePaths.Count() != 0)
            {
                int pCount = ParticleTexturePaths.Count();
                GameObject dupa = new GameObject(132453245, "asfoieasjhgeowisughasaedokfgheasiourfdseyhaeyogfiuhsweoiughdseifoluh");
                ps = new ParticleSystem(this.MyObject);

                for (int i = 0; i < pCount; ++i )
                {
                    ps.Textures.Add(ResourceManager.Instance.LoadTexture(ParticleTexturePaths[i]));
                }

                ps.ParticleCount = 20;
                ps.ParticleSize = new Vector2(0.3f, 0.3f);
                ps.ParticleSizeVariation = new Vector2(0.2f, 0.2f);
                ps.LifespanSec = 0.8f;
                ps.Wind = new Vector3(0.0f, 0.0f, 0.0f);
                ps.Offset = new Vector3(MathHelper.Pi);
                ps.Speed = 2.0f;
                ps.RotationMode = ParticleSystem.ParticleRotationMode.DIRECTION_Z;
                ps.ParticleRotation = new Vector3(0.0f, 0.0f, MathHelper.PiOver4);
                ps.FadeInTime = 0.0f;
                ps.FadeOutTime = 0.05f;
                //ps.PositionOffset = new Vector3(0.0f, -1.0f, 0.0f) * MyObject.MyTransform.Scale;
                ps.BlendMode = BlendState.AlphaBlend;
                ps.UseGravity = true;
                ps.Mass = 0.00000005f;
                ps.Initialize();

                ps.Stop();

                dupa.Components.Add(ps);
                dupa.MyTransform = new Transform(dupa);

                MyObject.AddChild(dupa);
            }
            

            cModel = (CustomModel)MyObject.GetComponent<CustomModel>();

            if(DestroyCueName != null && !TrashSoupGame.Instance.EditorMode)
            {
                jeb = Engine.AudioManager.Instance.SoundBank.GetCue(DestroyCueName);
            }

            base.Initialize();
        }

        public override void Destroy()
        {
            RemoveMyPointLight();
            base.Destroy();
        }

        public void TurnMyPointLight(bool val)
        {
            if(myPoint != null)
                myPoint.Enabled = val;
        }

        public void RemoveMyPointLight()
        {
            if (myPoint == null)
                return;
            ResourceManager.Instance.CurrentScene.RemovePointLight(myPoint);
            myPoint = null;
        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            if (TrashSoupGame.Instance.EditorMode) return;
            if (IsAttacking)
            {
                ((PlayerController)player.GetComponent<PlayerController>()).Walk.Stop(true);   
            }
            if(gameTime.TotalGameTime.TotalSeconds - timerOn > 1.5f)
                isAttacking = false;

            if (!TrashSoupGame.Instance.EditorMode)
            {
                if (pc.Equipment.CurrentWeapon == this && !isCurrent)
                {
                    pc.MyAttackTriggerComponent.AttackEvent += new PlayerAttackTrigger.AttackEventHandler(OnAttackHandler);
                    isCurrent = true;
                }
                else if (pc.Equipment.CurrentWeapon != this && isCurrent)
                {
                    pc.MyAttackTriggerComponent.AttackEvent -= OnAttackHandler;
                    isCurrent = false;
                }   
            }

            if(Destroyed && !disposeBoolHelper)
            {
                disposeBoolHelper = true;
                disposeHelper = gameTime.TotalGameTime.TotalMilliseconds;
            }

            if(disposeBoolHelper && (gameTime.TotalGameTime.TotalMilliseconds - disposeHelper > MS_TO_DISPOSE))
            {
                ResourceManager.Instance.CurrentScene.RemovePointLight(myPoint);
                ResourceManager.Instance.CurrentScene.DeleteObjectRuntime(MyObject);
            }

            if(shatterHelper)
            {
                shatterTimer = gameTime.TotalGameTime.TotalMilliseconds;
                shatterHelper = false;
            }

            if (shatterTimer != 0 && (gameTime.TotalGameTime.TotalMilliseconds - shatterTimer > MS_TO_SHATTER))
            {
                Shatter();
                shatterTimer = 0;
            }
        }

        public override void Draw(Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, Microsoft.Xna.Framework.GameTime gameTime)
        {

        }

        protected override void Start()
        {

        }

        private void OnAttackHandler(object o, CollisionEventArgs e)
        {
            Debug.Log("JEB! Durability = " + Durability.ToString());

            if(Durability > 0)
            {
                Durability -= DURABILITY_TAKEN_PER_HIT;
                if (Durability <= 0)
                {
                    shatterHelper = true;
                }
            }
        }

        private void Shatter()
        {
            Debug.Log("ALE ÓRWAŁ! Weapon shattered!");
            Destroyed = true;

            if (cModel != null)
                cModel.Visible = false;

            if(ps != null)
                ps.Play();

            if(jeb != null)
                jeb.Play();
        }

        public override System.Xml.Schema.XmlSchema GetSchema()
        {
            return null;
        }

        public override void ReadXml(System.Xml.XmlReader reader)
        {
            reader.MoveToContent();
            reader.ReadStartElement();

            base.ReadXml(reader);
            Durability = reader.ReadElementContentAsInt("Durability", "");
            Damage = reader.ReadElementContentAsInt("Damage", "");
            string s = reader.ReadElementString("Type", "");
            switch(s)
            {
                case "FISTS":
                    Type = WeaponType.FISTS;
                    break;
                case "LIGHT":
                    Type = WeaponType.LIGHT;
                    break;
                case "MEDIUM":
                    Type = WeaponType.MEDIUM;
                    break;
                case "HEAVY":
                    Type = WeaponType.HEAVY;
                    break;
            }

            reader.ReadEndElement();
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            base.WriteXml(writer);
            writer.WriteElementString("Durability", XmlConvert.ToString(Durability));
            writer.WriteElementString("Damage", XmlConvert.ToString(Damage));
            writer.WriteElementString("Type", Type.ToString());
        }
        #endregion
    }
}