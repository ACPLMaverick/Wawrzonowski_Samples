using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Serialization;
using TrashSoup.Engine;
using TrashSoup.Gameplay.Weapons;

namespace TrashSoup.Gameplay
{
    public class Equipment : ObjectComponent, IXmlSerializable
    {
        #region variables
        public const int MAX_JUNK_CAPACITY = 20;
        public const int MAX_FOOD_CAPACITY = 5;
        private int currentJunkCount;
        private int currentFoodCount;
        private Weapon currentWeapon;

        private SpriteFont font;

        private bool showFoodWarning = false;
        private float foodWarningTimer = 0.0f;
        private Vector2 foodWarningPos = new Vector2(0.4f, 0.7f);

        private bool showJunkWarning = false;
        private float junkWarningTimer = 0.0f;
        private Vector2 junkWarningPos = new Vector2(0.4f, 0.7f);

        private bool junkWarningCurrentlyShowing = false;
        private bool foodWarningCurrentlyShowing = false;
        #endregion

        #region properties
        public int JunkCount
        {
            get { return currentJunkCount; }
            set { currentJunkCount = value; }
        }

        public int FoodCount
        {
            get { return currentFoodCount; }
            set { currentFoodCount = value; }
        }

        public Weapon CurrentWeapon
        {
            get { return currentWeapon; }
            set { currentWeapon = value; }
        }
        #endregion

        #region methods
        public Equipment(GameObject obj) : base(obj)
        {
            Start();
        }

        public void AddJunk(int count)
        {
            if (JunkCount + count <= MAX_JUNK_CAPACITY)
                JunkCount += count;
            else
            {
                showJunkWarning = true;
                this.JunkCount = MAX_JUNK_CAPACITY;
            }
            return;
        }

        public void AddFood()
        {
            if (FoodCount < MAX_FOOD_CAPACITY)
                FoodCount++;
            else
            {
                this.showFoodWarning = true;
                this.FoodCount = MAX_FOOD_CAPACITY;
            }
            return;
        }

        public void PickUpWeapon(GameObject newWeapon)
        {
            if (TrashSoupGame.Instance.EditorMode)
                return;

            newWeapon.Dynamic = true;
            newWeapon.MyCarrierSocket = new Socket(base.MyObject, newWeapon, null, "Bip001_R_Hand");
            newWeapon.MyTransform.Position = (newWeapon.Components.Find(x => x is Weapon) as Weapon).OffsetPosition;
            newWeapon.MyTransform.Rotation = (newWeapon.Components.Find(x => x is Weapon) as Weapon).OffsetRotation;
            newWeapon.MyTransform.Scale = 1;
            base.MyObject.MyCollider.IgnoredColliders.Add(newWeapon.MyCollider);
        }

        public void DropWeapon(GameObject weapon)
        {
            weapon.Dynamic = false;
            weapon.MyTransform.BakeTransformFromCarrierInvertZ();
            weapon.MyCarrierSocket = null;
            weapon.MyTransform.Rotation = Vector3.Zero;
            weapon.MyTransform.Scale = 3;
            base.MyObject.MyCollider.IgnoredColliders.Remove(weapon.MyCollider);
        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            if((showFoodWarning && !showJunkWarning) || foodWarningCurrentlyShowing)
            {
                foodWarningTimer += gameTime.ElapsedGameTime.Milliseconds * 0.001f;
                this.foodWarningCurrentlyShowing = true;
                if(foodWarningTimer < 2.3f)
                {
                    GUIManager.Instance.DrawText(this.font, "Can't carry any more food", this.foodWarningPos, Color.Red);
                }
                if(foodWarningTimer > 2.5f)
                {
                    showFoodWarning = false;
                    this.foodWarningCurrentlyShowing = false;
                    foodWarningTimer = 0.0f;
                }
            }

            if ((!showFoodWarning && showJunkWarning) || junkWarningCurrentlyShowing)
            {
                junkWarningTimer += gameTime.ElapsedGameTime.Milliseconds * 0.001f;
                this.junkWarningCurrentlyShowing = true;
                if(junkWarningTimer < 2.3f)
                {
                    GUIManager.Instance.DrawText(this.font, "Can't carry any more junk", this.junkWarningPos, Color.Red);
                }
                if (junkWarningTimer > 2.5f)
                {
                    this.junkWarningCurrentlyShowing = false;
                    showJunkWarning = false;
                    junkWarningTimer = 0.0f;
                }
            }

            if (currentWeapon.Destroyed)
            {
                CurrentWeapon.MyObject.Dynamic = false;
                CurrentWeapon.MyObject.MyTransform.BakeTransformFromCarrier();
                CurrentWeapon.MyObject.MyCarrierSocket = null;

                this.CurrentWeapon = (ResourceManager.Instance.CurrentScene.GetObject(1144).Components.First(x => x.GetType() == typeof(Weapons.Fists)) as Weapon);
                this.PickUpWeapon(ResourceManager.Instance.CurrentScene.GetObject(1144));
            }
        }

        public override void Draw(Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, Microsoft.Xna.Framework.GameTime gameTime)
        {

        }

        protected override void Start()
        {
            currentJunkCount = 0;
            currentFoodCount = 0;
            currentWeapon = new Fists(this.MyObject);
            if(TrashSoupGame.Instance != null)
            {
                this.font = TrashSoupGame.Instance.Content.Load<SpriteFont>("Fonts/FontTest");
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

            base.ReadXml(reader);

            JunkCount = reader.ReadElementContentAsInt("JunkCount", "");
            FoodCount = reader.ReadElementContentAsInt("FoodCount", "");

            if (reader.Name == "Weapon")
            {
                //reader.ReadStartElement();
                (CurrentWeapon as IXmlSerializable).ReadXml(reader);
                //reader.ReadEndElement();
            }

            reader.ReadEndElement();
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            base.WriteXml(writer);
            writer.WriteElementString("JunkCount", XmlConvert.ToString(JunkCount));
            writer.WriteElementString("FoodCount", XmlConvert.ToString(FoodCount));
            if(CurrentWeapon != null)
            {
                writer.WriteStartElement("Weapon");
                (CurrentWeapon as IXmlSerializable).WriteXml(writer);
                writer.WriteEndElement();
            }
        }
        #endregion
    }
}
