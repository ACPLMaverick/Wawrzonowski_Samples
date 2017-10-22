using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace TrashSoup.Engine
{
    public abstract class ObjectComponent : IXmlSerializable
    {
        #region variables

        public GameObject MyObject;

        #endregion

        #region properties

        public bool Enabled { get; set; }
        public bool Visible { get; set; }
        public uint tmp;

        #endregion

        #region methods
        public ObjectComponent()
        {
            this.MyObject = null;
            this.Enabled = true;
            this.Visible = true;
        }

        public ObjectComponent(GameObject myObj)
        {
            this.MyObject = myObj;
            this.Enabled = true;
            this.Visible = true;
        }

        public ObjectComponent(GameObject myObj, ObjectComponent oc)
        {
            this.MyObject = myObj;
            this.Enabled = oc.Enabled;
            this.Visible = oc.Visible;
        }

        public abstract void Update(GameTime gameTime);

        public abstract void Draw(Camera cam, Effect effect, GameTime gameTime);

        protected abstract void Start();

        public virtual void Initialize()
        {

        }

        public virtual void Destroy()
        {

        }

        public virtual void OnCollision(GameObject other)
        {

        }

        public virtual void OnTrigger(GameObject other)
        {

        }

        public virtual void OnTriggerEnter(GameObject other)
        {

        }

        public virtual void OnTriggerExit(GameObject other)
        {

        }

        public virtual System.Xml.Schema.XmlSchema GetSchema() { return null; }
        public virtual void ReadXml(System.Xml.XmlReader reader)
        {
            //MyObject = ResourceManager.Instance.CurrentScene.GetObject((uint)reader.ReadElementContentAsInt("MyObjectID", ""));
            tmp = (uint)reader.ReadElementContentAsInt("MyObjectID", "");
        }
        public virtual void WriteXml(System.Xml.XmlWriter writer)
        {
            writer.WriteElementString("MyObjectID", MyObject.UniqueID.ToString());
        }

        public ObjectComponent GetComponent<T>()
        {
            return this.MyObject.GetComponent<T>();
        }

        public List<ObjectComponent> GetComponents<T>()
        {
            return this.MyObject.GetComponents<T>();
        }

        #endregion
    }
}
