﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System.Xml.Serialization;
using System.Xml;

namespace TrashSoup.Engine
{
    public class GameObject : IXmlSerializable
    {
        #region variables

        private bool visible;
        private bool dynamic;

        private PhysicalObject myPhisicalObject;
        private Socket myCarrierSocket;

        private GameObject parent;
        private List<GameObject> children;

        private List<ObjectComponent> runtimeAdded;
        private List<ObjectComponent> runtimeRemoved;

        private bool drawCollider = false;
        private bool drawLast = false;

        #endregion

        #region properties
        public uint UniqueID { get; protected set; }
        public string Name { get; set; }
        public List<string> Tags { get; set; }
        public bool Enabled { get; set; }
        public bool Visible 
        { 
            get
            {
                return visible;
            }
            set
            {
                visible = value;
                ChildrenVisible = value;
            }
        }

        public bool DrawCollider
        {
            protected get { return this.drawCollider; }
            set { this.drawCollider = value; }
        }

        public bool DrawLast
        {
            get { return drawLast; }
            set { drawLast = value; }
        }

        /// <summary>
        /// The value of this property is also modified by Visible property!!!
        /// </summary>
        public bool ChildrenVisible { get; set; }
        public bool Dynamic 
        { 
            get
            {
                return dynamic;
            }
            set
            {
                if(value != dynamic)
                {
                    dynamic = value;
                    Scene myScene = ResourceManager.Instance.CurrentScene;
                    if(myScene != null && myScene.ObjectsDictionary != null && myScene.ObjectsQT != null && myScene.Params != null && myScene.Params.UseGraph && 
                        myScene.ObjectsDictionary.Contains(new KeyValuePair<uint,GameObject>(UniqueID, this)))
                    {
                        if (dynamic == true)
                            myScene.ObjectsQT.AddDynamic(this);
                        else
                            myScene.ObjectsQT.RemoveDynamic(this);
                    }
                }
            }
        }

        public Transform MyTransform { get; set; }
        public Animator MyAnimator { get; set; }
        public Collider MyCollider { get; set; }

        public Socket MyCarrierSocket
        { 
            get
            {
                return myCarrierSocket;
            }
            set
            {
                if(myCarrierSocket != null && value != null)
                {
                    Debug.Log("GameObject WARNING! " + this.Name + ", ID " + this.UniqueID.ToString() + " already had assigned socket!");
                }
                if(myCarrierSocket == value && value != null)
                {
                    Debug.Log("GameObject ERROR: " + this.Name + ", ID " + this.UniqueID.ToString() + " is already assigned to this socket! Aborting.");
                    return;
                }
                if(value != null)
                    value.Carrier.AddComponentRuntime(value);
                myCarrierSocket = value;
            }
        }

        public QuadTreeNode MyNode { get; set; }

        public PhysicalObject MyPhysicalObject 
        {
            get
            {
                return this.myPhisicalObject;
            }
            set
            {
                if(value == null)
                {
                    PhysicsManager.Instance.RemovePhysicalObject(this);
                }
                else
                {
                    PhysicsManager.Instance.AddPhysicalObject(this);
                }

                this.myPhisicalObject = value;
            }
        }

        public List<ObjectComponent> Components { get; set; }
        public GraphicsDeviceManager GraphicsManager { get; protected set; }
        public List<LightPoint> LightsAffecting { get; protected set; }

        #endregion

        #region events

        public delegate void OnTriggerEventHandler(object sender, CollisionEventArgs e);
        public delegate void OnTriggerEnterEventHandler(object sender, CollisionEventArgs e);
        public delegate void OnTriggerExitEventHandler(object sender, CollisionEventArgs e);

        public event OnTriggerEventHandler OnTriggerEvent;
        public event OnTriggerEnterEventHandler OnTriggerEnterEvent;
        public event OnTriggerExitEventHandler OnTriggerExitEvent;

        #endregion

