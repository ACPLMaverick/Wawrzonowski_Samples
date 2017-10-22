using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Graphics;
using TrashSoup.Engine;
using System.IO;

namespace TrashSoup.Gameplay
{
    public class PlayerController : ObjectComponent, IXmlSerializable
    {
        #region constants

        protected const float PLAYER_SPEED = 12.0f;
        protected const float SPRINT_MULTIPLIER = 5.0f;
        protected const float SPRINT_ACCELERATION = 3.0f;
        protected const float SPRINT_DECELERATION = 2.5f*SPRINT_ACCELERATION;
        protected const float ROTATION_SPEED = 0.55f;
        public const float MAX_HEALTH = 50.0f;
        public const float MAX_POPULARITY = 100.0f;
        public const float DAMAGE_INCREASE_POPULARITY_AMOUNT = 0.8f * MAX_POPULARITY;
        protected const float POPULARITY_STOP_COOLDOWN = 1.0f;

        #endregion

        #region variables

        protected bool GOD_MODE = true;
        private int stairsTouching = 0;

        private SpriteFont font;
        private bool drawFoodWarning = false;
        private float foodWarningTimer = 0.0f;
        private Vector2 foodWarningPos = new Vector2(0.35f, 0.7f);
        private Vector2 weaponInfoPos = new Vector2(0.4f, 0.8f);
        private Vector2 deadPos = new Vector2(0.31f, 0.2f);

        protected Vector3 tempMove;
        protected Vector3 tempMoveRotated;
        protected Vector3 prevForward;
        protected float nextForwardAngle;
        protected float rotM;
        protected float sprint;
        protected float sprintM;
        protected float rotation;

        protected float rotY;
        protected float prevRotY;

        protected bool moving = false;

        public bool CollectedTrash = false;
        private double collisionFakeTime = 0.0;
        public double CollectedFakeTime = 0.0;

        private bool collisionWithWeapon = false;
        private bool collectedWeapon = false;
        //private double weaponCollisionFakeTime = 0.0;
        //private double weaponCollectedFakeTime = 0.0;
        private GameObject anotherWeapon;
        private GameObject weapon;

        private Equipment equipment;

        private PostEffectController pec;
        private ParticleSystem mBloodParticle;

        private float hitPoints = MAX_HEALTH;
        private float popularity = 0.0f;
        private float popularityDecreaseSpeed = 3.0f;
        private bool popularityEarned = false;
        private float popularityEarnedTimer = 0.0f;

        private bool isDead = false;

        private Texture2D interactionTexture;
        private Vector2 trashTextPosition = new Vector2(0.43f, 0.35f);
        private Vector2 weaponTextPosition = new Vector2(0.43f, 0.35f);

        public SoundEffect WalkSoundEffect;
        public SoundEffectInstance Walk;

        private bool deathAnimPlayed = false;

        private GameObject myAttackTrigger;
        private bool isAttacking = false;
        private float attackTimer = 0.0f;
        private float attackCooldown = 0.0f;
        private int combo = 1;
        private float lastAttackTime = 0.0f;
        private int damageMultiplier = 1;

        private string currentIdleAnimID;
        private Weapon eqLastWeap = null;

        private bool isBlocking = false;
        private bool dodged = false;
        private bool blockingAnim = false;
        private bool blockingAnimStopped = false;
        private float dodgeTimer = 0.0f;

        public Cue CommentaryCue;

        #endregion

        #region properties

        public PlayerAttackTrigger MyAttackTriggerComponent { get; private set; }

        public int StairsTouching
        {
            get
            {
                return this.stairsTouching;
            }
            set
            {
                this.stairsTouching = value;
                this.MyObject.MyPhysicalObject.IsUsingGravity = this.stairsTouching == 0;
                if(!this.MyObject.MyPhysicalObject.IsUsingGravity)
                {
                    this.MyObject.MyPhysicalObject.Velocity = Vector3.Zero;
                    this.MyObject.MyPhysicalObject.ZeroForce();
                }
            }
        }

        public float HitPoints 
        { 
            get { return hitPoints; }
            set
            {
                float val = value;
                if(GOD_MODE)
                {
                    val = MathHelper.Clamp(val, 1.0f, float.MaxValue);
                }
                hitPoints = val;
            }
        }

        public float Popularity
        {
            get { return popularity; }
            set { popularity = value; }
        }

        public bool IsDead
        { 
            get { return isDead; }
            set { isDead = value; }
        }

        public Equipment Equipment
        {
            get { return this.equipment; }
        }

        public bool FoodSaw
        {
            get;
            set;
        }

        public Food Food
        {
            get;
            set;
        }

        public bool Dodged
        {
            get
            {
                return this.dodged;
            }
        }

        #endregion

        #region methods
        public PlayerController() { }


        public PlayerController(GameObject obj) : base(obj)
        {
            Start();
        }

        public PlayerController(GameObject obj, PlayerController pc) : base(obj)
        {
            Start();
        }

