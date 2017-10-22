#pragma once

#include "renderer/GUIImage.h"
#include "core/Delegate.h"

namespace morphEngine
{
	namespace renderer
	{
		class GUIBatch;

		class GUIButton :
			public GUIImage
		{
			ME_TYPE

		friend class GUIBatch;

		public:

			enum class Direction
			{
				NEXT,	// when TAB or some other pass-through button is pressed
				TOP,
				RIGHT,
				BOTTOM,
				LEFT
			};

			DECLARE_MENUM(State, ACTIVE, HOVERED, PRESSED, INACTIVE);

		protected:

#pragma region Protected

			/* 
				Material parameters: 
					GUIButtonShader,
					4 textures for each state,
					4 colors for each state,
					MInt32 current state index
					MInt32 nextState index. Can be -1 and is then indicating that no lerping is needed
					MFloat32 for final lerp values between the two states
			*/

			memoryManagement::Handle<GUIButton> _links[5];
			MFloat32 _stateTransitionTime = 0.25f;
			renderer::GUIButton::State _state = State::ACTIVE;
			renderer::GUIButton::State _previous = State::ACTIVE;
			bool _bReceivesFocusAtStart = false;
			bool _bGetBackFromStateToPrevious = false;

			MFloat32 _stateTransitionHelper = FLT_MAX;
			MUint32 _currentStateIndex = 0;
			MUint32 _nextStateIndex = 0;

#pragma endregion

#pragma region Functions Protected

			GUIButton(const gom::ObjectInitializer& initializer);

			virtual void RegisterProperties() override;
			virtual void AssignDefaultMaterial() override;
			inline MUint64 PackIndices(MUint32 stateIndex, MUint32 nextStateIndex);

			void Click();
			void Press();
			void Down();

#pragma endregion

		public:

#pragma region Events Public

			// Same as EvtUp
			core::Event<void, memoryManagement::Handle<GUIButton>> EvtClick;
			core::Event<void, memoryManagement::Handle<GUIButton>> EvtPressed;
			core::Event<void, memoryManagement::Handle<GUIButton>> EvtDown;

#pragma endregion

#pragma region Functions Public
			
			GUIButton(const GUIButton& copy, bool bDeepCopy = true);
			virtual ~GUIButton();

			virtual void Initialize() override;
			virtual void Shutdown() override;
			virtual void Update() override;

			inline void SetLink(memoryManagement::Handle<GUIButton> button, Direction dir) { _links[static_cast<MUint8>(dir)] = button; }
			void SetState(renderer::GUIButton::State state);
			void SetStateImmediately(renderer::GUIButton::State state);
			inline void SetTransitionTime(MFloat32 stt) { _stateTransitionTime = stt; }
			inline void SetReceivesFocusAtStart(bool bReceivesFocus) { _bReceivesFocusAtStart = bReceivesFocus; }
			inline void Deactivate() { SetState(renderer::GUIButton::State::INACTIVE); }
			inline void Activate() { SetState(renderer::GUIButton::State::ACTIVE); }

			inline memoryManagement::Handle<GUIButton> GetLink(Direction dir) const { return _links[static_cast<MUint8>(dir)]; }
			inline bool GetIsDirectionValid(Direction dir) const { return _links[static_cast<MUint8>(dir)].IsValid(); }
			inline renderer::GUIButton::State GetState() const { return _state; }
			inline MFloat32 GetTransitionTime() const { return _stateTransitionTime; }
			inline bool GetReceivesFocusAtStart() const { return _bReceivesFocusAtStart; }

			// Returns: handle to a button focus has been passed to. Invalid handle if it has not been passed.
			memoryManagement::Handle<GUIButton> PassFocusInDirection(Direction dir);

			// Skips inactive buttons in this direction
			inline void PassFocusToThisButton(memoryManagement::Handle<GUIButton> prevFocusOwner)
			{
				ME_WARNING(prevFocusOwner->GetState() == renderer::GUIButton::State::HOVERED, "Passing focus from button that does not have focus.");
				prevFocusOwner->SetState(renderer::GUIButton::State::ACTIVE);
				this->SetState(renderer::GUIButton::State::HOVERED);
			}

#pragma endregion

		};

	}
}