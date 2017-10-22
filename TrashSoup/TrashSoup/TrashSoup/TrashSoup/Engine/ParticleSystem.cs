using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Schema;
using System.Xml.Serialization;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System.Collections;

namespace TrashSoup.Engine
{
    public class ParticleSystem : ObjectComponent, IXmlSerializable
    {
        #region enums

        public enum ParticleRotationMode
        {
            PLAIN,
            DIRECTION_Z,
            DIRECTION_RANDOM
        }

        public enum ParticleLoopMode
        {
            NONE,
            BURST,
            CONTINUOUS
        }

        #endregion

        #region variables
        //Vertex and index buffers
        private VertexBuffer vertices;
        private IndexBuffer indices;

        //Graphics device and effect
        GraphicsDevice graphicsDevice;
        public Effect myEffect;
        public Matrix world;

        //Particles and indices
        private List<Particle> inactiveParticles = new List<Particle>();
        private List<Particle> activeParticles = new List<Particle>();
        private List<Particle> toDelete = new List<Particle>();

        //Time particle was created
        DateTime start;
        private float delayHelper;
        private float lastDelay;

        // propertyz
        private Vector3 offset;

        #endregion

        #region properties

        public ParticleRotationMode RotationMode { get; set; }
        public ParticleLoopMode LoopMode { get; set; }
        public BlendState BlendMode { get; set; }

        public Vector3 Offset 
        { 
            get
            {
                return offset;
            }
            set
            {
                offset = value;

                RandAngle = Vector3.Up + new Vector3(
                -Offset.X + (float)SingleRandom.Instance.rnd.NextDouble() * (Offset.X - (-Offset.X)),
                -Offset.Y + (float)SingleRandom.Instance.rnd.NextDouble() * (Offset.Y - (-Offset.Y)),
                -Offset.Z + (float)SingleRandom.Instance.rnd.NextDouble() * (Offset.Z - (-Offset.Z))
                );
            }
        }
        public Vector3 RandAngle { get; private set; }
        public Vector3 Wind { get; set; }
        public Vector3 WindVariation { get; set; }
        public Vector3 PositionOffset { get; set; }
        public Vector3 PositionOffsetVariation { get; set; }
        public Vector3 ParticleRotation { get; set; }
        public Vector3 ParticleRotationVariation { get; set; }
        public Vector3 ParticleColor { get; set; }
        public Vector3 ParticleColorVariation { get; set; }
        public Vector2 ParticleSize { get; set; }
        public Vector2 ParticleSizeVariation { get; set; }
        public List<Texture2D> Textures { get; set; }
        public float LifespanSec { get; set; }
        public float LifespanSecVariation { get; set; }
        public float FadeInTime { get; set; }
        public float FadeInTimeVariation { get; set; }
        public float FadeOutTime { get; set; }
        public float FadeOutTimeVariation { get; set; }
        public float Speed { get; set; }
        public float SpeedVariation { get; set; }
        public float DelayMs { get; set; }
        public float DelayMsVariation { get; set; }
        public float Mass { get; set; }
        public float MassVariation { get; set; }
        public int ParticleCount { get; set; }
        public bool IgnoreScale { get; set; }

        public bool Stopped { get; set; }
        public bool UseGravity { get; set; }

        #endregion

        #region methods

        public ParticleSystem(GameObject obj) : base(obj)
        {
            Start();
        }

