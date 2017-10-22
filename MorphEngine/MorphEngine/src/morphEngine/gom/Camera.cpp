#include "Camera.h"
#include "renderer/device/GraphicsDevice.h"
#include "renderer/RenderingManager.h"
#include "renderer/RendererStaticMesh.h"
#include "renderer/Skybox.h"
#include "renderer/LightAmbient.h"
#include "renderer/LightDirectional.h"
#include "renderer/LightPoint.h"
#include "renderer/LightSpot.h"
#include "renderer/LightArea.h"
#include "renderer/GUIBatch.h"
#include "renderer/shaders/ShaderFullscreen.h"
#include "core/MessageSystem.h"
#include "gom/SceneManager.h"
#include "gom/Scene.h"

namespace morphEngine
{
	using namespace core;
	using namespace renderer;
	using namespace reflection;
	using namespace renderer::device;

	namespace gom
	{
		DEFINE_MENUM(CameraRenderMode);

		Camera::Camera(const ObjectInitializer& initializer) :
			GameObjectComponent(initializer)
		{
			RegisterProperties();
		}

		Camera::Camera(const Camera & c, bool bDeepCopy) :
			GameObjectComponent(c, bDeepCopy),
			_gBuffer(c._gBuffer),
			_renderMode(c._renderMode),
			_renderTarget(c._renderTarget),
			_target(c._target),
			//_up(c._up),
			_fov(c._fov),
			_width(c._width),
			_height(c._height),
			_whRatio(c._whRatio),
			_near(c._near),
			_far(c._far),
			_matView(c._matView),
			_matProj(c._matProj),
			_matViewProj(c._matViewProj),
			_bNeedUpdateProj(c._bNeedUpdateProj),
			_bNeedUpdateView(c._bNeedUpdateView)
		{
			RegisterProperties();
		}

		void Camera::RegisterProperties()
		{
			RegisterProperty("RenderMode", &_renderMode);
			RegisterProperty("UsersPostprocesses", &_usersPostprocesses);
			RegisterProperty("Target", &_target);
			//RegisterProperty("Up", &_up);
			RegisterProperty("Fov", &_fov);
			RegisterProperty("Width", &_width);
			RegisterProperty("Height", &_height);
			RegisterProperty("Near", &_near);
			RegisterProperty("Far", &_far);
			RegisterProperty("UsingTarget", &_bUsingTarget);
			RegisterProperty("ResizesWithWindow", &_bResizesWithWindow);	// saving a pointer but should be treated as a bool
		}

		Camera::~Camera()
		{
		}

		void Camera::Initialize()
		{
			GameObjectComponent::Initialize();

			_frustum.Initialize();

			if (!_skybox.IsValid())
			{
				_skybox = _owner->GetComponent<Skybox>();
				if (_skybox.IsValid())
				{
					_skybox->SetCamera(static_cast<Handle<Camera>>(_this));
				}
			}

			_whRatio = ((float)_width) / ((float)_height);
			if (_renderMode == CameraRenderMode::DEFERRED)
			{
				_gBuffer.Initialize(static_cast<memoryManagement::Handle<gom::Camera>>(_this),
					_width,
					_height);
			}
			else
			{
				if (_renderTarget.First == nullptr)
				{
					_renderTarget.First = RenderingManager::GetInstance()->CreateRenderTarget(_width,
						_height, GraphicDataFormat::UNORM_R8G8B8A8);
					_renderTarget.First->Initialize();
					_renderTarget.First->SetResizesWithWindow(_bResizesWithWindow);
				}

				_renderTarget.Second = RenderingManager::GetInstance()->CreateRenderTarget(_width,
					_height, GraphicDataFormat::UNORM_R8G8B8A8);
				_renderTarget.Second->Initialize();
				_renderTarget.Second->SetResizesWithWindow(_bResizesWithWindow);
			}

			_owner->GetTransform()->EvtTransformChanged += new Event<void, memoryManagement::Handle<Transform>>::ClassDelegate<Camera>(_this, &Camera::OnTransformChanged);

			if (_bResizesWithWindow && _resizeDelegate == nullptr)
			{
				_resizeDelegate = new Event<void, MInt32, MInt32>::ClassDelegate<Camera>(_this, &Camera::OnWindowResize);
				MessageSystem::OnWindowResize += _resizeDelegate;
			}

			UpdateView();
			UpdateProjection();
			MergeMatrices();
			UpdateFrustum();

			_bNeedUpdateProj = false;
			_bNeedUpdateView = false;
		}

