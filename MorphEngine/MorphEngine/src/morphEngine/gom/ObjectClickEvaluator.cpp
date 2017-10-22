#include "ObjectClickEvaluator.h"
#include "renderer/device/GraphicsDevice.h"
#include "renderer/RenderingManager.h"
#include "resourceManagement/ResourceManager.h"
#include "renderer/RenderTarget2D.h"
#include "renderer/shaders/ShaderGameObjectID.h"
#include "core/Engine.h"
#include "gom/Camera.h"
#include "renderer/RendererStaticMesh.h"
#include "gom/SceneManager.h"

#include "debugging/Debug.h"

namespace morphEngine
{
	using namespace renderer;
	using namespace renderer::device;
	using namespace renderer::shaders;
	using namespace debugging;
	using namespace resourceManagement;
	using namespace assetLibrary;

	namespace gom
	{
		ObjectClickEvaluator::ObjectClickEvaluator(const ObjectClickEvaluator& c, bool bDeepCopy) :
			GameObjectComponent(c, bDeepCopy),
			_renderTarget(c._renderTarget),
			_stagingTexture(c._stagingTexture),
			_shader(c._shader),
			_bHasDrawnThisFrame(c._bHasDrawnThisFrame)
		{
		}

		ObjectClickEvaluator & ObjectClickEvaluator::operator=(const ObjectClickEvaluator & c)
		{
			GameObjectComponent::operator=(c);
			_renderTarget = c._renderTarget;
			_stagingTexture = c._stagingTexture;
			_shader = c._shader;
			_bHasDrawnThisFrame = c._bHasDrawnThisFrame;
			return *this;
		}

		void ObjectClickEvaluator::Initialize()
		{
			GameObjectComponent::Initialize();

			Window* wnd = core::Engine::GetInstance()->GetWindow();

			_renderTarget = RenderingManager::GetInstance()->CreateRenderTarget(wnd->GetWidth() / RT_SIZE_DIVISOR, wnd->GetHeight() / RT_SIZE_DIVISOR,
				GraphicDataFormat::UINT_R32G32, true);
			_renderTarget->SetResizesWithWindow(true);
			_renderTarget->Initialize();
			_stagingTexture = RenderingManager::GetInstance()->GetDevice()->CreateStagingTexture2DInstance();
			_stagingTexture->Initialize(0, 0, 1, 1, GraphicDataFormat::UINT_R32G32);
			_shader = reinterpret_cast<ShaderGameObjectID*>(ResourceManager::GetInstance()->GetShader("ShaderGameObjectID"));
			_shader->SetupMaterialInterface(_tempMat);
			_tempMat.SetShader(_shader);
			_tempMat.Initialize();
		}

		void ObjectClickEvaluator::Shutdown()
		{
			GameObjectComponent::Shutdown();
			RenderingManager::GetInstance()->DestroyRenderTarget(_renderTarget);
			_renderTarget = nullptr;

			_stagingTexture->Shutdown();
			delete _stagingTexture;
			_stagingTexture = nullptr;
		}

		void ObjectClickEvaluator::Update()
		{
			GameObjectComponent::Update();

			if (_bHasDrawnThisFrame)
				_bHasDrawnThisFrame = false;

			//Handle<GameObject> h = GetClickedGameObject(SceneManager::GetInstance()->GetScene()->GetMainCameraPair(), utility::MVector2(400.0f, 300.0f));
		}

		Handle<GameObject> ObjectClickEvaluator::GetClickedGameObject(RenderSetCameraPair & camera, utility::MVector2 mousePosImageSpace)
		{
			if (!_bHasDrawnThisFrame)
			{
				DrawGameObjects(camera);
				_bHasDrawnThisFrame = true;
			}

			MUint64 id = GetClickedID(camera.Set, mousePosImageSpace);

			Handle<GameObject> h;
			if (!camera.Set.GameObjectsPerID.TryGetValue(id, &h))
			{
				ME_WARNING(false, "ObjectClickEvaluator: Clicked object not found in object map.");
				return Handle<GameObject>();
			}

			return h;
		}

