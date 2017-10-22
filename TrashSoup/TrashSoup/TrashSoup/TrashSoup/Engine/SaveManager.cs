using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using Microsoft.Xna.Framework;

namespace TrashSoup.Engine
{
    [XmlRoot("Save")]
    public class SaveManager : Singleton<SaveManager>, IXmlSerializable
    {
        #region variables
        public Scene scene;
        public String XmlPath { get; set; }

        public Action OnLevelBeginLoad;
        #endregion

        #region methods
        public SaveManager()
        {
            this.scene = ResourceManager.Instance.CurrentScene;
            //FOR TETIN
            this.XmlPath = "menu.xml";
        }

        public System.Xml.Schema.XmlSchema GetSchema()
        {
            return null;
        }

        public void ReadXml(System.Xml.XmlReader reader)
        {
            reader.MoveToContent();
            reader.ReadStartElement();

            if (reader.Name == "Scene")
            {
                (scene as IXmlSerializable).ReadXml(reader);
            }
        }

        public void WriteXml(System.Xml.XmlWriter writer)
        {
            writer.WriteStartElement("Scene");
            (scene as IXmlSerializable).WriteXml(writer);
            writer.WriteEndElement();
        }

        public void SaveFileAction()
        {
            if(TrashSoupGame.Instance.EditorMode)
            {
                this.scene = ResourceManager.Instance.CurrentScene;
                if (this.scene != null)
                {
                    foreach (GameObject obj in this.scene.ObjectsDictionary.Values)
                    {
                        obj.Destroy();
                    }
                }
            }
            if(scene != null)
            {
                XmlSerializer serializer = new XmlSerializer(typeof(Scene));
                using (FileStream file = new FileStream(this.XmlPath, FileMode.Create))
                {
                    serializer.Serialize(file, ResourceManager.Instance.CurrentScene);
                }
            }
        }

        public void LoadFileAction()
        {
            if(OnLevelBeginLoad != null)
            {
                OnLevelBeginLoad();
            }
            OnLevelBeginLoad = null;
            ResourceManager.Instance.UnloadContent();
            PhysicsManager.Instance.Reload();
            Debug.Log("Loading file: " + this.XmlPath);
            XmlSerializer serializer = new XmlSerializer(typeof(Scene));
            using(FileStream file = new FileStream(this.XmlPath, FileMode.Open))
            {
                //this.scene = new Scene();
                //Scene tmp = (Scene)serializer.Deserialize(file);
                ResourceManager.Instance.CurrentScene = new Scene();
                ResourceManager.Instance.CurrentScene = (Scene)serializer.Deserialize(file);
                if (ResourceManager.Instance.CurrentScene.Params.UseGraph)
                {
                    foreach (KeyValuePair<uint, GameObject> val in ResourceManager.Instance.CurrentScene.ObjectsDictionary)
                    {
                        ResourceManager.Instance.CurrentScene.ObjectsQT.Add(val.Value);
                    }
                }
            }

            foreach(GameObject go in ResourceManager.Instance.CurrentScene.ObjectsDictionary.Values)
            {
                go.Initialize();
            }
        }

        public void EditorLoadFileAction(string filepath)
        {
            this.XmlPath = filepath;
            LoadFileAction();
        }

        public void EditorSaveFileAction(string filepath)
        {
            this.XmlPath = filepath;
            SaveFileAction();
        }

        public void GetXmlPath()
        {
            if(!File.Exists("config.txt"))
            {
                using(FileStream fs = new FileStream("config.txt", FileMode.CreateNew))
                {
                    using(StreamWriter writer = new StreamWriter(fs))
                    {
                        writer.Write(this.XmlPath);
                    }
                    Debug.Log("config.txt created");
                }
            }
            using (StreamReader reader = new StreamReader("config.txt"))
            {
                this.XmlPath = reader.ReadToEnd();
                Debug.Log("Actual scene file: " + this.XmlPath);
            }
        }

        public void SaveState(int trashCount)
        {
            using (FileStream fs = new FileStream("user_save.txt", FileMode.Create))
            {
                using (StreamWriter writer = new StreamWriter(fs))
                {
                    writer.Write(trashCount);
                }
                Debug.Log("State saved");
            }
        }

        public int LoadState()
        {
            int output;
            using (StreamReader reader = new StreamReader("user_save.txt"))
            {
                output = Convert.ToInt32(reader.ReadToEnd());
                Debug.Log("Actual trash count: " + output);
                return output;
            }
        }

        #endregion
    }
}