        public ParticleSystem(GameObject obj, ParticleSystem ps) : base(obj, ps)
        {
            RotationMode = ps.RotationMode;
            LoopMode = ps.LoopMode;

            BlendMode = ps.BlendMode;
            Offset = ps.Offset;
            Wind = ps.Wind;
            WindVariation = ps.WindVariation;
            PositionOffset = ps.PositionOffset;
            PositionOffsetVariation = ps.PositionOffsetVariation;
            ParticleRotation = ps.ParticleRotation;
            ParticleRotationVariation = ps.ParticleRotationVariation;
            ParticleColor = ps.ParticleColor;
            ParticleColorVariation = ps.ParticleColorVariation;
            ParticleSize = ps.ParticleSize;
            ParticleSizeVariation = ps.ParticleSizeVariation;
            Textures = ps.Textures;
            LifespanSec = ps.LifespanSec;
            LifespanSecVariation = ps.LifespanSecVariation;
            FadeInTime = ps.FadeInTime;
            FadeInTimeVariation = ps.FadeInTimeVariation;
            FadeOutTime = ps.FadeOutTime;
            FadeOutTimeVariation = ps.FadeOutTimeVariation;
            Speed = ps.Speed;
            SpeedVariation = ps.SpeedVariation;
            DelayMs = ps.DelayMs;
            DelayMsVariation = ps.DelayMsVariation;
            Mass = ps.Mass;
            Mass = ps.MassVariation;
            ParticleCount = ps.ParticleCount;           
            IgnoreScale = ps.IgnoreScale;
            Stopped = ps.Stopped;
            UseGravity = ps.UseGravity;
        }

        protected override void Start()
        {
            RotationMode = ParticleRotationMode.PLAIN;
            LoopMode = ParticleLoopMode.NONE;
            BlendMode = BlendState.AlphaBlend;
            Offset = new Vector3(MathHelper.ToRadians(10.0f));
            Wind = Vector3.Zero;
            WindVariation = Vector3.Zero;
            PositionOffset = Vector3.Zero;
            PositionOffsetVariation = Vector3.Zero;
            ParticleRotation = Vector3.Zero;
            ParticleRotationVariation = Vector3.Zero;
            ParticleColor = Color.White.ToVector3();
            ParticleColorVariation = Vector3.Zero;
            ParticleSize = new Vector2(1.0f, 1.0f);
            ParticleSizeVariation = Vector2.Zero;
            Textures = new List<Texture2D>();
            LifespanSec = 1.0f;
            LifespanSecVariation = 0.0f;
            FadeInTime = 0.0f;
            FadeInTimeVariation = 0.0f;
            FadeOutTime = 0.0f;
            FadeOutTimeVariation = 0.0f;
            Speed = 1.0f;
            SpeedVariation = 0.0f;
            DelayMs = 0.0f;
            DelayMsVariation = 0.0f;
            Mass = 1.0f;
            MassVariation = 0.0f;
            ParticleCount = 400;
            IgnoreScale = true;
            Stopped = false;
            UseGravity = false;
        }

        void GenerateParticles()
        {
            Particle par;

            for(int i = 0; i < ParticleCount; ++i)
            {
                par = new Particle(this);
                par.StartPosition = PositionOffset;
                par.Speed = Vector3.Zero;
                par.DiffuseColor = Color.White.ToVector3();
                //par.Size = ParticleSize + new Vector2(
                //    (float)SingleRandom.Instance.rnd.NextDouble() * ParticleSizeVariation.X * 2 - ParticleSizeVariation.X,
                //    (float)SingleRandom.Instance.rnd.NextDouble() * ParticleSizeVariation.Y * 2 - ParticleSizeVariation.Y);
                par.Size = Vector2.Zero;
                par.Texture = Textures[0];
                par.FadeInMs = FadeInTime * 1000.0f;
                par.FadeOutMs = FadeOutTime * 1000.0f;
                par.TimeToLiveMs = LifespanSec * 1000.0f;
                par.Rotation = 0.0f;

                par.Initialize();

                inactiveParticles.Add(par);
            }
        }

        public void Play()
        {
            start = DateTime.Now;
            this.Stopped = false;

            if(activeParticles.Count + inactiveParticles.Count < ParticleCount)
            {
                activeParticles.Clear();
                inactiveParticles.Clear();
                GenerateParticles();
            }
            else
            {
                int aCount = activeParticles.Count;
                for (int i = 0; i < aCount; ++i)
                {
                    inactiveParticles.Add(activeParticles[i]);
                }
                activeParticles.Clear();

                int iCount = inactiveParticles.Count;
                for (int i = 0; i < iCount; ++i)
                {
                    inactiveParticles[i].Flush();
                }
            }
        }

