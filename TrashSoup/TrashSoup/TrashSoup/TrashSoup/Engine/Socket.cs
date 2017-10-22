using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrashSoup.Engine
{
    public class Socket : ObjectComponent
    {
        #region variables

        private Matrix boneTransform;

        #endregion

        #region properties

        public GameObject Carrier { get; private set; }
        public GameObject Carried { get; private set; }
        public CustomModel ModelInCarrier { get; private set; }
        public string BoneName { get; private set; }
        public Matrix BoneTransform
        {
            get
            {
                ModelInCarrier.GetBoneMatrix(BoneName, out boneTransform);
                Vector3 trans, scl;
                Quaternion quat;
                boneTransform.Decompose(out scl, out quat, out trans);
                boneTransform = Matrix.CreateFromQuaternion(quat) * Matrix.CreateTranslation(trans * Carrier.MyTransform.Scale);
                
                return boneTransform;
            }
            private set
            {
                boneTransform = value;
            }
        }

        #endregion

        #region methods

        public Socket()
        {

        }

        public Socket(GameObject carrier, GameObject carried, CustomModel modelInHandler, string boneName)
        {
            this.Carrier = carrier;
            this.Carried = carried;
            this.ModelInCarrier = modelInHandler;
            this.BoneName = boneName;

            if (ModelInCarrier == null)
            {
                foreach (ObjectComponent comp in Carrier.Components)
                {
                    if (comp is CustomModel)
                    {
                        this.ModelInCarrier = comp as CustomModel;
                        break;
                    }
                }
            }
        }

        #endregion

        public override void Update(GameTime gameTime)
        {
            // do nothing
        }

        public override void Draw(Camera cam, Microsoft.Xna.Framework.Graphics.Effect effect, GameTime gameTime)
        {
            // do nothing
        }

        protected override void Start()
        {
            
        }
    }
}
