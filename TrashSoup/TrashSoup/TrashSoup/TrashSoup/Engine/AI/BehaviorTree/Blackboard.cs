using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Serialization;

namespace TrashSoup.Engine.AI.BehaviorTree
{
    public class Blackboard : IXmlSerializable
    {
        private Dictionary<string, int> intObjects = new Dictionary<string, int>();
        private Dictionary<string, float> floatObjects = new Dictionary<string, float>();
        private Dictionary<string, bool> boolObjects = new Dictionary<string, bool>();
        private Dictionary<string, Vector3> vectorObjects = new Dictionary<string, Vector3>();

        public GameObject Owner
        {
            get;
            set;
        }

        public Blackboard()
        {
            this.Owner = null;
        }

        public Blackboard(GameObject owner)
        {
            this.Owner = owner;
        }

        public int GetInt(string id)
        {
            if (intObjects.ContainsKey(id))
            {
                return intObjects[id];
            }

            return 0;
        }

        public float GetFloat(string id)
        {
            if (floatObjects.ContainsKey(id))
            {
                return floatObjects[id];
            }

            return 0.0f;
        }

        public bool GetBool(string id)
        {
            if (boolObjects.ContainsKey(id))
            {
                return boolObjects[id];
            }

            return false;
        }

        public Vector3 GetVector3(string id)
        {
            if(this.vectorObjects.ContainsKey(id))
            {
                return this.vectorObjects[id];
            }

            return Vector3.Zero;
        }

        public void SetInt(string id, int value)
        {
            if (this.intObjects.ContainsKey(id))
            {
                this.intObjects[id] = value;
            }
            else
            {
                this.intObjects.Add(id, value);
            }
        }

        public void SetFloat(string id, float value)
        {
            if (this.floatObjects.ContainsKey(id))
            {
                this.floatObjects[id] = value;
            }
            else
            {
                this.floatObjects.Add(id, value);
            }
        }

        public void SetBool(string id, bool value)
        {
            if (this.boolObjects.ContainsKey(id))
            {
                this.boolObjects[id] = value;
            }
            else
            {
                this.boolObjects.Add(id, value);
            }
        }

        public void SetVector3(string id, Vector3 value)
        {
            if (this.vectorObjects.ContainsKey(id))
            {
                this.vectorObjects[id] = value;
            }
            else
            {
                this.vectorObjects.Add(id, value);
            }
        }

        public System.Xml.Schema.XmlSchema GetSchema()
        {
            return null;
        }

        public void ReadXml(System.Xml.XmlReader reader)
        {
            reader.ReadStartElement();

            if (reader.Name == "intObjects")
            {
                reader.ReadStartElement();
                while (reader.NodeType != System.Xml.XmlNodeType.EndElement)
                {
                    string s = reader.Name;
                    int i = reader.ReadElementContentAsInt(s, "");
                    SetInt(s, i);
                }
                reader.ReadEndElement();
            }

            if (reader.Name == "floatObjects")
            {
                reader.ReadStartElement();
                while (reader.NodeType != System.Xml.XmlNodeType.EndElement)
                {
                    string s = reader.Name;
                    float f = reader.ReadElementContentAsFloat(s, "");
                    SetFloat(s, f);
                }
                reader.ReadEndElement();
            }

            if (reader.Name == "boolObjects")
            {
                reader.ReadStartElement();
                while (reader.NodeType != System.Xml.XmlNodeType.EndElement)
                {
                    string s = reader.Name;
                    bool b = reader.ReadElementContentAsBoolean(s, "");
                    SetBool(s, b);
                }
                reader.ReadEndElement();
            }

            if (reader.Name == "vectorObjects")
            {
                reader.ReadStartElement();
                while (reader.NodeType != System.Xml.XmlNodeType.EndElement)
                {
                    string s = reader.Name;
                    string content = reader.ReadElementContentAsString(s, "");
                }
                reader.ReadEndElement();
            }

            reader.ReadEndElement();
        }

        public void WriteXml(System.Xml.XmlWriter writer)
        {
            writer.WriteStartElement("intObjects");
            foreach (int i in intObjects.Values)
            {
                writer.WriteElementString(intObjects.FirstOrDefault(x => x.Value == i).Key, XmlConvert.ToString(i));
            }
            writer.WriteEndElement();

            writer.WriteStartElement("floatObjects");
            foreach (float f in floatObjects.Values)
            {
                writer.WriteElementString(floatObjects.FirstOrDefault(x => x.Value == f).Key, XmlConvert.ToString(f));
            }
            writer.WriteEndElement();

            writer.WriteStartElement("boolObjects");
            foreach (bool b in boolObjects.Values)
            {
                writer.WriteElementString(boolObjects.FirstOrDefault(x => x.Value == b).Key, XmlConvert.ToString(b));
            }
            writer.WriteEndElement();

            writer.WriteStartElement("vectorObjects");
            foreach (Vector3 v in this.vectorObjects.Values)
            {
                writer.WriteElementString(vectorObjects.FirstOrDefault(x => x.Value == v).Key, XmlConvert.ToString(v.X) + " " + XmlConvert.ToString(v.Y) + " " + XmlConvert.ToString(v.Z));
            }
            writer.WriteEndElement();
        }
    }
}
