#pragma once

#include "gom/GameObjectComponent.h"
#include "utility/MVector.h"
#include "utility/MMatrix.h"
#include "utility/MArray.h"
#include "renderer/GBuffer.h"
#include "gom/Transform.h"
#include "core/Delegate.h"
#include "gom/Scene.h"
#include "renderer/BoundsFrustum.h"

namespace morphEngine
{
	namespace renderer
	{
		class RendererStaticMesh;
		class RenderTarget2D;
		class Skybox;
		
		namespace shaders
		{
			class ShaderFullscreen;
			typedef ShaderFullscreen Postprocess;
		}
	}

	namespace gom
	{
		DECLARE_MENUM(CameraRenderMode, FORWARD, DEFERRED);

		class Camera :
			public GameObjectComponent
		{
			ME_TYPE

		protected:

#pragma region Protected

			// drawing

			renderer::GBuffer _gBuffer;

			renderer::BoundsFrustum _frustum;

			memoryManagement::Handle<renderer::Skybox> _skybox;

			CameraRenderMode _renderMode = CameraRenderMode::FORWARD;
			renderer::RenderTarget2DPair _renderTarget = nullptr;

			// base postprocesses initialized in camera ctor
			utility::MArray<renderer::shaders::Postprocess*> _basePostprocesses;
			// user-set postprocesses
			utility::MArray<renderer::shaders::Postprocess*> _usersPostprocesses;

			// view-generating

			MMatrix _matViewProj;
			MMatrix _matView;
			MMatrix _matProj;

			MVector3 _target = MVector3::Zero;
			//MVector3 _up = MVector3::Up;

			core::Event<void, memoryManagement::Handle<Transform>>::ClassDelegate<Camera>* _transformChangedDelegate = nullptr;
			core::Event<void, MInt32, MInt32>::ClassDelegate<Camera>* _resizeDelegate = nullptr;

			float _fov = 75.0f;
			float _whRatio = 1.0f;
			float _near = 0.1f;
			float _far = 1000.0f;
			MUint16 _width = 512;
			MUint16 _height = 512;

			bool _bUsingTarget = false;
			bool _bNeedUpdateView = false;
			bool _bNeedUpdateProj = false;
			bool _bResizesWithWindow = false;
			bool _bIsMain = true;

#pragma endregion

#pragma region Functions Protected

			Camera(const ObjectInitializer& initializer);
			Camera(const Camera& c, bool bDeepCopy = true);

			virtual void RegisterProperties() override;

			inline void UpdateView();
			inline void UpdateProjection();
			inline void MergeMatrices();
			inline void UpdateFrustum();

#pragma endregion

			virtual void RegisterComponent() override;
			virtual void DeregisterComponent() override;
		
		public:

#pragma region Functions Public

			virtual ~Camera();

			// Called post-constructor and post-initialize-properties
			virtual void Initialize();
			// Called pre-deallocation
			virtual void Shutdown();

			virtual void Update() override;
			// This should only draw some gizmo in editor mode.
			virtual void Draw(const Handle<Camera> camera) const override;

			virtual void DrawRenderers(RenderSet& renderSet);

			virtual Camera& operator=(const Camera& camera);

#pragma region EventHandlers

			void OnTransformChanged(memoryManagement::Handle<Transform> transform);
			void OnWindowResize(MInt32 nWidth, MInt32 nHeight);

#pragma endregion

#pragma region GettersSetters

			void ClearTarget();
			inline void SetSkybox(memoryManagement::Handle<renderer::Skybox> sb) { _skybox = sb; }
			inline void SetTarget(const MVector3& target) { _target = target; _bNeedUpdateView = true; _bUsingTarget = true; }
			//inline void SetUp(const MVector3& up) { _up = up; _bNeedUpdateView = true; }
			inline void SetFOV(float fov) { _fov = fov; _bNeedUpdateProj = true; }
			inline void SetNear(float nearPlane) { _near = nearPlane; _bNeedUpdateProj = true; }
			inline void SetFar(float farPlane) { _far = farPlane; _bNeedUpdateProj = true; }
			void SetResizesWithWindow(bool option);
			void SetRenderMode(CameraRenderMode mode);
			void SetWidthHeight(MUint16 width, MUint16 height);
			inline void SetIsMain(bool main) { _bIsMain = main; }

			inline CameraRenderMode GetRenderMode() const { return _renderMode; }
			const renderer::RenderTarget2D* GetRenderTarget() const;

			inline utility::MArray<renderer::shaders::Postprocess*>& GetUsersPostprocesses() { return _usersPostprocesses; }

			inline const renderer::BoundsFrustum& GetBounds() const { return _frustum; }
			inline memoryManagement::Handle<renderer::Skybox> GetSkybox() const { return _skybox; }
			inline const MMatrix& GetMatViewProj() const { return _matViewProj; }
			inline const MMatrix& GetMatView() const { return _matView; }
			inline const MMatrix& GetMatProj() const { return _matProj; }
			inline MVector3 GetPosition() const { return _owner->GetTransform()->GetPosition(); }
			inline MVector3 GetTarget() const { if (_bUsingTarget) { return _target; } else { return GetPosition() + GetDirection(); } }
			inline MVector3 GetUp() const { return _owner->GetTransform()->GetUp(); }
			inline MVector3 GetDirection() const { if (_bUsingTarget) { return (GetTarget() - GetPosition()).Normalized(); } else { return _owner->GetTransform()->GetDirection(); } }
			inline MVector3 GetRight() const { return _owner->GetTransform()->GetRight(); }
			inline float GetFOV() const { return _fov; }
			inline float GetNearPlane() const { return _near; }
			inline float GetFarPlane() const { return _far; }
			inline float GetWidthHeightRatio() const { return _whRatio; }
			inline MUint16 GetWidth() const { return _width; }
			inline MUint16 GetHeight() const { return _height; }
			inline bool GetResizesWithWindow() { return _resizeDelegate != nullptr; }
			inline bool GetIsMain() { return _bIsMain; }

#pragma endregion

#pragma endregion
		};

	}
}