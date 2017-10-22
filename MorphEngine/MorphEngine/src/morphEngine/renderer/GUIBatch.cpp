#include "GUIBatch.h"
#include "gom/SceneManager.h"
#include "gom/Transform.h"
#include "renderer/GUIImage.h"
#include "core/Delegate.h"
#include "gom/Camera.h"
#include "renderer/RenderingManager.h"
#include "assetLibrary/MMaterial.h"
#include "core/Engine.h"
#include "core/ITime.h"
#include "debugging/Debug.h"

namespace morphEngine
{
	using namespace utility;
	using namespace memoryManagement;
	using namespace gom;
	using namespace core;

	namespace renderer
	{

		GUIBatch::GUIBatch(const gom::ObjectInitializer& initializer) :
			GameObjectComponent(initializer), _bDrawBeforePostprocessing(false)
		{
		}

		GUIBatch::GUIBatch(const GUIBatch & copy, bool bDeepCopy) : 
			gom::GameObjectComponent(copy, bDeepCopy),
			_registeredImages(copy._registeredImages),
			_renderCamera(copy._renderCamera),
			_bDrawBeforePostprocessing(copy._bDrawBeforePostprocessing)
		{
		}

		GUIBatch::~GUIBatch()
		{
		}

		void GUIBatch::Initialize()
		{
			gom::GameObjectComponent::Initialize();

			if (!_renderCamera.IsValid())
			{
				_renderCamera = gom::SceneManager::GetInstance()->GetScene()->GetMainCameraPair().Cam;
			}
			
		}

		void GUIBatch::Shutdown()
		{
			for (auto it = _registeredImages.GetBegin(); it.IsValid(); ++it)
			{
				(*it)->GetOwner()->GetTransform()->EvtParentChanged -= new Event<void, Handle<Transform>>::ClassDelegate<GUIBatch>(_this, &GUIBatch::RecalculateImageOrderInQueue);
			}

			_registeredImages.Clear();
			gom::GameObjectComponent::Shutdown();
		}

		void GUIBatch::Update()
		{
			if (_directionChangeTimer < _directionChangeDelay)
			{
				_directionChangeTimer += core::Engine::GetInstance()->GetGlobalTime().GetDeltaTime();
			}
		}

		void GUIBatch::Draw(bool bSetRenderTarget) const
		{
			if (_registeredImages.GetSize() == 0)
				return;

			if (bSetRenderTarget)
			{
				RenderingManager::GetInstance()->GetDevice()->PushRenderTarget();
				_renderCamera->GetRenderTarget()->SetAsOutput();
			}

			RenderingManager::GetInstance()->GetDevice()->PushDepthStencilState();
			RenderingManager::GetInstance()->GetDevice()->PushBlendState();
			RenderingManager::GetInstance()->GetDevice()->SetDepthStencilState(device::GraphicsDevice::DepthStencilState::DEPTH_DISABLED_STENCIL_DISABLED);
			RenderingManager::GetInstance()->GetDevice()->SetBlendState(device::GraphicsDevice::BlendState::ALPHA);

			_registeredImages[0]->GetMaterial()->SetPass(*_renderCamera);

			for (auto it = const_cast<GUIBatch*>(this)->_registeredImages.GetBegin(); it.IsValid(); ++it)
			{
				(*it)->Draw();
			}

			if (bSetRenderTarget)
			{
				RenderingManager::GetInstance()->GetDevice()->PopRenderTarget();
			}

			RenderingManager::GetInstance()->GetDevice()->PopBlendState();
			RenderingManager::GetInstance()->GetDevice()->PopDepthStencilState();

			RenderingManager::GetInstance()->GetDevice()->CopyToBackBuffer(_renderCamera->GetRenderTarget()->GetResource());
		}

		void GUIBatch::RegisterGUIImage(memoryManagement::Handle<GUIImage> image)
		{
			ME_WARNING_RETURN_STATEMENT(!_registeredImages.Contains(image), "GUIBatch: Trying to register an image more than once.");
			_registeredImages.Add(image);
			image->GetOwner()->GetTransform()->EvtParentChanged += new Event<void, Handle<Transform>>::ClassDelegate<GUIBatch>(_this, &GUIBatch::RecalculateImageOrderInQueue);
		}