        public void Stop()
        {
            this.Stopped = true;
        }

        public override void Update(GameTime gameTime)
        {
            //if (InputManager.Instance.GetKeyboardButtonDown(Microsoft.Xna.Framework.Input.Keys.P))
            //{
            //    Play();
            //}
            //if (InputManager.Instance.GetKeyboardButtonDown(Microsoft.Xna.Framework.Input.Keys.O))
            //{
            //    Stop();
            //}
            if (!TrashSoupGame.Instance.EditorMode && !Stopped)
            {
                // remove inactive particles and update them
                int count = activeParticles.Count;
                toDelete.Clear();
                for (int i = 0; i < count; ++i)
                {
                    if (activeParticles[i].MyState == Particle.ParticleState.INACTIVE)
                    {
                        toDelete.Add(activeParticles[i]);
                    }
                    else
                    {
                        if(RotationMode == ParticleRotationMode.DIRECTION_Z)
                        {
                            activeParticles[i].SolveRotation();
                        }
                        activeParticles[i].Update(gameTime, Wind);
                    }
                }

                int delCount = toDelete.Count;

                for(int i = 0; i < delCount; ++i)
                {
                    activeParticles.Remove(toDelete[i]);
                    if(LoopMode != ParticleLoopMode.NONE)
                    {
                        inactiveParticles.Add(toDelete[i]);
                    }
                }

                // solve delay
                delayHelper += (float)gameTime.ElapsedGameTime.TotalMilliseconds;
                if(delayHelper < lastDelay)
                {
                    return;
                }

                // add particles to active list if we still can
                int inacCount = inactiveParticles.Count;
                if (inacCount > 0)
                {
                    AddParticle();
                }
                else if(LoopMode == ParticleLoopMode.CONTINUOUS)
                {
                    // kill one active particle and restart it immediately
                    inactiveParticles.Add(activeParticles[0]);
                    Particle tr = activeParticles[0];
                    activeParticles.RemoveAt(0);
                    AddParticle();
                }


                if(inactiveParticles.Count == 0 && activeParticles.Count == 0)
                {
                    // no inactives to add - we have finished
                    Stopped = true;
                }
            }
        }

        public override void Draw(Camera cam, Effect effect, GameTime gameTime)
        {
            if (!TrashSoupGame.Instance.EditorMode && !Stopped && effect == null)
            {
                world = MyObject.MyTransform.GetWorldMatrix();
                Vector3 trans, scl;
                Quaternion rot;
                world.Decompose(out scl, out rot, out trans);
                world = IgnoreScale ? Matrix.CreateTranslation(trans + PositionOffset) : (Matrix.CreateScale(scl) * Matrix.CreateTranslation(trans + PositionOffset));

                int count = activeParticles.Count;
                //Debug.Log("Drawing " + count.ToString() + " active particles");

                BlendState bs = TrashSoupGame.Instance.GraphicsDevice.BlendState;
                DepthStencilState ds = TrashSoupGame.Instance.GraphicsDevice.DepthStencilState;
                TrashSoupGame.Instance.GraphicsDevice.BlendState = BlendMode;
                TrashSoupGame.Instance.GraphicsDevice.DepthStencilState = DepthStencilState.DepthRead;
                TrashSoupGame.Instance.GraphicsDevice.SetVertexBuffer(vertices);
                TrashSoupGame.Instance.GraphicsDevice.Indices = indices;

                for (int i = 0; i < count; ++i)
                {
                    activeParticles[i].Draw(ResourceManager.Instance.CurrentScene.Cam, myEffect, gameTime, ref world);
                }

                TrashSoupGame.Instance.GraphicsDevice.DepthStencilState = ds;
                TrashSoupGame.Instance.GraphicsDevice.BlendState = bs;
                TrashSoupGame.Instance.GraphicsDevice.SetVertexBuffer(null);
                TrashSoupGame.Instance.GraphicsDevice.Indices = null;
            }
        }

