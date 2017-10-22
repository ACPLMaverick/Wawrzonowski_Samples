using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;

namespace TrashSoup.Gameplay
{
    public class PlayerAttackTrigger : ObjectComponent
    {
        private List<Enemy> enemiesInTrigger = new List<Enemy>();
        private PlayerController pc;

        public delegate void AttackEventHandler(object o, CollisionEventArgs e);
        public event AttackEventHandler AttackEvent;

        public PlayerAttackTrigger(GameObject go) : base(go)
        {

        }

        public PlayerAttackTrigger(GameObject go, PlayerAttackTrigger pat) : base(go)
        {
            this.enemiesInTrigger = pat.enemiesInTrigger;
        }

        public PlayerAttackTrigger(GameObject go, PlayerController pc) : base(go)
        {
            this.pc = pc;
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

        public void Attack(int damage, int damageMultiplier = 1)
        {
            foreach(Enemy e in this.enemiesInTrigger)
            {
                if (e.IsDead)
                {
                    continue;
                }
                if(AttackEvent != null)
                {
                    AttackEvent(this, new CollisionEventArgs(e.MyObject));
                }

                int prevHitPoints = e.HitPoints;
                e.HitPoints -= damage * damageMultiplier;
                if(prevHitPoints != e.HitPoints)
                {
                    pc.AddPopularity();
                    ParticleSystem ps = e.MyObject.GetComponent<ParticleSystem>() as ParticleSystem;
                    if (ps != null)
                    {
                        ps.Play();
                    }
                    if (e.HitPoints > 0 && e.MyObject.MyPhysicalObject != null)
                    {
                        Vector3 diff = e.MyObject.MyTransform.Position - pc.MyObject.MyTransform.Position;
                        diff.Y = 0.0f;
                        diff.Normalize();
                        float force = 100.0f;
                        if (pc.Equipment.CurrentWeapon.Type != WeaponType.FISTS)
                        {
                            force = 120.0f;
                        }
                        e.MyObject.MyPhysicalObject.ZeroForce();
                        e.MyObject.MyPhysicalObject.Velocity = Vector3.Zero;
                        e.MyObject.MyPhysicalObject.AddForce(diff * force);
                        Rat eRat = (Rat)e.GetComponent<Rat>();
                        if (eRat != null)
                        {
                            eRat.MyBlackBoard.SetBool("TargetSeen", true);
                            eRat.MyBlackBoard.SetVector3("TargetPosition", this.pc.MyObject.MyTransform.Position);
                        }
                        else
                        {
                            Mutant eMutant = (Mutant)e.GetComponent<Mutant>();
                            if (eMutant != null)
                            {
                                eMutant.MyBlackBoard.SetBool("TargetSeen", true);
                                eMutant.MyBlackBoard.SetVector3("TargetPosition", this.pc.MyObject.MyTransform.Position);
                            }
                        }
                    }
                }
            }
        }

        public override void OnTriggerEnter(GameObject other)
        {
            Enemy e = (Enemy)other.GetComponent<Enemy>();
            if(e != null)
            {
                if(!this.enemiesInTrigger.Contains(e))
                {
                    this.enemiesInTrigger.Add(e);
                }
            }
            base.OnTriggerEnter(other);
        }

        public override void OnTriggerExit(GameObject other)
        {
            Enemy e = (Enemy)other.GetComponent<Enemy>();
            if (e != null)
            {
                if (this.enemiesInTrigger.Contains(e))
                {
                    this.enemiesInTrigger.Remove(e);
                }
            }
            base.OnTriggerEnter(other);
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