		void Camera::Shutdown()
		{
			GameObjectComponent::Shutdown();
			if (_renderMode == CameraRenderMode::DEFERRED)
			{
				_gBuffer.Shutdown();
			}
			else if (_renderMode == CameraRenderMode::FORWARD)
			{

			}

			_frustum.Shutdown();
		}

		void Camera::Update()
		{
			bool mergeMatrices = false;

			if (_bNeedUpdateView)
			{
				UpdateView();
				_bNeedUpdateView = false;
				mergeMatrices = true;
			}

			if (_bNeedUpdateProj)
			{
				UpdateProjection();
				_bNeedUpdateProj = false;
				mergeMatrices = true;
			}

			if (mergeMatrices)
			{
				MergeMatrices();
			}
		}

		void Camera::Draw(const Handle<Camera> camera) const
		{
			// TODO: implement
		}

		void Camera::DrawRenderers(RenderSet& renderSet)
		{
			if (_renderMode == CameraRenderMode::DEFERRED)
			{
				RenderingManager::GetInstance()->GetDevice()->PushRenderTarget();
				
				_gBuffer.SetDrawMeshes();
				
				// "Classic" way. Left here for tetin.
				//for (auto it = renderSet.RenderersOpaqueFrontToBack.GetBegin(); it.IsValid(); ++it)
				//{
				//	if ((*it).RendererPtr->GetVisible())
				//	{
				//		(*it).RendererPtr->SetMaterialShaderDeferredPass(static_cast<Handle<Camera>>(_this), (*it).MaterialIndex);
				//		(*it).RendererPtr->SetMaterialDataDeferred((*it).MaterialIndex);
				//		(*it).RendererPtr->DrawSingleDeferred(static_cast<Handle<Camera>>(_this), (*it).MaterialIndex);
				//	}
				//}


				MArray<MArray<MArray<RenderSet::RendererIndexPair>>>& perShader = renderSet.RenderersOpaqueGrouped;

				for (MSize i = 0; i < perShader.GetSize(); ++i)
				{
					MArray<MArray<RenderSet::RendererIndexPair>>& perMaterial = perShader[i];
					perMaterial[0][0].RendererPtr->SetMaterialShaderDeferredPass(static_cast<Handle<Camera>>(_this), perMaterial[0][0].MaterialIndex);
					
					for (MSize j = 0; j < perMaterial.GetSize(); ++j)
					{
						MArray<RenderSet::RendererIndexPair>& perRenderer = perMaterial[j];
						perRenderer[0].RendererPtr->SetMaterialDataDeferred(perRenderer[0].MaterialIndex);

						for (MSize k = 0; k < perRenderer.GetSize(); ++k)
						{
							perRenderer[k].RendererPtr->DrawSingleDeferred(static_cast<Handle<Camera>>(_this), perRenderer[k].MaterialIndex);
						}
					}

					perMaterial[0][0].RendererPtr->UnsetMaterialShaderPass();
				}
				
				_gBuffer.SetDrawLights();

				_gBuffer.SetDrawLightsAmbient();
				if (renderSet.LightAmb != nullptr)
				{
					_gBuffer.DrawLightAmbient(*renderSet.LightAmb);
				}
				
				_gBuffer.SetDrawLightsDirectional();
				for (auto iter = renderSet.LightsDirectional.GetBegin(); iter.IsValid(); ++iter)
				{
					_gBuffer.DrawLightDirectional(**iter);
				}
				
				_gBuffer.SetDrawLightsPoint();
				for (auto iter = renderSet.LightsPoint.GetBegin(); iter.IsValid(); ++iter)
				{
					_gBuffer.DrawLightPoint(**iter);
				}

				_gBuffer.SetDrawLightsSpot();
				for (auto iter = renderSet.LightsSpot.GetBegin(); iter.IsValid(); ++iter)
				{
					_gBuffer.DrawLightSpot(**iter);
				}

				_gBuffer.SetDrawLightsArea();
				for (auto iter = renderSet.LightsArea.GetBegin(); iter.IsValid(); ++iter)
				{
					_gBuffer.DrawLightArea(**iter);
				}
				
				// Draw AfterLighting
				_gBuffer.SetDrawAfterLighting();

					// skybox
					if (_skybox.IsValid())
					{
						_skybox->Draw(static_cast<Handle<Camera>>(_this));
					}

					// Draw debug-related things
					if (RenderingManager::GetInstance()->GetDrawOptionEnabled(RenderingManager::DRAWOPTIONS_BOUNDS) && 
						renderSet.RenderersOpaqueFrontToBack.GetSize() > 0)
					{
						RenderingManager::GetInstance()->GetDevice()->PushBlendState();
						RenderingManager::GetInstance()->GetDevice()->SetBlendState(device::GraphicsDevice::BlendState::ALPHA);
						renderSet.RenderersOpaqueFrontToBack[0].RendererPtr->GetBounds().SetShader();

						for (auto it = renderSet.RenderersOpaqueFrontToBack.GetBegin(); it.IsValid(); ++it)
						{
							if ((*it).RendererPtr->GetVisible())
							{
								for (MSize i = 0; i < (*it).RendererPtr->GetMaterialCount(); ++i)
								{
									(*it).RendererPtr->GetBounds().Draw(static_cast<Handle<Camera>>(_this));
								}
							}
						}

						_frustum.Draw(static_cast<Handle<Camera>>(_this));

						for (auto it = renderSet.LightsPoint.GetBegin(); it.IsValid(); ++it)
						{
							if ((*it)->GetVisible())
							{
								(*it)->GetBounds().Draw(static_cast<Handle<Camera>>(_this));
							}
						}
						for (auto it = renderSet.LightsSpot.GetBegin(); it.IsValid(); ++it)
						{
							if ((*it)->GetVisible())
							{
								(*it)->GetBounds().Draw(static_cast<Handle<Camera>>(_this));
							}
						}
						for (auto it = renderSet.LightsArea.GetBegin(); it.IsValid(); ++it)
						{
							if ((*it)->GetVisible())
							{
								(*it)->GetBounds().Draw(static_cast<Handle<Camera>>(_this));
							}
						}

						RenderingManager::GetInstance()->GetDevice()->PopBlendState();
						renderSet.RenderersOpaqueFrontToBack[0].RendererPtr->GetBounds().UnsetShader();
					}

				// Draw Transparent
				_gBuffer.SetDrawTransparentAlphaBlend();
				bool bLastAdditive = false;
				for (auto it = renderSet.RenderersTransparentBackToFront.GetBegin(); it.IsValid(); ++it)
				{
					if (bLastAdditive && 
						(*it).RendererPtr->GetMaterial((*it).MaterialIndex)->GetAlphaMode() == assetLibrary::MMaterial::AlphaMode::MODE_ALPHA_BLEND ||
						(*it).RendererPtr->GetMaterial((*it).MaterialIndex)->GetAlphaMode() == assetLibrary::MMaterial::AlphaMode::MODE_CUTOUT)
					{
						_gBuffer.SetDrawTransparentAlphaBlend();
						bLastAdditive = false;
					}
					else if (!bLastAdditive &&
						(*it).RendererPtr->GetMaterial((*it).MaterialIndex)->GetAlphaMode() == assetLibrary::MMaterial::AlphaMode::MODE_ADDITIVE)
					{
						_gBuffer.SetDrawTransparentAdditive();
						bLastAdditive = true;
					}

					(*it).RendererPtr->SetMaterialShaderPass(static_cast<Handle<Camera>>(_this), (*it).MaterialIndex);
					(*it).RendererPtr->SetMaterialData((*it).MaterialIndex);
					(*it).RendererPtr->DrawSingle(static_cast<Handle<Camera>>(_this), (*it).MaterialIndex);
				}

				// Draw postprocessed GUIs
				for (auto iter = renderSet.GUIBatchesBeforePostprocessing.GetBegin(); iter.IsValid(); ++iter)
				{
					if ((*iter)->GetVisible())
					{
						(*iter)->Draw();
					}
				}

				// Drawing Postprocesses.
				_gBuffer.SetDrawPostprocesses();


				for (auto it = _basePostprocesses.GetBegin(); it.IsValid(); ++it)
				{
					_gBuffer.DrawPostprocess(*(*it));
				}


				for (auto it = _usersPostprocesses.GetBegin(); it.IsValid(); ++it)
				{
					_gBuffer.DrawPostprocess(*(*it));
				}

				_gBuffer.GetOutputBuffer()->SetAsOutput();
				
				// Draw non-postprocessed GUIs
				for (auto iter = renderSet.GUIBatchesAfterPostprocessing.GetBegin(); iter.IsValid(); ++iter)
				{
					if ((*iter)->GetVisible())
					{
						(*iter)->Draw(true);
					}
				}

				_gBuffer.EndFrame();

				RenderingManager::GetInstance()->GetDevice()->PopRenderTarget();
			}
			else
			{
				RenderingManager::GetInstance()->GetDevice()->PushRenderTarget();

				_renderTarget->SetAsOutput();
				_renderTarget->ClearWithDepth(utility::MColor::Black);

				for (auto it = renderSet.RenderersOpaqueFrontToBack.GetBegin(); it.IsValid(); ++it)
				{
					if ((*it).RendererPtr->GetVisible())
					{
						for (MSize i = 0; i < (*it).RendererPtr->GetMaterialCount(); ++i)
						{
							(*it).RendererPtr->SetMaterialShaderPass(static_cast<Handle<Camera>>(_this), i);
							(*it).RendererPtr->SetMaterialData(i);
							(*it).RendererPtr->DrawSingle(static_cast<morphEngine::memoryManagement::Handle<Camera>>(_this), i);
						}
					}
				}

				// Draw postprocessed GUIs
				for (auto iter = renderSet.GUIBatchesBeforePostprocessing.GetBegin(); iter.IsValid(); ++iter)
				{
					if ((*iter)->GetVisible())
					{
						(*iter)->Draw();
					}
				}
				
				// Drawing Postprocesses. In this drawing mode, only simplest of simplest postprocesses can be used (which doesn't require GBuffer data)
				RenderingManager::GetInstance()->GetDevice()->ClearBoundRenderTargets(1);
				RenderingManager::GetInstance()->GetDevice()->ClearBoundShaderResourcesPS(0, 1);
				
				for (auto it = _basePostprocesses.GetBegin(); it.IsValid(); ++it)
				{
					(*it)->Set();
					(*it)->SetLocal(_renderTarget.First);
					_renderTarget.Second->SetAsOutput();
					RenderingManager::GetInstance()->GetDevice()->Draw(3);
					_renderTarget.Flip();
					RenderingManager::GetInstance()->GetDevice()->ClearBoundRenderTargets(1);
				}

				for (auto it = _usersPostprocesses.GetBegin(); it.IsValid(); ++it)
				{
					(*it)->Set();
					(*it)->SetLocal(_renderTarget.First);
					_renderTarget.Second->SetAsOutput();
					RenderingManager::GetInstance()->GetDevice()->Draw(3);
					_renderTarget.Flip();
					RenderingManager::GetInstance()->GetDevice()->ClearBoundRenderTargets(1);
				}

				// Draw non-postprocessed GUIs
				for (auto iter = renderSet.GUIBatchesAfterPostprocessing.GetBegin(); iter.IsValid(); ++iter)
				{
					if ((*iter)->GetVisible())
					{
						(*iter)->Draw(true);
					}
				}

				RenderingManager::GetInstance()->GetDevice()->PopRenderTarget();
			}
		}