        private void AddParticle()
        {
            RandAngle = Vector3.Up + new Vector3(
                    -Offset.X + (float)SingleRandom.Instance.rnd.NextDouble() * (Offset.X - (-Offset.X)),
                    -Offset.Y + (float)SingleRandom.Instance.rnd.NextDouble() * (Offset.Y - (-Offset.Y)),
                    -Offset.Z + (float)SingleRandom.Instance.rnd.NextDouble() * (Offset.Z - (-Offset.Z))
                    );

            Particle newPar = inactiveParticles[inactiveParticles.Count - 1];
            inactiveParticles.Remove(newPar);

            newPar.StartPosition = PositionOffset + new Vector3(
                (float)SingleRandom.Instance.rnd.NextDouble() * PositionOffsetVariation.X * 2 - PositionOffsetVariation.X,
                (float)SingleRandom.Instance.rnd.NextDouble() * PositionOffsetVariation.Y * 2 - PositionOffsetVariation.Y,
                (float)SingleRandom.Instance.rnd.NextDouble() * PositionOffsetVariation.Z * 2 - PositionOffsetVariation.Z);
            newPar.Flush();
            newPar.Speed = RandAngle * (Speed + (float)SingleRandom.Instance.rnd.NextDouble() * SpeedVariation * 2 - SpeedVariation);
            newPar.DiffuseColor = ParticleColor + new Vector3(
                (float)SingleRandom.Instance.rnd.NextDouble() * ParticleColorVariation.X * 2 - ParticleColorVariation.X,
                (float)SingleRandom.Instance.rnd.NextDouble() * ParticleColorVariation.Y * 2 - ParticleColorVariation.Y,
                (float)SingleRandom.Instance.rnd.NextDouble() * ParticleColorVariation.Z * 2 - ParticleColorVariation.Z);
            newPar.Size = ParticleSize + new Vector2(
                (float)SingleRandom.Instance.rnd.NextDouble() * ParticleSizeVariation.X * 2 - ParticleSizeVariation.X,
                (float)SingleRandom.Instance.rnd.NextDouble() * ParticleSizeVariation.Y * 2 - ParticleSizeVariation.Y);
            newPar.Texture = Textures[SingleRandom.Instance.rnd.Next(Textures.Count)];
            newPar.FadeInMs = (FadeInTime + (float)SingleRandom.Instance.rnd.NextDouble() * FadeInTimeVariation * 2 - FadeInTimeVariation) * 1000.0f;
            newPar.FadeOutMs = (FadeOutTime + (float)SingleRandom.Instance.rnd.NextDouble() * FadeOutTimeVariation * 2 - FadeOutTimeVariation) * 1000.0f;
            newPar.TimeToLiveMs = (LifespanSec + (float)SingleRandom.Instance.rnd.NextDouble() * LifespanSecVariation * 2 - LifespanSecVariation) * 1000.0f;

            if (UseGravity)
                newPar.Mass = (Mass + (float)SingleRandom.Instance.rnd.NextDouble() * MassVariation * 2 - MassVariation) * 1000.0f;
            else
                newPar.Mass = 0.0f;

            newPar.WindVariation = new Vector3(
                (float)SingleRandom.Instance.rnd.NextDouble() * WindVariation.X * 2 - WindVariation.X,
                (float)SingleRandom.Instance.rnd.NextDouble() * WindVariation.Y * 2 - WindVariation.Y,
                (float)SingleRandom.Instance.rnd.NextDouble() * WindVariation.Z * 2 - WindVariation.Z);
            //newPar.TimeToLiveMs = Lifespan * 1000.0f - (DateTime.Now - start).Ticks / 10000.0f;   // here we keep exactly same amount of time given
            // assuming rotation is fixed

            if (RotationMode != ParticleRotationMode.DIRECTION_RANDOM)
            {
                newPar.Rotation = (ParticleRotation.Z + (float)SingleRandom.Instance.rnd.NextDouble() * ParticleRotationVariation.Z * 2 - ParticleRotationVariation.Z);
            }
            else
            {
                Vector3 newRotation = Vector3.Zero;
                newRotation.X = (ParticleRotation.X + (float)SingleRandom.Instance.rnd.NextDouble() * ParticleRotationVariation.X * 2 - ParticleRotationVariation.X);
                newRotation.Y = (ParticleRotation.Y + (float)SingleRandom.Instance.rnd.NextDouble() * ParticleRotationVariation.Y * 2 - ParticleRotationVariation.Y);
                newRotation.Z = (ParticleRotation.Z + (float)SingleRandom.Instance.rnd.NextDouble() * ParticleRotationVariation.Z * 2 - ParticleRotationVariation.Z);
                newPar.RotationMatrix = Matrix.CreateFromYawPitchRoll(newRotation.X, newRotation.Y, newRotation.Z);
            }

            activeParticles.Add(newPar);

            delayHelper = 0.0f;
            lastDelay = DelayMs + (float)SingleRandom.Instance.rnd.NextDouble() * DelayMs * 2 - DelayMs;
        }

