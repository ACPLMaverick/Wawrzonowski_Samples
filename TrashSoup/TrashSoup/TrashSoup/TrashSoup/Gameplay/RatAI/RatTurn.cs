using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine.AI.BehaviorTree;
using TrashSoup.Engine;
using System.Xml.Serialization;
using Microsoft.Xna.Framework;

namespace TrashSoup.Gameplay.RatAI
{
    class RatTurn : TrashSoup.Engine.AI.BehaviorTree.Action
    {
        private float timer = 0.0f;
        private float turnTime = 2.0f;
        private float rotationTimer = 0.0f;
        private float movementSpeed = 0.6f;
        private Vector3 movementVector = Vector3.Right;
        private float rotationSpeed = 0.25f;
        private Vector3 newRot;
        private Vector3 oldRot;
        private bool firstTime = true;

        public override TickStatus Tick(GameTime gameTime, out INode running)
        {
            if (this.blackboard.GetBool("Dead"))
            {
                running = null;
                firstTime = true;
                return TickStatus.FAILURE;
            }

            if(this.blackboard.GetBool("TargetSeen") || this.blackboard.GetBool("Idle") || !this.blackboard.GetBool("ShouldTurn"))
            {
                running = null;
                firstTime = true;
                return TickStatus.FAILURE;
            }

            if(timer > turnTime)
            {
                timer = 0.0f;
                firstTime = true;
                rotationTimer = 0.0f;
                this.blackboard.SetBool("ShouldTurn", false);
                running = null;
                return TickStatus.SUCCESS;
            }

            if (this.blackboard.Owner.UniqueID == 1550)
            {
                Debug.Log("Should turn");
            }

            timer += gameTime.ElapsedGameTime.Milliseconds * 0.001f;

            if (firstTime)
            {
                this.blackboard.Owner.MyAnimator.ChangeState("Walk");
            }

            if (rotationTimer > 0.5f || firstTime)
            {
                rotationTimer = 0.0f;
                firstTime = false;
                this.rotationSpeed = 0.35f;
                oldRot = this.blackboard.Owner.MyTransform.Rotation;
                newRot = oldRot + Vector3.Up * rotationSpeed;
            }
            rotationTimer += gameTime.ElapsedGameTime.Milliseconds * 0.001f;
            this.blackboard.Owner.MyTransform.Rotation = Vector3.Lerp(oldRot, newRot, rotationTimer * 2.0f);
            this.movementVector = Vector3.Transform(Vector3.Right, Matrix.CreateRotationY(-this.blackboard.Owner.MyTransform.Rotation.Y));
            this.blackboard.Owner.MyTransform.Position += this.movementVector * gameTime.ElapsedGameTime.Milliseconds * 0.001f * this.movementSpeed;
            running = this;
            return TickStatus.RUNNING;
        }
    }
}