        public override void Update(Microsoft.Xna.Framework.GameTime gameTime)
        {
#if DEBUG
            if(equipment.CurrentWeapon != null)
            GUIManager.Instance.DrawText(this.font,
                    "Weapon: " + equipment.CurrentWeapon.Name, this.weaponInfoPos, Color.Red);
#endif
            if(InputHandler.Instance.GodMode())
            {
                this.GOD_MODE = !this.GOD_MODE;
            }

            Vector2 noclip = InputHandler.Instance.NoClipVector();
            if(noclip.X != 0.0f || noclip.Y != 0.0f)
            {
                this.MyObject.MyTransform.Version = Transform.GameVersionEnum.STENGERT_PAGI;
                Vector3 movementVector = ResourceManager.Instance.CurrentScene.Cam.Direction;
                movementVector.Y = 0.0f;
                
                if (noclip.Y == -1.0f && noclip.X == 0.0f)
                    movementVector = -movementVector;
                else if (noclip.Y == 0.0f && noclip.X == 1.0f)
                    movementVector = ResourceManager.Instance.CurrentScene.Cam.Right;
                else if (noclip.Y == 0.0f && noclip.X == -1.0f)
                    movementVector = -ResourceManager.Instance.CurrentScene.Cam.Right;

                movementVector.Z = -movementVector.Z;
                this.MyObject.MyTransform.Position += movementVector * (float)gameTime.ElapsedGameTime.TotalMilliseconds * 0.01f;
                this.MyObject.MyTransform.Version = Transform.GameVersionEnum.PBL;
            }

            if (isDead)
            {
                GUIManager.Instance.DrawText(this.font, "YOU'RE DEAD!", this.deadPos, Color.Red, 4.0f);
                if(!deathAnimPlayed)
                {
                    deathAnimPlayed = true;
                    GameManager.Instance.MovementEnabled = false;
                    this.MyObject.MyAnimator.ChangeState("Death");
                }
                return;
            }

            if (GameManager.Instance.MovementEnabled)
            {
                if (InputHandler.Instance.Eat())
                {
                    if (this.Equipment.FoodCount > 0)
                    {
                        this.Equipment.FoodCount -= 1;
                        IncreaseHealth(MAX_HEALTH);
                    }
                    else
                    {
                        drawFoodWarning = true;
                    }
                }
            }

            if(drawFoodWarning)
            {
                if(this.Equipment.FoodCount > 0)
                {
                    drawFoodWarning = false;
                    foodWarningTimer = 0.0f;
                }
                else
                {
                    GUIManager.Instance.DrawText(this.font, "You have no food. Find one", this.foodWarningPos, Color.Red);
                    foodWarningTimer += gameTime.ElapsedGameTime.Milliseconds * 0.001f;
                    if (foodWarningTimer > 4.0f)
                    {
                        foodWarningTimer = 0.0f;
                        drawFoodWarning = false;
                    }
                }
            }

            if (InputHandler.Instance.DropWeapon())
            {
                if(equipment.CurrentWeapon.Name != "Fists")
                {
                    equipment.CurrentWeapon.TurnMyPointLight(true);
                    equipment.DropWeapon(this.anotherWeapon);
                    equipment.CurrentWeapon = (ResourceManager.Instance.CurrentScene.GetObject(1144).Components.First(x => x.GetType() == typeof(Weapons.Fists)) as Weapon);
                    equipment.PickUpWeapon(ResourceManager.Instance.CurrentScene.GetObject(1144));
                }
            }

            equipment.Update(gameTime);
            if (GameManager.Instance.MovementEnabled)
            {
                Vector2 movementVector = InputHandler.Instance.GetMovementVector();
                tempMove = new Vector3(movementVector.X,
                    (InputManager.Instance.GetKeyboardButton(Keys.Q) ? 1.0f : 0.0f) - (InputManager.Instance.GetKeyboardButton(Keys.Z) ? 1.0f : 0.0f),
                    movementVector.Y);

                if (tempMove.Length() > 0.0f &&
                    ResourceManager.Instance.CurrentScene.Cam != null && !this.isAttacking && !this.isBlocking && !dodged)
                {
                    if (moving == false)
                    {
                        moving = true;

                        if (MyObject.MyAnimator != null)
                        {
                            MyObject.MyAnimator.SetBlendState("Walk");
                        }
                    }
                    if (moving == true)
                    {
                        Walk.Pitch = 0.1f * tempMove.Length();
                        Walk.Play();
                        MyObject.MyAnimator.CurrentInterpolation = MathHelper.Clamp(tempMove.Length(), 0.0f, 1.0f);
                    }
                    // now to rotate that damn vector as camera direction is rotated
                    rotM = rotation;
                    prevForward = MyObject.MyTransform.Forward;

                    rotation = (float)Math.Atan2(ResourceManager.Instance.CurrentScene.Cam.Direction.X,
                        -ResourceManager.Instance.CurrentScene.Cam.Direction.Z);
                    rotation = CurveAngle(rotM, rotation, 3.0f * ROTATION_SPEED);                 // to się chyba gdzieś tu wywala ale nie jestem pewien
                    tempMoveRotated = Vector3.Transform(tempMove, Matrix.CreateRotationY(rotation));

                    if (float.IsNaN(tempMoveRotated.X) || float.IsNaN(tempMoveRotated.Y) || float.IsNaN(tempMoveRotated.Z))
                    {
                        Debug.Log("PLAYERCONTROLLER ERROR: NaN detected in tempMoveRotated");
                    }

                    MyObject.MyTransform.Forward = Vector3.Lerp(prevForward, tempMoveRotated, ROTATION_SPEED);
                    MyObject.MyTransform.Rotation = RotateAsForward(MyObject.MyTransform.Forward, MyObject.MyTransform.Rotation);

                    if (InputHandler.Instance.IsSprinting() && tempMove.Length() >= 0.8f)
                    {
                        sprint = MathHelper.Lerp(1.0f, SPRINT_MULTIPLIER, sprintM);
                        sprintM += SPRINT_ACCELERATION * ((float)gameTime.ElapsedGameTime.TotalMilliseconds / 1000.0f);
                        sprintM = MathHelper.Min(sprintM, 1.0f);
                    }
                    else if (sprintM != 0.0f || sprint != 1.0f)
                    {
                        sprint = MathHelper.Lerp(1.0f, SPRINT_MULTIPLIER, sprintM);
                        sprintM -= SPRINT_DECELERATION * ((float)gameTime.ElapsedGameTime.TotalMilliseconds / 1000.0f);
                        sprintM = MathHelper.Max(sprintM, 0.0f);
                    }
                    MyObject.MyTransform.Position += (MyObject.MyTransform.Forward * PLAYER_SPEED * sprint * ((float)gameTime.ElapsedGameTime.TotalMilliseconds / 1000.0f));
                }
                else
                {
                    if (moving == true)
                    {
                        Walk.Stop(true);
                        moving = false;
                        MyObject.MyAnimator.RemoveBlendStateToCurrent();
                    }
                }                

                if (!dodged && !isBlocking && !isAttacking && InputHandler.Instance.IsAttacking())
                {
                    if (MyObject.MyAnimator != null)
                    {
                        if(gameTime.TotalGameTime.TotalMilliseconds - lastAttackTime < attackCooldown + 300.0f)
                        {
                            combo += 1;
                            if(this.equipment.CurrentWeapon.Type != WeaponType.MEDIUM)
                            {
                                if(combo > 4)
                                {
                                    combo = 1;
                                }
                            }
                            else
                            {
                                if(combo > 5)
                                {
                                    combo = 1;
                                }
                            }
                        }
                        else
                        {
                            combo = 1;
                        }
                        lastAttackTime = (float)gameTime.TotalGameTime.TotalMilliseconds;
                        string animName = BuildCurrentAnimName();
                        MyObject.MyAnimator.ChangeState(animName);
                        attackCooldown = (float)MyObject.MyAnimator.AvailableStates[animName].Animation.CurrentClip.Duration.TotalMilliseconds;
                    }
                    equipment.CurrentWeapon.IsAttacking = true;
                    equipment.CurrentWeapon.timerOn = gameTime.TotalGameTime.TotalSeconds;
                    this.MyAttackTriggerComponent.Attack(this.equipment.CurrentWeapon.Damage, this.damageMultiplier);
                    this.isAttacking = true;
                }

                if(!this.isAttacking)
                {
                    if(InputHandler.Instance.Block())
                    {
                        this.isBlocking = true;
                        if(!blockingAnim)
                        {
                            blockingAnim = true;
                            StartOtherStateNoMovementEnabledFalse("Block");
                        }
                        //if(MyObject.MyAnimator.CurrentState.Name == "Block" && 
                        //    MyObject.MyAnimator.CurrentState.Animation.CurrentTime.TotalMilliseconds >= (MyObject.MyAnimator.CurrentState.Animation.CurrentClip.Duration.TotalMilliseconds - 76) &&
                        //    !blockingAnimStopped)
                        //{
                        //    MyObject.MyAnimator.CurrentState.Animation.PauseClip();
                        //    blockingAnimStopped = true;
                        //}
                        //Debug.Log(MyObject.MyAnimator.CurrentState.Animation.CurrentTime.ToString() + " | " +
                        //    MyObject.MyAnimator.CurrentState.Animation.CurrentClip.Duration.ToString());
                    }
                    else
                    {
                        this.isBlocking = false;
                        if(blockingAnim)
                        {
                            blockingAnim = false;
                            blockingAnimStopped = false;

                            if(InputHandler.Instance.GetMovementVector() != Vector2.Zero)
                            {
                                StopOtherState();
                            }
                            else
                            {
                                MyObject.MyAnimator.ChangeState(currentIdleAnimID);
                            }
                            //Debug.Log(InputHandler.Instance.GetMovementVector().ToString());
                        }
                    }

                    if(!this.isBlocking)
                    {
                        if(!dodged && InputHandler.Instance.Dodge())
                        {
                            this.MyObject.MyAnimator.ChangeState("Dodge");
                            MyObject.MyPhysicalObject.ZeroForce();
                            MyObject.MyPhysicalObject.Velocity = Vector3.Zero;
                            this.MyObject.MyPhysicalObject.AddForce(-this.MyObject.MyTransform.Forward * 45.0f * this.MyObject.MyPhysicalObject.Mass);
                            dodged = true;
                        }
                        else if(dodged)
                        {
                            this.dodgeTimer += gameTime.ElapsedGameTime.Milliseconds * 0.001f;
                            if(dodgeTimer > 1.0f)
                            {
                                dodged = false;
                                dodgeTimer = 0.0f;
                                this.MyObject.MyPhysicalObject.Velocity = Vector3.Zero;
                                this.MyObject.MyPhysicalObject.ZeroForce();
                            }
                        }
                    }
                }
                else
                {
                    this.attackTimer += gameTime.ElapsedGameTime.Milliseconds;
                    if (this.attackTimer >= attackCooldown)
                    {
                        this.attackTimer = 0.0f;
                        this.isAttacking = false;
                    }
                }
            }
            else
            {
                moving = false;
                if(MyObject.MyAnimator.NewState != null) MyObject.MyAnimator.RemoveBlendStateToCurrent();
            }

            if(this.collisionWithWeapon)
            {
                if(!this.collectedWeapon)
                {
                    this.collisionFakeTime = gameTime.TotalGameTime.TotalSeconds;
                    GUIManager.Instance.DrawTexture(this.interactionTexture, new Vector2(0.475f, 0.775f), 0.05f, 0.05f);
                }
            }

            if (!this.collectedWeapon && this.collisionWithWeapon && InputHandler.Instance.GetWeapon())
            {
                //this.collectedWeapon = true;
                this.CollectedFakeTime = gameTime.TotalGameTime.TotalSeconds;
                if(this.anotherWeapon != null)
                {
                    equipment.DropWeapon(this.anotherWeapon);
                }
                equipment.CurrentWeapon = this.weapon.Components.First(x => x is Weapon) as Weapon;
                equipment.CurrentWeapon.TurnMyPointLight(false);
                AudioManager.Instance.SoundBank.PlayCue("commentary");
                equipment.PickUpWeapon(this.weapon);
                this.anotherWeapon = this.weapon;
                //PICKING ANIMATION + SOUNDS
            }

            this.collisionWithWeapon = false;
            
            if(this.popularityEarned)
            {
                this.popularityEarnedTimer += gameTime.ElapsedGameTime.Milliseconds * 0.001f;
                if(this.popularityEarnedTimer > POPULARITY_STOP_COOLDOWN)
                {
                    this.popularityEarnedTimer = 0.0f;
                    this.popularityEarned = false;
                }
            }
            else
            {
                this.Popularity -= gameTime.ElapsedGameTime.Milliseconds * 0.001f * this.popularityDecreaseSpeed;
                this.Popularity = MathHelper.Clamp(this.Popularity, 0.0f, MAX_POPULARITY);
            }

            if(this.FoodSaw)
            {
                GUIManager.Instance.DrawTexture(this.interactionTexture, new Vector2(0.475f, 0.775f), 0.05f, 0.05f);
                if(InputHandler.Instance.Action())
                {
                    AudioManager.Instance.SoundBank.PlayCue("commentary");
                    this.equipment.AddFood();
                    this.FoodSaw = false;
                    this.Food.MyObject.Enabled = false;
                    this.Food.RemoveMyPointLight();
                    this.Food = null;
                }
            }

            if (eqLastWeap != Equipment.CurrentWeapon)
            {
                eqLastWeap = Equipment.CurrentWeapon;
                IdleChangeHandler(eqLastWeap);
            }

            if (this.popularity >= DAMAGE_INCREASE_POPULARITY_AMOUNT)
            {
                this.damageMultiplier = 2;
            }
            else
            {
                this.damageMultiplier = 1;
            }

            if(InputHandler.Instance.ResetPlayer())
            {
                StopOtherState();
            }

            pec.Update(gameTime);
        }