        public override void Initialize()
        {
            vertices = new VertexBuffer(TrashSoupGame.Instance.GraphicsDevice, typeof(VertexPositionTexture),
                4, BufferUsage.WriteOnly);
            indices = new IndexBuffer(TrashSoupGame.Instance.GraphicsDevice, IndexElementSize.ThirtyTwoBits,
                6, BufferUsage.WriteOnly);

            VertexPositionTexture[] verts = new VertexPositionTexture[4];

            verts[0] = new VertexPositionTexture(new Vector3(0.0f, 0.0f, 0.0f), new Vector2(0.0f, 0.0f));
            verts[1] = new VertexPositionTexture(new Vector3(0.0f, 0.0f, 0.0f), new Vector2(1.0f, 0.0f));
            verts[2] = new VertexPositionTexture(new Vector3(0.0f, 0.0f, 0.0f), new Vector2(1.0f, 1.0f));
            verts[3] = new VertexPositionTexture(new Vector3(0.0f, 0.0f, 0.0f), new Vector2(0.0f, 1.0f));

            vertices.SetData<VertexPositionTexture>(verts);
            indices.SetData<int>(new int[] { 2, 1, 0, 3, 2, 0 });

            this.graphicsDevice = TrashSoupGame.Instance.GraphicsDevice;

            myEffect = ResourceManager.Instance.LoadEffect("Effects/ParticleEffect");

            GenerateParticles();

            start = DateTime.Now;

            lastDelay = DelayMs + (float)SingleRandom.Instance.rnd.NextDouble() * DelayMs * 2 - DelayMs;
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

            string rotationMode = reader.ReadElementString("RotationMode", "");
            string loopMode = reader.ReadElementString("LoopMode", "");
            string blendMode = reader.ReadElementString("BlendMode", "");

            switch (rotationMode)
            {
                case "PLAIN":
                    RotationMode = ParticleRotationMode.PLAIN;
                    break;
                case "DIRECTION_Z":
                    RotationMode = ParticleRotationMode.DIRECTION_Z;
                    break;
                case "DIRECTION_RANDOM":
                    RotationMode = ParticleRotationMode.DIRECTION_RANDOM;
                    break;
            }

            switch (loopMode)
            {
                case "NONE":
                    LoopMode = ParticleLoopMode.NONE;
                    break;
                case "BURST":
                    LoopMode = ParticleLoopMode.BURST;
                    break;
                case "CONTINUOUS":
                    LoopMode = ParticleLoopMode.CONTINUOUS;
                    break;
            }

            switch (blendMode)
            {
                case "ADDITIVE":
                    BlendMode = BlendState.Additive;
                    break;
                case "ALPHA":
                    BlendMode = BlendState.AlphaBlend;
                    break;
                case "ALPHANP":
                    BlendMode = BlendState.NonPremultiplied;
                    break;
                case "OPAQUE":
                    BlendMode = BlendState.Opaque;
                    break;
            }

            Offset = ReadVector3(reader, "Offset");
            RandAngle = ReadVector3(reader, "RandAngle");
            Wind = ReadVector3(reader, "Wind");
            WindVariation = ReadVector3(reader, "WindVariation");
            PositionOffset = ReadVector3(reader, "PositionOffset");
            PositionOffsetVariation = ReadVector3(reader, "PositionOffsetVariation");
            ParticleRotation = ReadVector3(reader, "ParticleRotation");
            ParticleRotationVariation = ReadVector3(reader, "ParticleRotationVariation");
            ParticleColor = ReadVector3(reader, "ParticleColor");
            ParticleColorVariation = ReadVector3(reader, "ParticleColorVariation");
            ParticleSize = ReadVector2(reader, "ParticleSize");
            ParticleSizeVariation = ReadVector2(reader, "ParticleSizeVariation");

            if (reader.Name == "Textures")
            {
                reader.ReadStartElement();
                Textures = new List<Texture2D>();
                while (reader.NodeType != XmlNodeType.EndElement)
                {
                    Textures.Add(ResourceManager.Instance.LoadTexture(reader.ReadElementString("TexturePath")));
                }
                reader.ReadEndElement();
            }

            LifespanSec = reader.ReadElementContentAsFloat("LifeSpanSec", "");
            LifespanSecVariation = reader.ReadElementContentAsFloat("LifeSpanSecVariation", "");
            FadeInTime = reader.ReadElementContentAsFloat("FadeInTime", "");
            FadeInTimeVariation = reader.ReadElementContentAsFloat("FadeInTimeVariation", "");
            FadeOutTime = reader.ReadElementContentAsFloat("FadeOutTime", "");
            FadeOutTimeVariation = reader.ReadElementContentAsFloat("FadeOutTimeVariation", "");
            Speed = reader.ReadElementContentAsFloat("Speed", "");
            SpeedVariation = reader.ReadElementContentAsFloat("SpeedVariation", "");
            DelayMs = reader.ReadElementContentAsFloat("DelayMs", "");
            DelayMsVariation = reader.ReadElementContentAsFloat("DelayMsVariation", "");
            Mass = reader.ReadElementContentAsFloat("Mass", "");
            MassVariation = reader.ReadElementContentAsFloat("MassVariation", "");
            ParticleCount = reader.ReadElementContentAsInt("ParticleCount", "");
            IgnoreScale = reader.ReadElementContentAsBoolean("IgnoreScale", "");
            Stopped = reader.ReadElementContentAsBoolean("Stopped", "");
            UseGravity = reader.ReadElementContentAsBoolean("UseGravity", "");

            reader.ReadEndElement();
        }

