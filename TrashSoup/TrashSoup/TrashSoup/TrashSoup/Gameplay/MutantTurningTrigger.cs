using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay
{
    public class MutantTurningTrigger : ObjectComponent
    {
        private Mutant myMutant;

        public uint MyMutantID;

        public MutantTurningTrigger(GameObject go)
            : base(go)
        {

        }

        public MutantTurningTrigger(GameObject go, MutantTurningTrigger rtt)
            : base(go, rtt)
        {
            this.myMutant = rtt.myMutant;
            this.MyMutantID = rtt.MyMutantID;
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
            this.myMutant = (Mutant)ResourceManager.Instance.CurrentScene.GetObject(MyMutantID).GetComponent<Mutant>();
            this.myMutant.MyObject.AddChild(this.MyObject);
            this.MyObject.MyTransform.Position = new Microsoft.Xna.Framework.Vector3(2.0f, 0.0f, 0.0f);
            base.Initialize();
        }

        public override void OnTrigger(GameObject other)
        {
            if (this.myMutant.MyBlackBoard == null) return;
            if(other.Tags.Contains("NoRatAllowed"))
            {
                this.myMutant.MyBlackBoard.SetBool("ShouldTurn", true);
            }
            base.OnTrigger(other);
        }

        public override void ReadXml(System.Xml.XmlReader reader)
        {
            reader.ReadStartElement();
            reader.ReadStartElement("MyMutantID");
            this.MyMutantID = (uint)reader.ReadContentAsInt();
            reader.ReadEndElement();

            base.ReadXml(reader);

            reader.ReadEndElement();
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            writer.WriteStartElement("MyMutantID");
            writer.WriteValue(this.MyMutantID);
            writer.WriteEndElement();
            base.WriteXml(writer);
        }
    }
}
