using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay
{
    public class Enemy : ObjectComponent, IXmlSerializable
    {
        #region variables
        protected int hitPoints;
        protected bool isDead;
        private bool deathAnimPlayed = false;

        public Action OnDead;

        private GameObject hpBar;
        private Billboard hpBarBillboardComp;
        private GameObject hpBarOutline;
        private Texture2D myHpBarTexture;
        public uint MyHPBarID;
        public uint MyHPBarOutlineID;
        private Vector3 hpBarYOffset = 2.0f * Vector3.Up;

        private int stairsTouching;

        public GameObject target;
        #endregion

        #region properties
        public int ArmorPoints
        {
            get;
            set;
        }

        public int HitPoints
        {
            get { return hitPoints; }
            set 
            {
                int diff = hitPoints - value;
                diff -= ArmorPoints;
                if (diff < 0) diff = 0;
                hitPoints = hitPoints - diff;
                if(this.hpBarBillboardComp != null)
                {
                    this.hpBarBillboardComp.Size = new Vector2(hitPoints / 100.0f, this.hpBarBillboardComp.Size.Y);
                }
            }
        }

        public int StairsTouching
        {
            get
            {
                return this.stairsTouching;
            }
            set
            {
                this.stairsTouching = value;
                if(SaveManager.Instance.XmlPath.Contains("save2"))
                {
                    this.MyObject.MyPhysicalObject.IsUsingGravity = this.stairsTouching == 0;
                    if (!this.MyObject.MyPhysicalObject.IsUsingGravity)
                    {
                        this.MyObject.MyPhysicalObject.Velocity = Vector3.Zero;
                        this.MyObject.MyPhysicalObject.ZeroForce();
                    }
                }
                else
                {
                    this.MyObject.MyPhysicalObject.IsUsingGravity = true;
                }
            }
        }

        public bool IsDead
        {
            get { return isDead; }
            set { isDead = value; }
        }
        #endregion

        #region methods
        public Enemy(GameObject obj) : base(obj)
        {
            this.hitPoints = 100;
            this.MyHPBarID = 0;
        }

        public Enemy(GameObject obj, Enemy e) : base(obj, e)
        {
            this.HitPoints = 100;
            this.MyHPBarID = e.MyHPBarID;
        }

        public Enemy(GameObject obj, int hitPoints):base(obj)
        {
            this.MyHPBarID = 0;
            this.hitPoints = hitPoints;
        }
        #endregion

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            if (this.HitPoints <= 0)
                IsDead = true;
            if (IsDead)
            {
                if(!deathAnimPlayed && OnDead != null)
                {
                    this.hpBar.Enabled = false;
                    this.hpBarOutline.Enabled = false;
                    deathAnimPlayed = true;
                    OnDead();
                    Safehouse.SafehouseController.Instance.EnemiesLeft -= 1;
                }
                return;
            }
            else
            {
                if (this.hpBar != null)
                {
                    this.hpBar.MyTransform.Position = this.MyObject.MyTransform.Position + hpBarYOffset;
                }
                if(this.hpBarOutline != null)
                {
                    this.hpBarOutline.MyTransform.Position = this.MyObject.MyTransform.Position + hpBarYOffset;
                }
            }
        }

        public override void Draw(Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, Microsoft.Xna.Framework.GameTime gameTime)
        {
            
        }

        public override void Initialize()
        {
            this.hpBar = ResourceManager.Instance.CurrentScene.GetObject(this.MyHPBarID);
            if(this.hpBar != null)
            {
                hpBarBillboardComp = (Billboard)this.hpBar.GetComponent<Billboard>();
                this.myHpBarTexture = hpBarBillboardComp.Mat.DiffuseMap;
            }
            this.StairsTouching = 0;
            if(this.GetComponent<Mutant>() != null)
            {
                hpBarYOffset = 3.0f * Vector3.Up;
            }
            this.hpBarOutline = ResourceManager.Instance.CurrentScene.GetObject(this.MyHPBarOutlineID);
            base.Initialize();
        }

        protected override void Start()
        {
            IsDead = false;
        }

        public void AttackTarget(float damage)
        {
            PlayerController pc = (PlayerController)target.GetComponent<PlayerController>();
            if(pc != null)
            {
                if(!pc.Dodged)
                {
                    AudioManager.Instance.SoundBank.PlayCue("hit");
                }
                pc.DecreaseHealth(damage);
            }
            else
            {
                if(this.MyObject.Name.Contains("Mutant"))
                {
                    damage *= 10;
                }
                Fortification f = (Fortification)target.GetComponent<Fortification>();
                if(f != null)
                {
                    AudioManager.Instance.SoundBank.PlayCue("fortificationHit");
                    f.CurrentHealth -= (uint)damage;
                }
                else
                {
                    f = (Fortification)target.GetParent().GetComponent<Fortification>();
                    if(f != null)
                    {
                        AudioManager.Instance.SoundBank.PlayCue("fortificationHit");
                        f.CurrentHealth -= (uint)damage;
                    }
                }
            }
        }

        public override System.Xml.Schema.XmlSchema GetSchema()
        {
            return null;
        }

        public override void ReadXml(System.Xml.XmlReader reader)
        {
            reader.MoveToContent();
            reader.ReadStartElement();

            reader.ReadStartElement("MyHPBarID");
            this.MyHPBarID = (uint)reader.ReadContentAsInt();
            reader.ReadEndElement();
            reader.ReadStartElement("MyHPBarOutlineID");
            this.MyHPBarOutlineID = (uint)reader.ReadContentAsInt();
            reader.ReadEndElement();
            base.ReadXml(reader);

            reader.ReadEndElement();
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            writer.WriteStartElement("MyHPBarID");
            writer.WriteValue(this.MyHPBarID);
            writer.WriteEndElement();
            writer.WriteStartElement("MyHPBarOutlineID");
            writer.WriteValue(this.MyHPBarOutlineID);
            writer.WriteEndElement();
            base.WriteXml(writer);
        }
    }
}
