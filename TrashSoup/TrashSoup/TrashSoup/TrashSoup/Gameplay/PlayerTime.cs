using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Serialization;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay
{
    public class PlayerTime : ObjectComponent, IXmlSerializable
    {
        #region variables
        private int initHours = 0;
        private int initMinutes = 0;
        private double multiplier = 60;
        private double initMillis = 0;
        private double millis = 0;

        private Vector2 timePos = new Vector2(0.05f, 0.8f);
        #endregion

        #region properties
        public int Hours
        {
            get 
            {
                return TotalHours % 24;
            }
        }

        public int Minutes
        {
            get 
            {
                return TotalMinutes % 60;
            }
        }

        public int Seconds
        {
            get
            {
                return TotalSeconds % 60;
            }
        }

        public long Milliseconds
        {
            get
            {
                return TotalMilliseconds % 1000;
            }
        }

        public int TotalHours
        {
            get
            {
                return (int)(millis / 3600000);
            }
        }

        public int TotalMinutes
        {
            get
            {
                return (int)(millis / 60000);
            }
        }

        public int TotalSeconds
        {
            get
            {
                return (int)(millis / 1000);
            }
        }

        public long TotalMilliseconds
        {
            get
            {
                return (long)millis;
            }
            set
            {
                millis = (double)value;
            }
        }

        public double Multiplier
        {
            get { return multiplier; }
            set { multiplier = value; }
        }

        public int InitHours 
        { 
            get
            {
                return initHours;
            }
            set
            {
                double current = ((double)initHours) * 60 * 60 * 1000;
                initMillis -= current;
                initHours = value;
                current = ((double)initHours) * 60 * 60 * 1000;
                initMillis += current;
                millis = initMillis;
            }
        }
        public int InitMinutes 
        { 
            get
            {
                return initMinutes;
            }
            set
            {
                double current = ((double)initMinutes) * 60 * 1000;
                initMillis -= current;
                initMinutes = value;
                current = ((double)initMinutes) * 60 * 1000;
                initMillis += current;
                millis = initMillis;
            }
        }
        public double InitMillis 
        { 
            get
            {
                return initMillis;
            }
            set
            {
                initMillis = value;
                initHours = (int)(initMillis / (1000 * 60 * 60)) % 24;
                initMinutes = (int)(initMillis / (1000 * 60)) % 60;
                millis = initMillis;
            }
        }

        public bool Stopped { get; set; }
        #endregion

        #region methods
        public PlayerTime(GameObject obj) : base(obj)
        {
            InitHours = 12;
            InitMinutes = 0;
            this.millis = InitMillis;
            this.Stopped = false;
        }

        public PlayerTime(GameObject obj, int initHours, int initMinutes) : base(obj)
        {
            InitHours = initHours;
            InitMinutes = initMinutes;
            this.millis = InitMillis;
            this.Stopped = false;
        }

        public override void Update(GameTime gameTime)
        {
            if(!Stopped)
                millis += gameTime.ElapsedGameTime.TotalMilliseconds * multiplier;

            GUIManager.Instance.DrawText(TrashSoupGame.Instance.Content.Load<SpriteFont>("Fonts/digital-7"), Hours.ToString("00") + ":" + Minutes.ToString("00"), this.timePos, Color.Red);
        }

        public override void Draw(Camera cam, Effect effect, GameTime gameTime)
        {

	    }

        protected override void Start()
        {
            // do nothing
        }

        public override void Initialize()
        {
            if(SaveManager.Instance.XmlPath.Contains("safehouse"))
            {
                int[] time = Safehouse.SafehouseController.Instance.GetExitTime();
                if(time[0] < 21)
                {
                    this.InitHours = 21;
                    this.InitMinutes = 0;
                }
                else if(time[0] == 21)
                {
                    if(time[1] == 0)
                    {
                        this.InitHours = 21;
                        this.InitMinutes = 0;
                    }
                }
                else
                {
                    this.InitHours = 22;
                    this.InitMinutes = 0;
                }
            }
            base.Initialize();
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

            InitHours = reader.ReadElementContentAsInt("initHours", "");
            InitMinutes = reader.ReadElementContentAsInt("initMinutes", "");
            Multiplier = reader.ReadElementContentAsInt("Multiplier", "");

            reader.ReadEndElement();
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            base.WriteXml(writer);

            writer.WriteElementString("initHours", XmlConvert.ToString(initHours));
            writer.WriteElementString("initMinutes", XmlConvert.ToString(initMinutes));
            writer.WriteElementString("Multiplier", XmlConvert.ToString(Multiplier));
        }

        #endregion
    }
}