		Camera & Camera::operator=(const Camera & c)
		{
			GameObjectComponent::operator=(c);

			_renderMode = c._renderMode;
			_renderTarget = c._renderTarget;
			_target = c._target;
			//_up = c._up;
			_fov = c._fov;
			_whRatio = c._whRatio;
			_near = c._near;
			_far = c._far;
			_matView = c._matView;
			_matProj = c._matProj;
			_matViewProj = c._matViewProj;
			_bNeedUpdateProj = c._bNeedUpdateProj;
			_bNeedUpdateView = c._bNeedUpdateView;

			return *this;
		}

		void Camera::OnTransformChanged(memoryManagement::Handle<Transform> transform)
		{
			// not deferring the change because of flexibility

			UpdateView();
			MergeMatrices();
			_frustum.UpdateToWorld(transform);
		}

		void Camera::OnWindowResize(MInt32 nWidth, MInt32 nHeight)
		{
			_width = static_cast<MUint16>(nWidth);
			_height = static_cast<MUint16>(nHeight);
			_whRatio = ((float)nWidth) / ((float)nHeight);
			_bNeedUpdateProj = true;
		}

		void Camera::ClearTarget()
		{
			_owner->GetTransform()->SetDirection(_target - _owner->GetTransform()->GetPosition());
			_target = MVector3::Zero; 
			_bUsingTarget = false;
		}