        public override void Initialize()
        {
            this.font = TrashSoupGame.Instance.Content.Load<SpriteFont>("Fonts/FontTest");
            this.interactionTexture = ResourceManager.Instance.LoadTexture(@"Textures/HUD/x_button");
            this.weapon = new GameObject(0, "");
            //GameObject w = ResourceManager.Instance.CurrentScene.GetObject(1144);
            //if (w != null)
            //{
            //    equipment.CurrentWeapon = (w.Components.First(x => x.GetType() == typeof(Weapons.Fists)) as Weapon);
            //    equipment.PickUpWeapon(w);
            //}
            this.myAttackTrigger = new GameObject(1122334455, "MyAttackTrigger");
            this.myAttackTrigger.MyTransform = new Transform(this.myAttackTrigger, Vector3.Backward * 1.5f + Vector3.Up * 1.0f, Vector3.Forward, Vector3.Zero, 1.5f);
            this.MyAttackTriggerComponent = new PlayerAttackTrigger(this.myAttackTrigger, this);
            this.myAttackTrigger.AddComponentRuntime(this.MyAttackTriggerComponent);
            this.myAttackTrigger.MyCollider = new BoxCollider(this.myAttackTrigger, true);
            this.MyObject.AddChild(this.myAttackTrigger);
            ResourceManager.Instance.CurrentScene.AddObjectRuntime(this.myAttackTrigger);

            pec = new PostEffectController(this, (DefaultPostEffect)ResourceManager.Instance.CurrentScene.CurrentPostEffect, MAX_HEALTH, MAX_POPULARITY);

            if (!TrashSoupGame.Instance.EditorMode)
            {
                if (SaveManager.Instance.XmlPath.Contains("save2"))
                {
                    SaveManager.Instance.OnLevelBeginLoad += LevelLoad;
                }
                else if (SaveManager.Instance.XmlPath.Contains("safehouse"))
                {
                    for(int i = 0 ; i < 5; ++i)
                    {
                        this.equipment.AddFood();
                    }
                    Type t = typeof(Weapon);
                    using (StreamReader reader = new StreamReader("weapon.txt"))
                    {
                        string s = reader.ReadToEnd();
                        t = Type.GetType(s);
                    }
                    List<ObjectComponent> weaponsOfThisType = ResourceManager.Instance.CurrentScene.GetComponentsOfType(t);
                    if (weaponsOfThisType != null && weaponsOfThisType.Count > 0)
                    {
                        int ind = 0;
                        float maxDistance = 0.0f;
                        float currDistance = 0.0f;
                        foreach (ObjectComponent go in weaponsOfThisType)
                        {
                            currDistance = Vector3.Distance(this.MyObject.MyTransform.Position, go.MyObject.MyTransform.Position);
                            if (currDistance > maxDistance)
                            {
                                ind = weaponsOfThisType.IndexOf(go);
                                maxDistance = currDistance;
                            }
                        }
                        this.Equipment.PickUpWeapon(weaponsOfThisType[ind].MyObject);
                        this.Equipment.CurrentWeapon = (Weapon)weaponsOfThisType[ind];
                        this.anotherWeapon = this.Equipment.CurrentWeapon.MyObject;
                        equipment.CurrentWeapon.TurnMyPointLight(false);
                    }
                }
            }

            mBloodParticle = new ParticleSystem(this.MyObject);
            mBloodParticle.Textures.Add(ResourceManager.Instance.LoadTexture("Textures/Particles/Particle_blood01"));
            mBloodParticle.Textures.Add(ResourceManager.Instance.LoadTexture("Textures/Particles/Particle_blood02"));
            mBloodParticle.Textures.Add(ResourceManager.Instance.LoadTexture("Textures/Particles/Particle_blood03"));
            mBloodParticle.Textures.Add(ResourceManager.Instance.LoadTexture("Textures/Particles/Particle_blood04"));
            mBloodParticle.Offset = new Vector3(0.07453292f, 0.07453292f, 0.07453292f);
            mBloodParticle.PositionOffset = new Vector3(0.0f, 0.65f, 0.0f);
            mBloodParticle.Wind = new Vector3(0.0f, 1.0f, 0.0f);
            mBloodParticle.ParticleCount = 7;
            mBloodParticle.ParticleSize = new Vector2(1.05f, 1.05f);
            mBloodParticle.ParticleSizeVariation = new Vector2(0.1f, 0.1f);
            mBloodParticle.LifespanSec = 0.15f;
            mBloodParticle.Wind = new Vector3(0.0f, 0.0f, 0.0f);
            mBloodParticle.Offset = new Vector3(MathHelper.PiOver2);
            mBloodParticle.Speed = 1.5f;
            mBloodParticle.RotationMode = ParticleSystem.ParticleRotationMode.PLAIN;
            mBloodParticle.ParticleRotation = new Vector3(0.0f, 0.0f, 0.0f);
            mBloodParticle.FadeInTime = 0.0f;
            mBloodParticle.FadeOutTime = 0.1f;
            mBloodParticle.IgnoreScale = true;
            //ps.PositionOffset = new Vector3(0.0f, -1.0f, 0.0f) * MyObject.MyTransform.Scale;
            mBloodParticle.BlendMode = BlendState.AlphaBlend;
            mBloodParticle.UseGravity = false;
            mBloodParticle.Initialize();

            mBloodParticle.Stop();

            GameObject particleCarrier = new GameObject(100000214, "ParticleCarrier");
            particleCarrier.DrawLast = true;
            ResourceManager.Instance.CurrentScene.AddObjectRuntime(particleCarrier);
            particleCarrier.Components.Add(mBloodParticle);
            particleCarrier.MyTransform = MyObject.MyTransform;

            base.Initialize();
        }