        #region methods
        public GameObject(uint uniqueID, string name)
        {
            this.UniqueID = uniqueID;
            this.Name = name;

            this.Components = new List<ObjectComponent>();
            this.LightsAffecting = new List<LightPoint>();
            this.children = new List<GameObject>();
            this.parent = null;
            this.GraphicsManager = TrashSoupGame.Instance.GraphicsManager;

            this.runtimeAdded = new List<ObjectComponent>();
            this.runtimeRemoved = new List<ObjectComponent>();

            this.Enabled = true;
            this.Visible = true;
            this.Tags = new List<string>();
        }

        public void Initialize()
        {
            int cCount = children.Count;
            for (int i = 0; i < cCount; ++i )
            {
                children[i].Initialize();
            }

                foreach (ObjectComponent component in this.Components)
                {
                    component.Initialize();
                }
            if (this.MyCollider != null)
            {
                this.MyCollider.Initialize();
            }
            if (this.MyAnimator != null)
            {
                this.MyAnimator.Initialize();
            }
            if (this.MyPhysicalObject != null)
            {
                this.MyPhysicalObject.Initialize();
            }
            if (this.MyTransform != null)
            {
                this.MyTransform.Initialize();
            }
        }

        public virtual void Destroy()
        {
            foreach(ObjectComponent comp in Components)
            {
                comp.Destroy();
            }
        }

        public virtual void Update(GameTime gameTime)
        {
            if(this.Enabled && !ResourceManager.Instance.ImmediateStop)
            {
                int cCount = children.Count;
                for (int i = 0; i < cCount; ++i)
                {
                    children[i].Update(gameTime);
                }

                if(this.MyTransform != null)
                {
                    this.MyTransform.Update(gameTime);
                }

                if (this.MyPhysicalObject != null)
                {
                    this.MyPhysicalObject.Update(gameTime);
                }

                if (this.MyCollider != null)
                {
                    this.MyCollider.Update(gameTime);
                }

                if (this.MyAnimator != null)
                {
                    this.MyAnimator.Update(gameTime);
                }

                foreach (ObjectComponent obj in Components)
                {
                    obj.Update(gameTime);
                }

                if (runtimeRemoved.Count > 0 || runtimeAdded.Count > 0)
                {
                    SolveRuntimeAdditions();
                }
            }
        }

        public virtual void Draw(Camera cam, Effect effect, GameTime gameTime)
        {
            if (this.Visible && this.Enabled && !ResourceManager.Instance.ImmediateStop)
            {
                foreach (ObjectComponent obj in Components)
                {
                    obj.Draw(cam, effect, gameTime);
                }

                //[vansten] This is for debug drawing of collider
                //[vansten] It won't be build if we build a release version
                //[vansten] COMMENT DRAWING COLLIDER TO GET HIGHER FPS RATE!!!!!!!!!!!!!
#if DEBUG
                if (this.MyCollider != null && this.DrawCollider)
                {
                   this.MyCollider.Draw(cam, effect, gameTime);
                }

#else
                if (this.MyCollider != null && this.DrawCollider && TrashSoupGame.Instance.EditorMode)
                {
                    this.MyCollider.Draw(cam, effect, gameTime);
                }
#endif
            }

            if(this.ChildrenVisible && this.Enabled)
            {
                int cCount = children.Count;
                for (int i = 0; i < cCount; ++i)
                {
                    children[i].Draw(cam, effect, gameTime);
                }
            }
        }

        public void SetParent(GameObject parent)
        {
            this.parent = parent;
        }

        public GameObject GetParent()
        {
            return parent;
        }

        public List<GameObject> GetChildren()
        {
            return children;
        }

        public void AddChild(GameObject obj)
        {
            if(children.Contains(obj))
            {
                Debug.Log("GameObject ERRROR: Child is already here!");
                return;
            }
            this.children.Add(obj);
            obj.SetParent(this);

            if(this.Dynamic && !obj.Dynamic)
            {
                obj.Dynamic = true;
            }
        }

