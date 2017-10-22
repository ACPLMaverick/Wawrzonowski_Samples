using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay
{
    public class Stairs : ObjectComponent
    {
        PlayerController pc = null;

        public Stairs(GameObject go) : base(go)
        {

        }

        public Stairs(GameObject go, Stairs s) : base(go)
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

        public override void OnTriggerEnter(GameObject other)
        {
            if(other.UniqueID == 1)
            {
                if(pc == null)
                {
                    pc = (PlayerController)other.GetComponent<PlayerController>();
                }
                if (pc != null)
                {
                    pc.StairsTouching += 1;
                }
                Vector3 position = other.MyTransform.Position;
                position.Y = this.MyObject.MyTransform.Position.Y + 0.505f;
                other.MyTransform.Position = position;
            }
            else
            {
                Enemy e = (Enemy)other.GetComponent<Enemy>();
                if(e != null)
                {
                    e.StairsTouching += 1;
                    Vector3 position = other.MyTransform.Position;
                    if (e.GetComponent<Rat>() != null)
                    {
                        position.Y = this.MyObject.MyTransform.Position.Y + 1.1f;
                    }
                    else
                    {
                        position.Y = this.MyObject.MyTransform.Position.Y + 0.5f;
                    }
                    other.MyTransform.Position = position;
                }
            }
            base.OnTriggerEnter(other);
        }

        public override void OnTriggerExit(GameObject other)
        {
            if(other.UniqueID == 1)
            {
                if (pc != null)
                {
                    pc.StairsTouching -= 1;
                }
            }
            else
            {
                Enemy e = (Enemy)other.GetComponent<Enemy>();
                if (e != null)
                {
                    e.StairsTouching -= 1;
                }
            }
            base.OnTriggerExit(other);
        }

        public override void ReadXml(System.Xml.XmlReader reader)
        {
            reader.ReadStartElement();

            base.ReadXml(reader);

            reader.ReadEndElement();
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            base.WriteXml(writer);
        }
    }
}