        public override void Draw(Camera cam, Effect effect, Microsoft.Xna.Framework.GameTime gameTime)
        {
            // Draw nothing
        }

        public void AddPopularity(float amount = 10.0f)
        {
            amount = amount + amount * 0.5f * (combo - 1);
            this.popularityEarned = true;
            this.popularityEarnedTimer = 0.0f;
            this.Popularity += amount;
            this.Popularity = MathHelper.Clamp(this.Popularity, 0.0f, MAX_POPULARITY);
        }

        protected override void Start()
        {
            sprint = 1.0f;
            sprintM = 0.0f;
            equipment = new Equipment(this.MyObject);
            //if(this.MyObject.GetComponent<Weapons.Fists>() == null)
                //this.MyObject.Components.Add(equipment.CurrentWeapon);

            if (MyObject == null) return;

            if(MyObject.MyAnimator != null)
            {
                MyObject.MyAnimator.AvailableStates.Add("Idle", new AnimatorState("Idle", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/idle_Fists")));
                MyObject.MyAnimator.AvailableStates.Add("IdleSM", new AnimatorState("IdleSM", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/idle_SMweapon")));
                MyObject.MyAnimator.AvailableStates.Add("IdleH", new AnimatorState("IdleH", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/idle_Hweapon")));
                MyObject.MyAnimator.AvailableStates.Add("Walk", new AnimatorState("Walk", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/run_2")));
                MyObject.MyAnimator.AvailableStates.Add("Build", new AnimatorState("Build", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/building")));
                MyObject.MyAnimator.AvailableStates.Add("PickTrash", new AnimatorState("PickTrash", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/grzebanie")));
                MyObject.MyAnimator.AvailableStates.Add("Block", new AnimatorState("Block", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/block_1"), AnimatorState.StateType.SINGLE));
                MyObject.MyAnimator.AvailableStates.Add("Death", new AnimatorState("Death", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/dying_1"), AnimatorState.StateType.SINGLE));
                MyObject.MyAnimator.AvailableStates.Add("Dodge", new AnimatorState("Dodge", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/dodge_1"), AnimatorState.StateType.SINGLE));
                MyObject.MyAnimator.AvailableStates.Add("AttackFist01", new AnimatorState("AttackFist01", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/boxing_1"), AnimatorState.StateType.SINGLE));
                MyObject.MyAnimator.AvailableStates.Add("AttackFist02", new AnimatorState("AttackFist02", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/boxing_2"), AnimatorState.StateType.SINGLE));
                MyObject.MyAnimator.AvailableStates.Add("AttackFist03", new AnimatorState("AttackFist03", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/boxing_3"), AnimatorState.StateType.SINGLE));
                MyObject.MyAnimator.AvailableStates.Add("AttackFist04", new AnimatorState("AttackFist04", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/boxing_4"), AnimatorState.StateType.SINGLE));
                MyObject.MyAnimator.AvailableStates.Add("AttackHWeapon01", new AnimatorState("AttackHWeapon01", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/attack_Hweapon_1"), AnimatorState.StateType.SINGLE));
                MyObject.MyAnimator.AvailableStates.Add("AttackHWeapon02", new AnimatorState("AttackHWeapon02", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/attack_Hweapon_2"), AnimatorState.StateType.SINGLE));
                MyObject.MyAnimator.AvailableStates.Add("AttackHWeapon03", new AnimatorState("AttackHWeapon03", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/attack_Hweapon_3"), AnimatorState.StateType.SINGLE));
                MyObject.MyAnimator.AvailableStates.Add("AttackHWeapon04", new AnimatorState("AttackHWeapon04", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/attack_Hweapon_4"), AnimatorState.StateType.SINGLE));
                MyObject.MyAnimator.AvailableStates.Add("AttackMWeapon01", new AnimatorState("AttackMWeapon01", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/attack_Mweapon_3"), AnimatorState.StateType.SINGLE));
                MyObject.MyAnimator.AvailableStates.Add("AttackMWeapon02", new AnimatorState("AttackMWeapon02", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/attack_Mweapon_2"), AnimatorState.StateType.SINGLE));
                MyObject.MyAnimator.AvailableStates.Add("AttackMWeapon03", new AnimatorState("AttackMWeapon03", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/attack_Mweapon_3"), AnimatorState.StateType.SINGLE));
                MyObject.MyAnimator.AvailableStates.Add("AttackMWeapon04", new AnimatorState("AttackMWeapon04", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/attack_Mweapon_4"), AnimatorState.StateType.SINGLE));
                MyObject.MyAnimator.AvailableStates.Add("AttackMWeapon05", new AnimatorState("AttackMWeapon05", MyObject.MyAnimator.GetAnimationPlayer("Animations/MainCharacter/attack_Mweapon_5"), AnimatorState.StateType.SINGLE));
                MyObject.MyAnimator.AvailableStates["Idle"].AddTransition(MyObject.MyAnimator.AvailableStates["Walk"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["Walk"].AddTransition(MyObject.MyAnimator.AvailableStates["Idle"], new TimeSpan(0, 0, 0, 0, 250));
                MyObject.MyAnimator.AvailableStates["IdleSM"].AddTransition(MyObject.MyAnimator.AvailableStates["Walk"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["Walk"].AddTransition(MyObject.MyAnimator.AvailableStates["IdleSM"], new TimeSpan(0, 0, 0, 0, 250));
                MyObject.MyAnimator.AvailableStates["IdleH"].AddTransition(MyObject.MyAnimator.AvailableStates["Walk"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["Walk"].AddTransition(MyObject.MyAnimator.AvailableStates["IdleH"], new TimeSpan(0, 0, 0, 0, 250));
                MyObject.MyAnimator.AvailableStates["Block"].AddTransition(MyObject.MyAnimator.AvailableStates["Idle"], new TimeSpan(0, 0, 0, 0, 100));
                MyObject.MyAnimator.AvailableStates["Block"].AddTransition(MyObject.MyAnimator.AvailableStates["IdleSM"], new TimeSpan(0, 0, 0, 0, 100));
                MyObject.MyAnimator.AvailableStates["Block"].AddTransition(MyObject.MyAnimator.AvailableStates["IdleH"], new TimeSpan(0, 0, 0, 0, 100));
                MyObject.MyAnimator.AvailableStates["Block"].AddTransition(MyObject.MyAnimator.AvailableStates["Walk"], new TimeSpan(0, 0, 0, 0, 100));
                MyObject.MyAnimator.AvailableStates["Block"].AddTransition(MyObject.MyAnimator.AvailableStates["AttackFist01"], new TimeSpan(0, 0, 0, 0, 1));
                MyObject.MyAnimator.AvailableStates["Block"].AddTransition(MyObject.MyAnimator.AvailableStates["AttackHWeapon01"], new TimeSpan(0, 0, 0, 0, 1));
                MyObject.MyAnimator.AvailableStates["Block"].AddTransition(MyObject.MyAnimator.AvailableStates["AttackMWeapon01"], new TimeSpan(0, 0, 0, 0, 1));
                MyObject.MyAnimator.AvailableStates["Idle"].AddTransition(MyObject.MyAnimator.AvailableStates["Block"], new TimeSpan(0, 0, 0, 0, 250));
                MyObject.MyAnimator.AvailableStates["IdleH"].AddTransition(MyObject.MyAnimator.AvailableStates["Block"], new TimeSpan(0, 0, 0, 0, 250));
                MyObject.MyAnimator.AvailableStates["IdleSM"].AddTransition(MyObject.MyAnimator.AvailableStates["Block"], new TimeSpan(0, 0, 0, 0, 250));
                MyObject.MyAnimator.AvailableStates["Walk"].AddTransition(MyObject.MyAnimator.AvailableStates["Block"], new TimeSpan(0, 0, 0, 0, 250));
                MyObject.MyAnimator.AvailableStates["Idle"].AddTransition(MyObject.MyAnimator.AvailableStates["Dodge"], new TimeSpan(0, 0, 0, 0, 100));
                MyObject.MyAnimator.AvailableStates["IdleH"].AddTransition(MyObject.MyAnimator.AvailableStates["Dodge"], new TimeSpan(0, 0, 0, 0, 100));
                MyObject.MyAnimator.AvailableStates["IdleSM"].AddTransition(MyObject.MyAnimator.AvailableStates["Dodge"], new TimeSpan(0, 0, 0, 0, 100));
                MyObject.MyAnimator.AvailableStates["Walk"].AddTransition(MyObject.MyAnimator.AvailableStates["Dodge"], new TimeSpan(0, 0, 0, 0, 100));
                MyObject.MyAnimator.AvailableStates["Dodge"].AddTransition(MyObject.MyAnimator.AvailableStates["Idle"], new TimeSpan(0, 0, 0, 0, 100));
                MyObject.MyAnimator.AvailableStates["Dodge"].AddTransition(MyObject.MyAnimator.AvailableStates["IdleH"], new TimeSpan(0, 0, 0, 0, 100));
                MyObject.MyAnimator.AvailableStates["Dodge"].AddTransition(MyObject.MyAnimator.AvailableStates["IdleSM"], new TimeSpan(0, 0, 0, 0, 100));
                MyObject.MyAnimator.AvailableStates["Dodge"].AddTransition(MyObject.MyAnimator.AvailableStates["Walk"], new TimeSpan(0, 0, 0, 0, 100));

                string[] combatAnimsNames = {"AttackFist01", "AttackFist02", "AttackFist03", "AttackFist04",
                                            "AttackHWeapon01", "AttackHWeapon02", "AttackHWeapon03", "AttackHWeapon04",
                                            "AttackMWeapon01", "AttackMWeapon02", "AttackMWeapon03", "AttackMWeapon04", "AttackMWeapon05"};
                for (int i = 0; i < combatAnimsNames.Length; ++i)
                {
                    MyObject.MyAnimator.AvailableStates["Idle"].AddTransition(MyObject.MyAnimator.AvailableStates[combatAnimsNames[i]], new TimeSpan(0, 0, 0, 0, 100));
                    MyObject.MyAnimator.AvailableStates["IdleSM"].AddTransition(MyObject.MyAnimator.AvailableStates[combatAnimsNames[i]], new TimeSpan(0, 0, 0, 0, 100));
                    MyObject.MyAnimator.AvailableStates["IdleH"].AddTransition(MyObject.MyAnimator.AvailableStates[combatAnimsNames[i]], new TimeSpan(0, 0, 0, 0, 100));
                    MyObject.MyAnimator.AvailableStates[combatAnimsNames[i]].AddTransition(MyObject.MyAnimator.AvailableStates["Idle"], new TimeSpan(0, 0, 0, 0, 500));
                    MyObject.MyAnimator.AvailableStates[combatAnimsNames[i]].AddTransition(MyObject.MyAnimator.AvailableStates["IdleSM"], new TimeSpan(0, 0, 0, 0, 500));
                    MyObject.MyAnimator.AvailableStates[combatAnimsNames[i]].AddTransition(MyObject.MyAnimator.AvailableStates["IdleH"], new TimeSpan(0, 0, 0, 0, 500));
                    MyObject.MyAnimator.AvailableStates[combatAnimsNames[i]].AddTransition(MyObject.MyAnimator.AvailableStates["Death"], new TimeSpan(0, 0, 0, 0, 100));
                    MyObject.MyAnimator.AvailableStates[combatAnimsNames[i]].AddTransition(MyObject.MyAnimator.AvailableStates["Block"], new TimeSpan(0, 0, 0, 0, 250));
                }

                MyObject.MyAnimator.AvailableStates["AttackFist01"].AddTransition(MyObject.MyAnimator.AvailableStates["AttackFist02"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["AttackFist02"].AddTransition(MyObject.MyAnimator.AvailableStates["AttackFist03"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["AttackFist03"].AddTransition(MyObject.MyAnimator.AvailableStates["AttackFist04"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["AttackFist04"].AddTransition(MyObject.MyAnimator.AvailableStates["AttackFist01"], new TimeSpan(0, 0, 0, 0, 200));

                MyObject.MyAnimator.AvailableStates["AttackHWeapon01"].AddTransition(MyObject.MyAnimator.AvailableStates["AttackHWeapon02"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["AttackHWeapon02"].AddTransition(MyObject.MyAnimator.AvailableStates["AttackHWeapon03"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["AttackHWeapon03"].AddTransition(MyObject.MyAnimator.AvailableStates["AttackHWeapon04"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["AttackHWeapon04"].AddTransition(MyObject.MyAnimator.AvailableStates["AttackHWeapon01"], new TimeSpan(0, 0, 0, 0, 200));

                MyObject.MyAnimator.AvailableStates["AttackMWeapon01"].AddTransition(MyObject.MyAnimator.AvailableStates["AttackMWeapon02"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["AttackMWeapon02"].AddTransition(MyObject.MyAnimator.AvailableStates["AttackMWeapon03"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["AttackMWeapon03"].AddTransition(MyObject.MyAnimator.AvailableStates["AttackMWeapon04"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["AttackMWeapon04"].AddTransition(MyObject.MyAnimator.AvailableStates["AttackMWeapon05"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["AttackMWeapon05"].AddTransition(MyObject.MyAnimator.AvailableStates["AttackMWeapon01"], new TimeSpan(0, 0, 0, 0, 200));

                MyObject.MyAnimator.AvailableStates["Idle"].AddTransition(MyObject.MyAnimator.AvailableStates["Build"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["IdleSM"].AddTransition(MyObject.MyAnimator.AvailableStates["Build"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["IdleH"].AddTransition(MyObject.MyAnimator.AvailableStates["Build"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["Walk"].AddTransition(MyObject.MyAnimator.AvailableStates["Build"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["Build"].AddTransition(MyObject.MyAnimator.AvailableStates["Idle"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["Build"].AddTransition(MyObject.MyAnimator.AvailableStates["IdleSM"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["Build"].AddTransition(MyObject.MyAnimator.AvailableStates["IdleH"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["Build"].AddTransition(MyObject.MyAnimator.AvailableStates["Walk"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["Idle"].AddTransition(MyObject.MyAnimator.AvailableStates["Death"], new TimeSpan(0, 0, 0, 0, 100));
                MyObject.MyAnimator.AvailableStates["Walk"].AddTransition(MyObject.MyAnimator.AvailableStates["Death"], new TimeSpan(0, 0, 0, 0, 100));

                MyObject.MyAnimator.AvailableStates["Idle"].AddTransition(MyObject.MyAnimator.AvailableStates["PickTrash"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["IdleSM"].AddTransition(MyObject.MyAnimator.AvailableStates["PickTrash"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["IdleH"].AddTransition(MyObject.MyAnimator.AvailableStates["PickTrash"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["PickTrash"].AddTransition(MyObject.MyAnimator.AvailableStates["Idle"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["PickTrash"].AddTransition(MyObject.MyAnimator.AvailableStates["IdleSM"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["PickTrash"].AddTransition(MyObject.MyAnimator.AvailableStates["IdleH"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["Walk"].AddTransition(MyObject.MyAnimator.AvailableStates["PickTrash"], new TimeSpan(0, 0, 0, 0, 200));
                MyObject.MyAnimator.AvailableStates["PickTrash"].AddTransition(MyObject.MyAnimator.AvailableStates["Walk"], new TimeSpan(0, 0, 0, 0, 200));

                currentIdleAnimID = "Idle";

                MyObject.MyAnimator.CurrentState = MyObject.MyAnimator.AvailableStates[currentIdleAnimID];
                //MyObject.MyAnimator.SetBlendState("Walk");
            }

            if (!TrashSoupGame.Instance.EditorMode)
            {
                CommentaryCue = AudioManager.Instance.GetCue("commentary");
            }

            WalkSoundEffect = TrashSoupGame.Instance.Content.Load<SoundEffect>(@"Audio/Character/walk");
            Walk = WalkSoundEffect.CreateInstance();
            Walk.Volume = 0.1f;
            //Change speed of sound
            //Walk.Pitch += 0.7f;
        }

        public override void OnTrigger(GameObject other)
        { 
            if(other.Components.Exists(x => x is Weapon))
            {
                this.collisionWithWeapon = true;
                this.weapon = other;
            }
            base.OnTrigger(other);
        }

        protected Vector3 RotateAsForward(Vector3 forward, Vector3 rotation)
        {
            this.prevRotY = this.rotY;
            this.rotY = (float)Math.Atan2(-forward.X, forward.Z);
            this.rotY = CurveAngle(prevRotY, rotY, ROTATION_SPEED);
            return rotation = new Vector3(rotation.X, rotY, rotation.Z);
        }

        private float CurveAngle(float from, float to, float step)
        {
            if (step == 0) return from;
            if (from == to || step == 1) return to;

            Vector2 fromVector = new Vector2((float)Math.Cos(from), (float)Math.Sin(from));
            Vector2 toVector = new Vector2((float)Math.Cos(to), (float)Math.Sin(to));

            Vector2 currentVector = Slerp(fromVector, toVector, step);

            float toReturn = (float)Math.Atan2(currentVector.Y, currentVector.X);

            return toReturn;
        }

        private Vector2 Slerp(Vector2 from, Vector2 to, float step)
        {
            if (step == 0) return from;
            if (from == to || step == 1) return to;

            double dot = (double)Vector2.Dot(from, to);

            // clampin'!
            if (dot > 1) dot = 1;
            else if (dot < -1) dot = -1;

            double theta = Math.Acos(dot);
            if (theta == 0) return to;

            double sinTheta = Math.Sin(theta);
            
            Vector2 toReturn = (float)(Math.Sin((1 - step) * theta) / sinTheta) * from + (float)(Math.Sin(step * theta) / sinTheta) * to;

            if(float.IsNaN(toReturn.X) || float.IsNaN(toReturn.Y))
            {
                Debug.Log("PLAYERCONTROLLER ERROR: NaN detected in Slerp()");
                throw new InvalidOperationException("PLAYERCONTROLLER ERROR: NaN detected in Slerp()");
            }

            return toReturn;
        }

        public void IncreaseHealth(float value)
        {
            if (value > MAX_HEALTH - HitPoints)
                HitPoints = MAX_HEALTH;
            else
                HitPoints += value;
        }

        public void DecreaseHealth(float value)
        {
            if(this.isBlocking)
            {
                this.isBlocking = false;
                return;
            }
            if(dodged)
            {
                return;
            }
            AudioManager.Instance.SoundBank.PlayCue("obrazenia");
            HitPoints -= value;
            if (HitPoints <= 0)
            {
                Debug.Log("YOU'RE DEAD");
                AudioManager.Instance.SoundBank.PlayCue("death");
                isDead = true;
            }

            if(mBloodParticle.Stopped)
            {
                mBloodParticle.Play();
            }

            this.Popularity -= 5.0f;
        }

        public void StartOtherState(string id)
        {
            MyObject.MyAnimator.AvailableStates[id].Animation.StopClip();
            MyObject.MyAnimator.AvailableStates[id].Animation.StartClip();
            GameManager.Instance.MovementEnabled = false;
                MyObject.MyAnimator.CurrentInterpolation = 0.0f;
                MyObject.MyAnimator.CurrentState = MyObject.MyAnimator.AvailableStates[id];
            
        }

        public void StartOtherStateNoMovementEnabledFalse(string id)
        {
            StartOtherState(id);
            GameManager.Instance.MovementEnabled = true;
        }

        public void StopOtherState()
        {
            GameManager.Instance.MovementEnabled = true;
                MyObject.MyAnimator.CurrentInterpolation = 0.0f;
                MyObject.MyAnimator.CurrentState = MyObject.MyAnimator.AvailableStates[currentIdleAnimID];
            
        }

        private string BuildCurrentAnimName()
        {
            string currentAnimName = "Attack";
            switch(this.equipment.CurrentWeapon.Type)
            {
                case WeaponType.FISTS:
                    currentAnimName += "Fist0";
                    break;
                case WeaponType.HEAVY:
                    currentAnimName += "HWeapon0";
                    break;
                case WeaponType.LIGHT:
                    currentAnimName += "MWeapon0";
                    break;
                case WeaponType.MEDIUM:
                    currentAnimName += "MWeapon0";
                    break;
                default:
                    currentAnimName += "Fists0";
                    break;
            }
            currentAnimName += this.combo.ToString();
            return currentAnimName;
        }

        public void IdleChangeHandler(Weapon wep)
        {
            WeaponType type = wep.Type;

            if(type == WeaponType.FISTS)
            {
                currentIdleAnimID = "Idle";
            }
            else if(type == WeaponType.HEAVY)
            {
                currentIdleAnimID = "IdleH";
            }
            else
            {
                currentIdleAnimID = "IdleSM";
            }

            MyObject.MyAnimator.CurrentState = MyObject.MyAnimator.AvailableStates[currentIdleAnimID];
        }

        public void LevelLoad()
        {
            Debug.Log("SAVING");
            using (FileStream fs = new FileStream("weapon.txt", FileMode.Create))
            {
                using (StreamWriter writer = new StreamWriter(fs))
                {
                    writer.Write(this.equipment.CurrentWeapon.GetType());
                }
            }
        }

        public override System.Xml.Schema.XmlSchema GetSchema()
        {
            return null;
        }

        public override void ReadXml(System.Xml.XmlReader reader)
        {
            reader.ReadStartElement();

            base.ReadXml(reader);
            //MyObject = ResourceManager.Instance.CurrentScene.GetObject(tmp);

            reader.ReadEndElement();
        }

        public override void WriteXml(System.Xml.XmlWriter writer)
        {
            base.WriteXml(writer);
        }

        #endregion
    }
}
