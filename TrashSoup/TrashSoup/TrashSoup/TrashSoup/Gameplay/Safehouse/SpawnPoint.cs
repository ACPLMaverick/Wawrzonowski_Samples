using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay.Safehouse
{
    public class SpawnPoint : ObjectComponent
    {
        public uint EnemyID;
        private GameObject enemy;

        GameObject player;

        public SpawnPoint(GameObject go) : base(go)
        {

        }

        public SpawnPoint(GameObject go, SpawnPoint sp) : base(go)
        {
            this.EnemyID = sp.EnemyID;
        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
            if (TrashSoupGame.Instance.EditorMode) return;
            if (enemy == null) return;
            foreach (GameObject go in enemy.GetChildren())
            {
                go.MyCollider.Enabled = enemy.Enabled;
            }
        }

        public override void Draw(Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, Microsoft.Xna.Framework.GameTime gameTime)
        {

        }

        protected override void Start()
        {

        }



        public void SpawnEnemies()
        {
            if (enemy == null) return;
            if (enemy.Enabled)
            {
                return;
            }
            player.MyPhysicalObject.IsUsingGravity = false;
            foreach (GameObject go in enemy.GetChildren())
            {
                go.MyCollider.Enabled = true;
            }
            enemy.Enabled = true;
        }

        public override void Initialize()
        {
            enemy = ResourceManager.Instance.CurrentScene.GetObject(EnemyID);
            if(enemy != null)
            {
                enemy.Enabled = false;
                enemy.Dynamic = true;
                enemy.MyTransform.Version = Transform.GameVersionEnum.STENGERT_PAGI;
                enemy.MyTransform.Position = this.MyObject.MyTransform.Position + 0.2f * Vector3.Up;
                enemy.MyTransform.Version = Transform.GameVersionEnum.PBL;
            }

            player = ResourceManager.Instance.CurrentScene.GetObject(1);
            base.Initialize();
        }

        public override void ReadXml(System.Xml.XmlReader reader)
        {
            reader.ReadStartElement();

            reader.ReadStartElement("EnemyID");
            EnemyID = (uint)reader.ReadContentAsInt();
            reader.ReadEndElement();

            base.ReadXml(reader);

            reader.ReadEndElement();
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            writer.WriteStartElement("EnemyID");
            writer.WriteValue(EnemyID);
            writer.WriteEndElement();
            base.WriteXml(writer);
        }
    }
}