		void Camera::SetResizesWithWindow(bool option)
		{
			if (!_bResizesWithWindow && option)
			{
				_resizeDelegate = new Event<void, MInt32, MInt32>::ClassDelegate<Camera>(_this, &Camera::OnWindowResize);
				MessageSystem::OnWindowResize += _resizeDelegate;
				_bResizesWithWindow = true;
			}
			else if (_bResizesWithWindow && !option)
			{
				MessageSystem::OnWindowResize -= _resizeDelegate;
				delete _resizeDelegate;
				_resizeDelegate = nullptr;
				_bResizesWithWindow = false;
			}
		}

		void Camera::SetRenderMode(CameraRenderMode mode)
		{
			_renderMode = mode;
			if (mode == CameraRenderMode::FORWARD && _renderTarget.First == nullptr && _renderTarget.Second == nullptr && _gBuffer.IsInitialized())
			{
				_gBuffer.Shutdown();

				_renderTarget.First = RenderingManager::GetInstance()->CreateRenderTarget(_width,
					_height, GraphicDataFormat::UNORM_R8G8B8A8);
				_renderTarget.First->Initialize();
				_renderTarget.First->SetResizesWithWindow(_bResizesWithWindow);

				_renderTarget.Second = RenderingManager::GetInstance()->CreateRenderTarget(_width,
					_height, GraphicDataFormat::UNORM_R8G8B8A8);
				_renderTarget.Second->Initialize();
				_renderTarget.Second->SetResizesWithWindow(_bResizesWithWindow);
			}
			else if (mode == CameraRenderMode::DEFERRED && _renderTarget.First != nullptr && _renderTarget.Second != nullptr && !_gBuffer.IsInitialized())
			{
				RenderingManager::GetInstance()->DestroyRenderTarget(_renderTarget.First);
				RenderingManager::GetInstance()->DestroyRenderTarget(_renderTarget.Second);
				_renderTarget.First = nullptr;
				_renderTarget.Second = nullptr;

				_gBuffer.Initialize(static_cast<memoryManagement::Handle<gom::Camera>>(_this),
					_width,
					_height);
			}
		}

