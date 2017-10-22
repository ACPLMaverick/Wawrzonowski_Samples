#include "GUIButton.h"
#include "GUIBatch.h"
#include "resourceManagement/ResourceManager.h"
#include "core/Engine.h"
#include "core/ITime.h"
#include "debugging/Debug.h"

namespace morphEngine
{
	using namespace resourceManagement;
	using namespace utility;
	using namespace gom;
	using namespace memoryManagement;
	using namespace debugging;

	namespace renderer
	{
		DEFINE_MENUM(morphEngine::renderer::GUIButton::State);

		GUIButton::GUIButton(const gom::ObjectInitializer& initializer) :
			GUIImage(initializer)
		{
			RegisterProperties();
		}

		GUIButton::GUIButton(const GUIButton & copy, bool bDeepCopy) : 
			GUIImage(copy, bDeepCopy)
		{
			RegisterProperties();
		}

		GUIButton::~GUIButton()
		{
		}

		void GUIButton::Initialize()
		{
			GUIImage::Initialize();

			ME_WARNING_RETURN_STATEMENT(_parentBatch.IsValid(), "Parent batch not found on button level.");
			_parentBatch->RegisterGUIButton(static_cast<Handle<GUIButton>>(_this));
		}

		void GUIButton::Shutdown()
		{
#if _DEBUG
			if (_parentBatch.IsValid())
			{
#endif // _DEBUG

				_parentBatch->DeregisterGUIButton(static_cast<Handle<GUIButton>>(_this));

#if _DEBUG
			}
#endif // _DEBUG

			GUIImage::Shutdown();
		}

		void GUIButton::Update()
		{
			if (_currentStateIndex != _nextStateIndex)
			{
				// update timer
				if (_stateTransitionHelper < _stateTransitionTime)
				{
					_stateTransitionHelper += core::Engine::GetInstance()->GetGlobalTime().GetDeltaTime();
				}
				else
				{
					_currentStateIndex = _nextStateIndex;
					if (_bGetBackFromStateToPrevious)
					{
						_nextStateIndex = static_cast<MUint32>(_previous);
						_state = _previous;
						_stateTransitionHelper = 0.0f;
						_bGetBackFromStateToPrevious = false;
						EvtClick(static_cast<Handle<GUIButton>>(_this));
					}
					else
					{
						_stateTransitionHelper = _stateTransitionTime;
					}
					_drawMaterial->SetUint(PackIndices(_currentStateIndex, _nextStateIndex), 0);
				}

				// update material data
				_drawMaterial->SetFloat(_stateTransitionHelper / _stateTransitionTime, 0);
			}
		}

		void GUIButton::SetState(renderer::GUIButton::State state)
		{
			_currentStateIndex = static_cast<MUint32>(_state);
			_nextStateIndex = static_cast<MUint32>(state);
			_drawMaterial->SetUint(PackIndices(_currentStateIndex, _nextStateIndex), 0);

			_previous = _state;
			_state = state;

			_bGetBackFromStateToPrevious = false;

			_stateTransitionHelper = 0.0f;
		}

		void GUIButton::SetStateImmediately(renderer::GUIButton::State state)
		{
			_state = state;
			_currentStateIndex = static_cast<MUint32>(_state);
			_nextStateIndex = static_cast<MUint32>(_state);
			_drawMaterial->SetUint(PackIndices(_currentStateIndex, _nextStateIndex), 0);
		}

		memoryManagement::Handle<GUIButton> GUIButton::PassFocusInDirection(Direction dir)
		{
			if (_links[static_cast<MUint8>(dir)].IsValid())
			{
				Handle<GUIButton> but = _links[static_cast<MUint8>(dir)];

				// Skipping inactive buttons on the way
				while (but->GetState() == State::INACTIVE && but.IsValid())
				{
					but = but->_links[static_cast<MUint8>(dir)];
				}

				// If no active button was found in this direction
				if(!but.IsValid())
					return memoryManagement::Handle<GUIButton>();

				but->PassFocusToThisButton(static_cast<memoryManagement::Handle<GUIButton>>(_this));
				return but;
			}
			else
			{
				return memoryManagement::Handle<GUIButton>();
			}
		}

		void GUIButton::RegisterProperties()
		{
			RegisterProperty("Link0", _links);
			RegisterProperty("Link1", _links + 1);
			RegisterProperty("Link2", _links + 2);
			RegisterProperty("Link3", _links + 3);
			RegisterProperty("Link4", _links + 4);
			RegisterProperty("StateTransitionTime", &_stateTransitionTime);
			RegisterProperty("State", &_state);
			RegisterProperty("ReceivesFocusAtStart", &_bReceivesFocusAtStart);
		}

		void GUIButton::AssignDefaultMaterial()
		{
			_drawMaterial = ResourceManager::GetInstance()->CreateMaterialInstance("DefaultGUIButtonMaterial");
		}

		inline MUint64 GUIButton::PackIndices(MUint32 stateIndex, MUint32 nextStateIndex)
		{
			MUint64 stateIndexF = static_cast<MUint64>(stateIndex);
			MUint64 nextStateIndexF = static_cast<MUint64>(nextStateIndex);
			return MUint64((nextStateIndexF << 32) | stateIndexF);
		}

		void GUIButton::Click()
		{
			// if is pressed, move to hovered state
			// if not, perform bounce to pressed state and immediately return to hovered
			if (GetState() == State::PRESSED && !_bGetBackFromStateToPrevious)
			{
				SetState(State::HOVERED);
				EvtClick(static_cast<Handle<GUIButton>>(_this));
			}
			else if(!_bGetBackFromStateToPrevious)
			{
				// defer firing event until returning begins
				SetState(State::PRESSED);
				_bGetBackFromStateToPrevious = true;
			}
		}

		void GUIButton::Press()
		{
			// keep pressed state
			if (GetState() != State::PRESSED)
			{
				SetState(State::PRESSED);
			}
			EvtPressed(static_cast<Handle<GUIButton>>(_this));
		}

		void GUIButton::Down()
		{
			//
			if (GetState() != State::PRESSED)
			{
				SetState(State::PRESSED);
				EvtDown(static_cast<Handle<GUIButton>>(_this));
			}
		}


	}
}