		void ObjectClickEvaluator::DrawGameObjects(RenderSetCameraPair & camera)
		{

			RenderingManager::GetInstance()->GetDevice()->PushRenderTarget();
			_renderTarget->SetAsOutput();
			_renderTarget->ClearWithDepth(); 

			RenderingManager::GetInstance()->GetDevice()->PushBlendState();
			RenderingManager::GetInstance()->GetDevice()->SetBlendState(GraphicsDevice::BlendState::SOLID);

			RenderingManager::GetInstance()->GetDevice()->PushViewport();
			Window* wnd = core::Engine::GetInstance()->GetWindow();
			GraphicsDevice::Viewport vp;
			vp._width = static_cast<MFloat32>(wnd->GetWidth() / RT_SIZE_DIVISOR);
			vp._height = static_cast<MFloat32>(wnd->GetHeight() / RT_SIZE_DIVISOR);
			RenderingManager::GetInstance()->GetDevice()->SetViewport(vp);

			_tempMat.SetPass(*camera.Cam);

			// opaque
			MSize i = 0;
			for (auto it = camera.Set.RenderersOpaqueFrontToBack.GetBegin(); it.IsValid(); ++it, ++i)
			{
				MUint64 id = (*it).RendererPtr->GetOwner()->GetUniqueID();
				_tempMat.SetUint(id, 0);
				for(MSize i = 0; i < (*it).RendererPtr->GetSubmeshCount(); ++i)
					(*it).RendererPtr->DrawSingleWithExternMaterial(camera.Cam, &_tempMat, i);
			}

			// alpha
			i = 0;
			for (auto it = camera.Set.RenderersTransparentBackToFront.GetBegin(); it.IsValid(); ++it, ++i)
			{
				MUint64 id = (*it).RendererPtr->GetOwner()->GetUniqueID();
				_tempMat.SetUint(id, 0);
				for (MSize i = 0; i < (*it).RendererPtr->GetSubmeshCount(); ++i)
					(*it).RendererPtr->DrawSingleWithExternMaterial(camera.Cam, &_tempMat, i);
			}

			// effects

			// gui

			// TODOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
			
			RenderingManager::GetInstance()->GetDevice()->PopViewport();
			RenderingManager::GetInstance()->GetDevice()->PopBlendState();

			RenderingManager::GetInstance()->GetDevice()->PopRenderTarget();
		}

		MUint64 ObjectClickEvaluator::GetClickedID(RenderSet& set, utility::MVector2 mousePosImageSpace)
		{
			mousePosImageSpace /= static_cast<MFloat32>(RT_SIZE_DIVISOR);
			/*
			Window* wnd = core::Engine::GetInstance()->GetWindow();
			_stagingTexture->OriginX = 0;
			_stagingTexture->OriginY = 0;
			_stagingTexture->RectWidth = wnd->GetWidth() / RT_SIZE_DIVISOR;
			_stagingTexture->RectHeight = wnd->GetHeight() / RT_SIZE_DIVISOR;
			_stagingTexture->UpdateRectSize();
			_stagingTexture->CopyTextureData(_renderTarget->GetResource());
			MUint64* ptr;
			_stagingTexture->Map(reinterpret_cast<void**>(&ptr));

			for (MSize i = 0; i < _stagingTexture->RectWidth * _stagingTexture->RectHeight; ++i)
			{
				if (ptr[i] != 0)
				{
					Debug::Log(ELogType::LOG, MString::FromInt(ptr[i]));
				}
			}

			_stagingTexture->Unmap();
			*/

			_stagingTexture->OriginX = static_cast<MInt32>(mousePosImageSpace.X);
			_stagingTexture->OriginY = static_cast<MInt32>(mousePosImageSpace.Y);
			_stagingTexture->CopyTextureData(_renderTarget->GetResource());

			MUint64* pixel;
			MUint64 id = -1;
			_stagingTexture->Map(reinterpret_cast<void**>(&pixel));
			id = (*pixel);
			_stagingTexture->Unmap();

			ME_ASSERT_S(id != -1);

			return id;
		}
	}
}