		void Camera::SetWidthHeight(MUint16 width, MUint16 height)
		{
			_width = width;
			_height = height;
			_whRatio = ((float)_width) / ((float)_height);
			
			UpdateProjection();
			MergeMatrices();

			//GameObjectComponent::Shutdown();	// ???!!!!!!!!!!!!!!!
			if (_renderMode == CameraRenderMode::DEFERRED)
			{
				if (_gBuffer.IsInitialized())
				{
					_gBuffer.Resize(_width, _height);
				}
			}
			else if (_renderMode == CameraRenderMode::FORWARD)
			{
				if (_renderTarget.First != nullptr && _renderTarget.Second != nullptr)
				{
					_renderTarget.First->Resize(_width, _height);
					_renderTarget.Second->Resize(_width, _height);
				}
			}
		}

		const renderer::RenderTarget2D * Camera::GetRenderTarget() const
		{
			if (_renderMode == CameraRenderMode::DEFERRED)
			{
				return _gBuffer.GetOutputBuffer();
			}
			else
			{
				return _renderTarget.First;
			}
		}

		inline void Camera::UpdateView()
		{
			//_up = MVector3::Cross(GetDirection(), GetRight()).Normalized();

			if (_bUsingTarget)
			{
				_matView = MMatrix::LookAt(GetPosition(), _target, GetUp());
			}
			else
			{
				_matView = MMatrix::LookAt(GetPosition(), GetPosition() + GetDirection(), GetUp());
				//_matView = MMatrix::LookAt(MVector3(-15.0f, 15.0f, -15.0f), MVector3::Zero, MVector3::Up);
			}
		}

		inline void Camera::UpdateProjection()
		{
			_matProj = MMatrix::PerspectiveFovLH(_fov, _whRatio, _near, _far);
		}

		inline void Camera::MergeMatrices()
		{
			_matViewProj = _matView * _matProj;
		}