        public void RemoveChild(GameObject obj)
        {
            if (!children.Contains(obj))
            {
                Debug.Log("GameObject ERRROR: Child is not here!");
                return;
            }

            int cCount = children.Count;
            for (int i = 0; i < cCount; ++i)
            {
                if (children[i] == obj)
                {
                    int ccCount = children[i].children.Count;
                    for (int j = 0; j < ccCount; ++j)
                    {
                        this.children.Add(children[i].children[j]);
                        children[i].children[j].parent = this;
                    }
                    children.Remove(obj);
                }
            }
        }

        public void RemoveChildAddHigher(GameObject obj)
        {
            if (!children.Contains(obj))
            {
                Debug.Log("GameObject ERRROR: Child is not here!");
                return;
            }

            int cCount = children.Count;
            for (int i = 0; i < cCount; ++i)
            {
                if (children[i] == obj)
                {
                    if (parent != null)
                    {
                        parent.children.Add(obj);
                        obj.parent = parent;
                    }
                    else
                    {
                        ResourceManager.Instance.CurrentScene.AddObjectRuntime(obj);
                        obj.parent = null;
                    }
                }
            }
            for(int i = 0; i < cCount; ++i)
            {
                children.Remove(obj);
            }
        }

        public void RemoveChildWithoutHierarchy(GameObject obj)
        {
            if (!children.Contains(obj))
            {
                Debug.Log("GameObject ERRROR: Child is not here!");
                return;
            }

            int cCount = children.Count;
            for(int i = 0; i < cCount; ++i)
            {
                if(children[i] == obj)
                {
                    children.Remove(obj);
                }
            }
        }

        public void AddComponentRuntime(ObjectComponent obj)
        {
            this.runtimeAdded.Add(obj);
        }

        public void RemoveComponentRuntime(ObjectComponent obj)
        {
            if(runtimeAdded.Contains(obj))
            {
                this.runtimeAdded.Remove(obj);
            }
            else
            {
                this.runtimeRemoved.Add(obj);
            }
        }

        private void SolveRuntimeAdditions()
        {
            int aCount = runtimeAdded.Count;
            int rCount = runtimeRemoved.Count;

            for (int i = 0; i < aCount; ++i)
            {
                this.Components.Add(this.runtimeAdded[i]);
            }
            for (int i = 0; i < rCount; ++i)
            {
                this.Components.Remove(this.runtimeRemoved[i]);
            }

            runtimeAdded.Clear();
            runtimeRemoved.Clear();
        }

        public System.Xml.Schema.XmlSchema GetSchema()
        {
            return null;
        }