        public override void WriteXml(XmlWriter writer)
        {
            base.WriteXml(writer);
            writer.WriteElementString("RotationMode", RotationMode.ToString());
            writer.WriteElementString("LoopMode", LoopMode.ToString());
            writer.WriteStartElement("BlendMode");

            if(BlendMode == BlendState.Additive)
            {
                writer.WriteValue("ADDITIVE");
            }
            else if (BlendMode == BlendState.AlphaBlend)
            {
                writer.WriteValue("ALPHA");
            }
            else if (BlendMode == BlendState.NonPremultiplied)
            {
                writer.WriteValue("ALPHANP");
            }
            else if (BlendMode == BlendState.Opaque)
            {
                writer.WriteValue("OPAQUE");
            }
            else
            {
                throw new InvalidOperationException("ParticleSystem: Unrecognized BlendMode");
            }

            writer.WriteEndElement();
            
            WriteVector3(writer, Offset, "Offset");
            WriteVector3(writer, RandAngle, "RandAngle");
            WriteVector3(writer, Wind, "Wind");
            WriteVector3(writer, WindVariation, "WindVariation");
            WriteVector3(writer, PositionOffset, "PositionOffset");
            WriteVector3(writer, PositionOffsetVariation, "PositionOffsetVariation");
            WriteVector3(writer, ParticleRotation, "ParticleRotation");
            WriteVector3(writer, ParticleRotationVariation, "ParticleRotationVariation");
            WriteVector3(writer, ParticleColor, "ParticleColor");
            WriteVector3(writer, ParticleColorVariation, "ParticleColorVariation");
            WriteVector2(writer, ParticleSize, "ParticleSize");
            WriteVector2(writer, ParticleSizeVariation, "ParticleSizeVariation");

            writer.WriteStartElement("Textures");
            foreach (Texture2D texture in Textures)
            {
                writer.WriteElementString("TexturePath", ResourceManager.Instance.Textures.FirstOrDefault(x => x.Value == texture).Key);
            }
            writer.WriteEndElement();

            writer.WriteElementString("LifeSpanSec", XmlConvert.ToString(LifespanSec));
            writer.WriteElementString("LifeSpanSecVariation", XmlConvert.ToString(LifespanSecVariation));
            writer.WriteElementString("FadeInTime", XmlConvert.ToString(FadeInTime));
            writer.WriteElementString("FadeInTimeVariation", XmlConvert.ToString(FadeInTimeVariation));
            writer.WriteElementString("FadeOutTime", XmlConvert.ToString(FadeOutTime));
            writer.WriteElementString("FadeOutTimeVariation", XmlConvert.ToString(FadeOutTime));
            writer.WriteElementString("Speed", XmlConvert.ToString(Speed));
            writer.WriteElementString("SpeedVariation", XmlConvert.ToString(SpeedVariation));
            writer.WriteElementString("DelayMs", XmlConvert.ToString(DelayMs));
            writer.WriteElementString("DelayMsVariation", XmlConvert.ToString(DelayMsVariation));
            writer.WriteElementString("Mass", XmlConvert.ToString(Mass));
            writer.WriteElementString("MassVariation", XmlConvert.ToString(MassVariation));
            writer.WriteElementString("ParticleCount", XmlConvert.ToString(ParticleCount));
            writer.WriteElementString("IgnoreScale", XmlConvert.ToString(IgnoreScale));
            writer.WriteElementString("Stopped", XmlConvert.ToString(Stopped));
            writer.WriteElementString("UseGravity", XmlConvert.ToString(UseGravity));
        }