		inline void Camera::UpdateFrustum()
		{
			// create frustum planes and points using only projection matrix
			// Top, Bottom, Left, Right, Front, Back
			// ForwardTopLeft, ForwardBottomLeft, ForwardBottomRight, ForwardTopRight, RearTopLeft, RearBottomLeft, RearBottomRight, RearTopRight

			// NDC to view space
			_frustum.PointsLocal[0] = MVector3(-1.0f, 1.0f, 1.0f);
			_frustum.PointsLocal[1] = MVector3(-1.0f, -1.0f, 1.0f);
			_frustum.PointsLocal[2] = MVector3(1.0f, -1.0f, 1.0f);
			_frustum.PointsLocal[3] = MVector3(1.0f, 1.0f, 1.0f);  
			_frustum.PointsLocal[4] = MVector3(-1.0f, 1.0f, -1.0f);
			_frustum.PointsLocal[5] = MVector3(-1.0f, -1.0f, -1.0f);
			_frustum.PointsLocal[6] = MVector3(1.0f, -1.0f, -1.0f);
			_frustum.PointsLocal[7] = MVector3(1.0f, 1.0f, -1.0f);



			auto planeFromFourPoints = [](const MVector3& one, const MVector3& two, const MVector3& three, const MVector3& four,
				MVector3& outNormal, MVector3& outCenter)
			{
				MVector3 dir1((two - one).Normalized());
				MVector3 dir2((two - three).Normalized());
				outNormal = MVector3::Cross(dir1, dir2).Normalized();
				outCenter = (one + two + three + four) * 0.25f;
			};



			for (MSize i = 0; i < 8; ++i)
			{
				MVector4 projToView(_matProj.Inverted().Transposed() * MVector4(_frustum.PointsLocal[i], 1.0f));
				_frustum.PointsLocal[i] = projToView / projToView.W;
			}

			// Calculate planes.

			// Top
			planeFromFourPoints(
				_frustum.PointsLocal[4],
				_frustum.PointsLocal[7],
				_frustum.PointsLocal[3],
				_frustum.PointsLocal[0],
				_frustum.Planes[0].NormalLocal,
				_frustum.Planes[0].CenterLocal
				);

			// Bottom
			planeFromFourPoints(
				_frustum.PointsLocal[1],
				_frustum.PointsLocal[2],
				_frustum.PointsLocal[6],
				_frustum.PointsLocal[5],
				_frustum.Planes[1].NormalLocal,
				_frustum.Planes[1].CenterLocal
			);

			// Left
			planeFromFourPoints(
				_frustum.PointsLocal[1],
				_frustum.PointsLocal[5],
				_frustum.PointsLocal[4],
				_frustum.PointsLocal[0],
				_frustum.Planes[2].NormalLocal,
				_frustum.Planes[2].CenterLocal
			);

			// Right
			planeFromFourPoints(
				_frustum.PointsLocal[2],
				_frustum.PointsLocal[3],
				_frustum.PointsLocal[7],
				_frustum.PointsLocal[6],
				_frustum.Planes[3].NormalLocal,
				_frustum.Planes[3].CenterLocal
			);

			// Front
			planeFromFourPoints(
				_frustum.PointsLocal[1],
				_frustum.PointsLocal[0],
				_frustum.PointsLocal[3],
				_frustum.PointsLocal[2],
				_frustum.Planes[4].NormalLocal,
				_frustum.Planes[4].CenterLocal
			);

			// Back
			planeFromFourPoints(
				_frustum.PointsLocal[5],
				_frustum.PointsLocal[6],
				_frustum.PointsLocal[7],
				_frustum.PointsLocal[4],
				_frustum.Planes[5].NormalLocal,
				_frustum.Planes[5].CenterLocal
			);

			// view space to world space
			_frustum.UpdateToWorld(_owner->GetTransform());
		}

		void Camera::RegisterComponent()
		{
			if (_bIsMain)
			{
				SceneManager::GetInstance()->GetScene()->RegisterCameraAsMain(static_cast<Handle<Camera>>(_this));
			}
			else
			{
				SceneManager::GetInstance()->GetScene()->RegisterCamera(static_cast<Handle<Camera>>(_this));
			}
		}

		void Camera::DeregisterComponent()
		{
			SceneManager::GetInstance()->GetScene()->DeregisterCamera(static_cast<Handle<Camera>>(_this));
		}
	}
}