        public void ReadXml(System.Xml.XmlReader reader)
        {
            reader.MoveToContent();
            //reader.ReadStartElement();

            UniqueID = (uint)reader.ReadElementContentAsInt("UniqueID", "");
            Name = reader.ReadElementString("Name", "");
            DrawLast = reader.ReadElementContentAsBoolean("DrawLast", "");

            if (reader.Name == "Tags")
            {
                reader.ReadStartElement();
                Tags = new List<string>();
                while (reader.NodeType != XmlNodeType.EndElement)
                {
                    string s = reader.ReadElementString("Tag", "");
                    if(s != "null")
                        Tags.Add(s);
                }
                reader.ReadEndElement();
            }
            Dynamic = reader.ReadElementContentAsBoolean("Dynamic", "");

            if(reader.Name == "MyTransform")
            {
                MyTransform = new Transform(this);
                (MyTransform as IXmlSerializable).ReadXml(reader);
            }

            if(reader.Name == "MyPhysicalObject")
            {
                MyPhysicalObject = new PhysicalObject(this);
                (MyPhysicalObject as IXmlSerializable).ReadXml(reader);
            }

            //if (reader.Name == "MyCollider")
            //{
            //    reader.ReadStartElement();
            //    String s = reader.ReadElementString("Type", "");
            //    switch(s)
            //    {
            //        case "TrashSoup.Engine.BoxCollider":
            //            MyCollider = new BoxCollider(this);
            //            break;
            //        case "TrashSoup.Engine.SphereCollider":
            //            MyCollider = new SphereCollider(this);
            //            break;
            //        default:
            //            MyCollider = new Collider(this);
            //            break;
            //    }
            //    (MyCollider as IXmlSerializable).ReadXml(reader);
            //    reader.ReadEndElement();
            //}

            if(reader.Name == "MyAnimator")
            {
                reader.ReadStartElement();
                Model baseAnim = null;
                string baseAnimPath = reader.ReadElementString("BaseAnim", "");
                if(!ResourceManager.Instance.Models.TryGetValue(baseAnimPath, out baseAnim))
                {
                    baseAnim = ResourceManager.Instance.LoadModel(baseAnimPath);
                }
                MyAnimator = new Animator(this, ResourceManager.Instance.Models[baseAnimPath]);
                (MyAnimator as IXmlSerializable).ReadXml(reader);
                //MyAnimator.MyObject = ResourceManager.Instance.CurrentScene.GetObject(MyAnimator.tmp);
                while (reader.NodeType != System.Xml.XmlNodeType.EndElement)
                {
                    String s = reader.ReadElementString("AnimatorClip", "");
                    MyAnimator.AddAnimationClip(ResourceManager.Instance.LoadAnimationFromModel(
                        ResourceManager.Instance.Models[baseAnimPath],
                        ResourceManager.Instance.LoadAnimation(s),
                        s
                        ));
                    //MyAnimator.animationPlayers.Add(s, new SkinningModelLibrary.AnimationPlayer(MyAnimator.SkinningData, s));
                }
                reader.ReadEndElement();
            }

            if(reader.Name == "Components")
            {
                List<object> parameters = new List<object>();
                parameters.Add(this);
                reader.MoveToContent();
                reader.ReadStartElement();
                while (reader.NodeType != System.Xml.XmlNodeType.EndElement)
                {
                    Object obj = Activator.CreateInstance(Type.GetType(reader.Name), parameters.ToArray());
                    (obj as IXmlSerializable).ReadXml(reader);
                    Components.Add((ObjectComponent)obj);
                }


            }
            if(reader.NodeType == System.Xml.XmlNodeType.EndElement)
            reader.ReadEndElement();

            if (reader.Name == "MyCollider")
            {
                reader.ReadStartElement();
                String s = reader.ReadElementString("Type", "");
                switch (s)
                {
                    //commented because Collider system will be changed
                    case "TrashSoup.Engine.BoxCollider":
                        MyCollider = new BoxCollider(this);
                        break;
                    case "TrashSoup.Engine.SphereCollider":
                        MyCollider = new SphereCollider(this);
                        break;
                    default:
                        //MyCollider = new Collider(this);
                        break;
                }
                (MyCollider as IXmlSerializable).ReadXml(reader);
                reader.ReadEndElement();
            }

            //this.Initialize();

            //reader.ReadEndElement();
        }

        public void WriteXml(System.Xml.XmlWriter writer)
        {
            writer.WriteElementString("UniqueID", UniqueID.ToString());
            writer.WriteElementString("Name", Name);
            writer.WriteElementString("DrawLast", XmlConvert.ToString(DrawLast));

            writer.WriteStartElement("Tags");
            if (Tags != null && Tags.Count > 0)
            {
                foreach (string tag in Tags)
                {
                    writer.WriteElementString("Tag", tag);
                }
            }
            else if(Tags == null || Tags.Count == 0)
            {
                writer.WriteElementString("Tag", "null");
            }
            writer.WriteEndElement();

            writer.WriteElementString("Dynamic", XmlConvert.ToString(Dynamic));

            if(MyTransform != null)
            {
                writer.WriteStartElement("MyTransform");
                (MyTransform as IXmlSerializable).WriteXml(writer);
                writer.WriteEndElement();
            }

            if(MyPhysicalObject != null)
            {
                writer.WriteStartElement("MyPhysicalObject");
                (MyPhysicalObject as IXmlSerializable).WriteXml(writer);
                writer.WriteEndElement();
            }

            if(MyAnimator != null)
            {
                writer.WriteStartElement("MyAnimator");
                writer.WriteElementString("BaseAnim", ResourceManager.Instance.Models.FirstOrDefault(x => x.Value == MyAnimator.BaseAnim).Key);
                (MyAnimator as IXmlSerializable).WriteXml(writer);
                foreach (KeyValuePair<string, SkinningModelLibrary.AnimationPlayer> pair in MyAnimator.animationPlayers)
                {
                    writer.WriteElementString("AnimatorClip", pair.Key);
                }
                writer.WriteEndElement();
            }

            if(Components.Count != 0)
            {
                writer.WriteStartElement("Components");
                foreach (ObjectComponent comp in Components)
                {
                    if(comp != null)
                    {
                        writer.WriteStartElement(comp.GetType().ToString());
                        (comp as IXmlSerializable).WriteXml(writer);
                        writer.WriteEndElement();
                    }
                }
                writer.WriteEndElement();
            }

            if (MyCollider != null)
            {
                writer.WriteStartElement("MyCollider");
                writer.WriteElementString("Type", MyCollider.GetType().ToString());
                (MyCollider as IXmlSerializable).WriteXml(writer);
                writer.WriteEndElement();
            }

        }