        private void WriteVector3(XmlWriter writer, Vector3 input, string name)
        {
            writer.WriteStartElement(name);
            writer.WriteElementString("X", XmlConvert.ToString(input.X));
            writer.WriteElementString("Y", XmlConvert.ToString(input.Y));
            writer.WriteElementString("Z", XmlConvert.ToString(input.Z));
            writer.WriteEndElement();
        }

        private Vector3 ReadVector3(XmlReader reader, string name)
        {
            Vector3 output = Vector3.Zero;
            if (reader.Name == name)
            {
                reader.ReadStartElement();
                output = new Vector3(reader.ReadElementContentAsFloat("X", ""),
                    reader.ReadElementContentAsFloat("Y", ""),
                    reader.ReadElementContentAsFloat("Z", ""));
                reader.ReadEndElement();
            }
            return output;
        }

        private void WriteVector2(XmlWriter writer, Vector2 input, string name)
        {
            writer.WriteStartElement(name);
            writer.WriteElementString("X", XmlConvert.ToString(input.X));
            writer.WriteElementString("Y", XmlConvert.ToString(input.Y));
            writer.WriteEndElement();
        }

        private Vector2 ReadVector2(XmlReader reader, string name)
        {
            Vector2 output = Vector2.Zero;
            if (reader.Name == name)
            {
                reader.ReadStartElement();
                output = new Vector2(reader.ReadElementContentAsFloat("X", ""),
                    reader.ReadElementContentAsFloat("Y", ""));
                reader.ReadEndElement();
            }
            return output;
        }

        #endregion

    }

}