		void GUIBatch::DeregisterGUIImage(memoryManagement::Handle<GUIImage> image)
		{
			ME_ASSERT_S(_registeredImages.Contains(image));
			_registeredImages.Remove(image);
			image->GetOwner()->GetTransform()->EvtParentChanged -= new Event<void, Handle<Transform>>::ClassDelegate<GUIBatch>(_this, &GUIBatch::RecalculateImageOrderInQueue);
		}

		void GUIBatch::RegisterGUIButton(memoryManagement::Handle<GUIButton> button)
		{
			ME_WARNING_RETURN_STATEMENT(!_registeredButtons.Contains(button), "GUIBatch: Trying to register a button more than once.");

			if (button->GetReceivesFocusAtStart() && !_currentlyFocused.IsValid())
			{
				_currentlyFocused = button;
				_firstToFocus = button;
				button->SetStateImmediately(GUIButton::State::HOVERED);
			}

			_registeredButtons.Add(button);
		}

		void GUIBatch::DeregisterGUIButton(memoryManagement::Handle<GUIButton> button)
		{
			ME_ASSERT_S(_registeredButtons.Contains(button));
			_registeredButtons.Remove(button);
		}

		bool GUIBatch::ButtonMoveFocus(GUIButton::Direction direction)
		{
			if (_registeredButtons.IsEmpty())
				return false;

			if (_directionChangeTimer < _directionChangeDelay)
				return false;

			if (!_currentlyFocused.IsValid())
			{
				if (_firstToFocus.IsValid())
					_currentlyFocused = _firstToFocus;
				else
					_currentlyFocused = _registeredButtons[0];

				_currentlyFocused->SetState(GUIButton::State::HOVERED);
				_directionChangeTimer = 0.0f;

				return true;
			}

			Handle<GUIButton> newFocused = _currentlyFocused->PassFocusInDirection(direction);
			if (newFocused.IsValid())
			{
				_currentlyFocused = newFocused;

				_directionChangeTimer = 0.0f;
				return true;
			}
			else
			{
				return false;
			}
		}

		bool GUIBatch::ButtonClickFocused()
		{
			if (!_currentlyFocused.IsValid())
				return false;

			_currentlyFocused->Click();
			return true;
		}

		bool GUIBatch::ButtonPressFocused()
		{
			if (!_currentlyFocused.IsValid())
				return false;

			_currentlyFocused->Press();
			return true;
		}

		bool GUIBatch::ButtonDownFocused()
		{
			if (!_currentlyFocused.IsValid())
				return false;

			_currentlyFocused->Down();
			return true;
		}

		void GUIBatch::ButtonRemoveFocus()
		{
			_currentlyFocused->SetState(GUIButton::State::ACTIVE);
			_currentlyFocused.Invalidate();
		}

		void GUIBatch::RecalculateImageOrderInQueue(memoryManagement::Handle<Transform> trToChange)
		{
			Handle<GUIImage> gi = trToChange->GetOwner()->GetComponent<GUIImage>();
			ME_ASSERT_S(gi.IsValid());

			if (_registeredImages.GetSize() < 2)
				return;

			MSize newIndex = -1;
			MStack<Handle<Transform>> trs;
			trs.Push(_registeredImages[0]->GetOwner()->GetTransform());
			while (!trs.IsEmpty())
			{
				Handle<Transform> tr = trs.Pop();
				Handle<GUIImage> image = tr->GetOwner()->GetComponent<GUIImage>();
				if (image.IsValid())
				{
					++newIndex;

					if (image == gi)
					{
						break;
					}
				}

				for (auto it = tr->GetChildrenIterator(); it.IsValid(); ++it)
				{
					trs.Push(*it);
				}
			}

			_registeredImages.Remove(gi);
			_registeredImages.Add(gi, newIndex);				// HAHA, this will crash so much
		}
	}
}