        /// <summary>
        /// 
        /// This function will call every OnTrigger(GameObject) in this game object components
        /// </summary>
        public virtual void OnTrigger(GameObject otherGO)
        {
            if (OnTriggerEvent != null) OnTriggerEvent(this, new CollisionEventArgs(otherGO));

            foreach(ObjectComponent oc in this.Components)
            {
                oc.OnTrigger(otherGO);
            }
        }

        public virtual void OnTriggerEnter(GameObject otherGO)
        {
            if (OnTriggerEnterEvent != null) OnTriggerEnterEvent(this, new CollisionEventArgs(otherGO));

            //Debug.Log(this.Name + " trigger enter with " + otherGO.Name);
            foreach (ObjectComponent oc in this.Components)
            {
                oc.OnTriggerEnter(otherGO);
            }
        }

        public virtual void OnTriggerExit(GameObject otherGO)
        {
            if (OnTriggerExitEvent != null) OnTriggerExitEvent(this, new CollisionEventArgs(otherGO));

            //Debug.Log(this.Name + " trigger exit with " + otherGO.Name);
            foreach (ObjectComponent oc in this.Components)
            {
                oc.OnTriggerExit(otherGO);
            }
        }

        /// <summary>
        /// 
        /// This function will call every OnCollision(GameObject) in this game object components
        /// </summary>
        public virtual void OnCollision(GameObject otherGO)
        {
            foreach (ObjectComponent oc in this.Components)
            {
                oc.OnCollision(otherGO);
            }
        }

        public ObjectComponent GetComponent<T>()
        {
            System.Type t = typeof(T);

            if(t == typeof(Transform) && this.MyTransform != null)
            {
                return this.MyTransform;
            }

            if(t == typeof(Animator) && this.MyAnimator != null)
            {
                return this.MyAnimator;
            }

            if(t == typeof(Collider) && this.MyCollider != null)
            {
                return this.MyCollider;
            }

            if(t == typeof(PhysicalObject) && this.MyPhysicalObject != null)
            {
                return this.MyPhysicalObject;
            }

            foreach(ObjectComponent oc in this.Components)
            {
                if(oc.GetType() == t)
                {
                    return oc;
                }
            }

            return null;
        }

        public List<ObjectComponent> GetComponents<T>()
        {
            List<ObjectComponent> componentsList = new List<ObjectComponent>();

            System.Type t = typeof(T);

            if (t == typeof(Transform) && this.MyTransform != null)
            {
                componentsList.Add(this.MyTransform);
            }

            if (t == typeof(Animator) && this.MyAnimator != null)
            {
                componentsList.Add(this.MyAnimator);
            }

            if (t == typeof(Collider) && this.MyCollider != null)
            {
                componentsList.Add(this.MyCollider);
            }

            if (t == typeof(PhysicalObject) && this.MyPhysicalObject != null)
            {
                componentsList.Add(this.MyPhysicalObject);
            }

            foreach (ObjectComponent oc in this.Components)
            {
                if (oc.GetType() == t)
                {
                    componentsList.Add(oc);
                }
            }

            return componentsList;
        }

        #endregion

    }
}