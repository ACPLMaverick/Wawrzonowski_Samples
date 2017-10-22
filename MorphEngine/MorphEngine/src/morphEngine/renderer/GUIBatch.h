#pragma once

#include "gom/GameObjectComponent.h"
#include "renderer/GUIImage.h"
#include "renderer/GUIButton.h"
#include "utility/MVector.h"
#include "utility/MMatrix.h"
#include "utility/Collections.h"
#include "core/Delegate.h"

namespace morphEngine
{
	namespace assetLibrary
	{
		class MMesh;
		class MMaterial;
	}

	namespace gom
	{
		class Camera;
		class Transform;
	}

	class GUIImage;

	namespace renderer
	{

		/// <summary>
		/// GUIBatch is used to draw GUI elements onto some render target in some way. In default render target is scene main camera's.
		/// Also used to control GUI buttons (focus, clicking, etc.) but need registered input component.
		/// </summary>
		class GUIBatch :
			public gom::GameObjectComponent
		{
			ME_TYPE

		protected:

#pragma region Protected

			utility::MArray<memoryManagement::Handle<GUIImage>> _registeredImages;
			utility::MArray<memoryManagement::Handle<GUIButton>> _registeredButtons;	// remember that these button handles are only used for logic
			memoryManagement::Handle<gom::Camera> _renderCamera;

			memoryManagement::Handle<GUIButton> _currentlyFocused;
			memoryManagement::Handle<GUIButton> _firstToFocus;
			MFloat32 _directionChangeDelay = 0.5f;
			MFloat32 _directionChangeTimer = _directionChangeDelay;

			bool _bDrawBeforePostprocessing;

#pragma endregion

#pragma region Functions Protected

			GUIBatch(const gom::ObjectInitializer& initializer);

			void RecalculateImageOrderInQueue(memoryManagement::Handle<gom::Transform> trToChange);

#pragma endregion

		public:

#pragma region Events Public

			core::Event<void, memoryManagement::Handle<gom::Camera>> EvtCameraChanged;

#pragma endregion

#pragma region Functions Public
			
			GUIBatch(const GUIBatch& copy, bool bDeepCopy = true);
			virtual ~GUIBatch();

			virtual void Initialize() override;
			virtual void Shutdown() override;

			virtual void Update() override;

			// Draws all registered GUIImage children onto camera's render target, in the order of registration.
			virtual void Draw(bool bSetRenderTarget = false) const;

			void RegisterGUIImage(memoryManagement::Handle<GUIImage> image);
			void DeregisterGUIImage(memoryManagement::Handle<GUIImage> image);
			void RegisterGUIButton(memoryManagement::Handle<GUIButton> button);
			void DeregisterGUIButton(memoryManagement::Handle<GUIButton> button);

			bool ButtonMoveFocus(GUIButton::Direction direction);
			bool ButtonClickFocused();
			bool ButtonPressFocused();
			bool ButtonDownFocused();
			void ButtonRemoveFocus();

			inline void SetRenderCamera(memoryManagement::Handle<gom::Camera> cam) { _renderCamera = cam; EvtCameraChanged(_renderCamera); }
			inline void SetDrawBeforePostprocessing(bool val) { _bDrawBeforePostprocessing = val; }
			inline void SetDirectionChangeDelay(MFloat32 delay) { _directionChangeDelay = delay; }

			inline memoryManagement::Handle<gom::Camera> GetRenderCamera() const { return _renderCamera; }
			inline bool GetDrawBeforePostprocessing() const { return _bDrawBeforePostprocessing; }
			inline MFloat32 GetDirectionChangeDelay() const { return _directionChangeDelay; }

#pragma endregion

		};

	}
}