using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay
{
    public class Food : ObjectComponent
    {
        private GameObject myTrigger;
        private LightPoint myPoint;

        public Food(GameObject go) : base(go)
        {
        }

        public Food(GameObject go, Food f) : base(go, f)
        {
            
        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {

        }

        public override void Draw(Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, Microsoft.Xna.Framework.GameTime gameTime)
        {
        }

        protected override void Start()
        {
        }

        public override void Initialize()
        {
            myPoint = new LightPoint(110, "LightPoint1", Color.Green.ToVector3(), Color.Green.ToVector3(), 10.0f, false);
            MyObject.AddChild(myPoint);
            myPoint.MyTransform = new Transform(myPoint, new Vector3(0.0f, 0.25f, 0.0f), new Vector3(0.0f, 1.0f, 0.0f), new Vector3(0.0f, 0.0f, 0.0f), 4.0f);
            myPoint.MyCollider = new SphereCollider(myPoint, true);
            myPoint.MyCollider.CustomScale = new Vector3(6.0f, 0.0f, 0.0f);
            myPoint.MyPhysicalObject = new PhysicalObject(myPoint, 0.0f, 0.0f, false);

            ResourceManager.Instance.CurrentScene.PointLights.Add(myPoint);

            foreach(GameObject obj in ResourceManager.Instance.CurrentScene.ObjectsDictionary.Values)
            {
                if(obj.Name.Contains("Street") && !obj.LightsAffecting.Contains(myPoint) 
                    && Vector3.Distance(MyObject.MyTransform.PositionGlobal, obj.MyTransform.PositionGlobal) <= 25.0f)
                {
                    myPoint.AffectedObjects.Add(obj);
                    obj.LightsAffecting.Add(myPoint);
                }
            }

            base.Initialize();
        }

        public override void Destroy()
        {
            RemoveMyPointLight();
            base.Destroy();
        }

        public void RemoveMyPointLight()
        {
            if (myPoint == null)
                return;
            ResourceManager.Instance.CurrentScene.RemovePointLight(myPoint);
            myPoint = null;
        }

        public override void OnTriggerEnter(GameObject other)
        {
            if (other.UniqueID == 1)
            {
                PlayerController pc = (PlayerController)other.GetComponent<PlayerController>();
                if (pc != null)
                {
                    pc.FoodSaw = true;
                    pc.Food = this;
                }
            }
            base.OnTriggerEnter(other);
        }

        public override void OnTriggerExit(GameObject other)
        {
            if (other.UniqueID == 1)
            {
                PlayerController pc = (PlayerController)other.GetComponent<PlayerController>();
                if (pc != null)
                {
                    pc.FoodSaw = false;
                    pc.Food = null;
                }
            }
            base.OnTriggerExit(other);
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
