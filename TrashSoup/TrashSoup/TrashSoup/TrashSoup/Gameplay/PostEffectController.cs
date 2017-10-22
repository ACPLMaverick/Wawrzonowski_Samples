using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TrashSoup.Engine;
using Microsoft.Xna.Framework;

namespace TrashSoup.Gameplay
{
    public class PostEffectController
    {
        #region constants

        private static Vector3 normalStateVignetteColor = Vector3.Zero;
        private static Vector2 normalStateVignetteSize = new Vector2(0.6f, 0.1f);
        private static float normalStateContrast = 1.0f;

        private static Vector3 woundedStateVignetteColor = new Vector3(0.35f, 0.0f, 0.0f);
        private static Vector2 woundedStateVignetteSize = new Vector2(0.55f, 0.05f);
        private static float woundedStateContrast = 2.0f;

        private static Vector3 boostStateVignetteColor = new Vector3(0.55f, 0.25f, 0.0f);
        private static Vector2 boostStateVignetteSize = new Vector2(0.6f, 0.1f);
        private static float boostStateContrast = 1.35f;

        private static float woundedBorder = 0.35f;
        private static float boostBorder = 0.5f;

        private static float brightnessAddition = 0.05f;

        #endregion

        #region variables

        private PlayerController pc;
        private DefaultPostEffect pe;
        private float maxHP;
        private float maxBoost;

        #endregion

        #region methods

        public PostEffectController(PlayerController pc, DefaultPostEffect pe, float maxHP, float maxBoost)
        {
            this.pc = pc;
            this.pe = pe;
            this.maxHP = maxHP;
            this.maxBoost = maxBoost;
        }

        public void Update(GameTime gameTime)
        {
            Vector3 normColor = normalStateVignetteColor;
            Vector2 normSize = normalStateVignetteSize;
            float normContrast = normalStateContrast;

            if(pc.Popularity >= maxBoost * boostBorder)
            {
                float lerpFactor = ((pc.Popularity - maxBoost * boostBorder) / (maxBoost * boostBorder));
                normSize = Vector2.Lerp(normSize, boostStateVignetteSize, lerpFactor);
                normColor = Vector3.Lerp(normColor, boostStateVignetteColor, lerpFactor);
                normContrast = MathHelper.Lerp(normContrast, boostStateContrast, lerpFactor);
            }

            if(pc.HitPoints <= maxHP * woundedBorder)
            {
                float lerpFactor = ((maxHP * woundedBorder - pc.HitPoints) / (maxHP * woundedBorder));
                normSize = Vector2.Lerp(normSize, woundedStateVignetteSize, lerpFactor);
                normColor = Vector3.Lerp(normColor, woundedStateVignetteColor, lerpFactor);
                normContrast =  MathHelper.Lerp(normContrast, woundedStateContrast, lerpFactor);
            }

            pe.VignetteRadius = normSize;
            pe.VignetteColor = normColor;
            pe.Contrast = normContrast;

            if (InputManager.Instance.GetKeyboardButtonDown(Microsoft.Xna.Framework.Input.Keys.OemPlus))
            {
                pe.ColorAddition = new Vector3(pe.ColorAddition.X + brightnessAddition, pe.ColorAddition.Y + brightnessAddition, pe.ColorAddition.Z + brightnessAddition);
            }

            if (InputManager.Instance.GetKeyboardButtonDown(Microsoft.Xna.Framework.Input.Keys.OemMinus))
            {
                pe.ColorAddition = new Vector3(pe.ColorAddition.X - brightnessAddition, pe.ColorAddition.Y - brightnessAddition, pe.ColorAddition.Z - brightnessAddition);
            }
        }

        #endregion
    }
}
