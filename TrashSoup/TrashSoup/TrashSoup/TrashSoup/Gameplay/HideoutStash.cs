using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Schema;
using System.Xml.Serialization;
using TrashSoup.Engine;
using Microsoft.Xna.Framework;

namespace TrashSoup.Gameplay
{
    public class HideoutStash : ObjectComponent, IXmlSerializable
    {
        #region constants

        private const int MAX_TRASH = 2000;

        #endregion

        #region variables

        

        #endregion

        #region properties

        public float CurrentTrashFloat { get; set; }

        public int CurrentTrash
        { 
            get
            {
                return (int)CurrentTrashFloat;
            }
            set
            {
                if(value >= 0 && value <= MAX_TRASH)
                {
                    CurrentTrashFloat = (float)value;
                }
                else
                {
                    Debug.Log("HideoutStash: Trash value is invalid!");
                }
            }
        }

        #endregion

        #region methods

        public HideoutStash(GameObject go) : base(go)
        {

        }

        public HideoutStash(GameObject go, HideoutStash hs) : base(go, hs)
        {
            CurrentTrashFloat = hs.CurrentTrashFloat;
        }

        public override void Update(GameTime gameTime)
        {
            //Debug.Log(CurrentTrashInt.ToString());
        }

        public override void Draw(Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, GameTime gameTime)
        {
            
        }

        protected override void Start()
        {
            
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

            CurrentTrash = reader.ReadElementContentAsInt("CurrentTrash", "");

            reader.ReadEndElement();
        }

        public override void WriteXml(XmlWriter writer)
        {
            base.WriteXml(writer);

            writer.WriteElementString("CurrentTrash", XmlConvert.ToString(CurrentTrash));
        }

        #endregion
        
    }
}
