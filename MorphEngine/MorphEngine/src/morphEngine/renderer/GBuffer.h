#pragma once

#include "utility/MVector.h"
#include "RenderTarget2D.h"
#include "memoryManagement/Handle.h"

namespace morphEngine
{
	namespace gom
	{
		class Camera;
	}

	namespace renderer
	{
		class LightAmbient;
		class LightDirectional;
		class LightPoint;
		class LightSpot;
		class LightArea;

		namespace shaders
		{
			class ShaderDeferredLightAmbient;
			class ShaderDeferredLightDirectional;
			class ShaderDeferredLightArea;
			class ShaderDeferredLightPoint;
			class ShaderDeferredLightSpot;
			struct ShaderFullscreenGlobalData;
			class ShaderFullscreen;
			typedef ShaderFullscreen Postprocess;
		}

		/*
		
		Buffers carry following data:
		- ColorBuffer: 8 bpc, RGB (color); A (specular)
		- NormalBuffer: 32 bpc, RGB (normal), A (gloss)
		- PositionBuffer: 32 bpc, RGB (world position), A (specular color, packed 8bpc)

		*/
		class GBuffer
		{

#pragma region Protected

			memoryManagement::Handle<gom::Camera> _camera;

			// constists also of depth buffer
			RenderTarget2D* _colorBuffer = nullptr;
			RenderTarget2D* _normalBuffer = nullptr;
			RenderTarget2D* _positionBuffer = nullptr;

			RenderTarget2DPair _outputBuffers;

			shaders::ShaderDeferredLightAmbient* _shaderAmbient;
			shaders::ShaderDeferredLightDirectional* _shaderDirectional;
			shaders::ShaderDeferredLightPoint* _shaderPoint;
			shaders::ShaderDeferredLightSpot* _shaderSpot;
			shaders::ShaderDeferredLightArea* _shaderArea;

			bool _bInputBuffers = false;

#pragma endregion

#pragma region Functions Protected

			inline void FillShaderGlobalData(shaders::ShaderFullscreenGlobalData& outData);

#pragma endregion

		public:

#pragma region Functions Public

			GBuffer();
			GBuffer(const GBuffer& copy);
			~GBuffer();

			GBuffer& operator=(const GBuffer& copy);

			void Initialize(memoryManagement::Handle<gom::Camera> camera, MUint16 width, MUint16 height, RenderTarget2D** inputBuffers = nullptr);
			void Shutdown();

			ME_DEPRECATED_MSG("TODO: Implement")
			void SaveBuffers();


			// Rendering pipeline

			void SetDrawMeshes();

			
			void SetDrawLights();
			
			void SetDrawLightsAmbient();
			void SetDrawLightsDirectional();
			void SetDrawLightsPoint();
			void SetDrawLightsSpot();
			void SetDrawLightsArea();

			// Before calling this function, SetDrawLightsAmbient must be called.
			void DrawLightAmbient(const LightAmbient& lightAmbient);
			// Before calling this function, SetDrawLightsDirectional must be called.
			void DrawLightDirectional(const LightDirectional& lightDirectional);
			// Before calling this function, SetDrawLightsPoint must be called.
			void DrawLightPoint(const LightPoint& lightPoint);
			// Before calling this function, SetDrawLightsSpot must be called.
			void DrawLightSpot(const LightSpot& lightSpot);
			// Before calling this function, SetDrawLightsArea must be called.
			void DrawLightArea(const LightArea& lightArea);

			// Sets Output rendertarget with Color depth buffer
			void SetDrawAfterLighting();
			void SetDrawTransparentAlphaBlend();
			void SetDrawTransparentAdditive();
			
			void SetDrawPostprocesses();

			void DrawPostprocess(const shaders::Postprocess& postprocess);

			
			void EndFrame();


			void PutToBackBuffer() const;

			// ACHTUNG! All data will be lost.
			void Resize(MUint16 nWidth, MUint16 nHeight);

			// /////

#pragma region Accessors

			inline bool IsInitialized() const { return _colorBuffer != nullptr; }

			inline const RenderTarget2D* GetColorBuffer() const { return _colorBuffer; }
			inline const RenderTarget2D* GetNormalBuffer() const { return _normalBuffer; }
			inline const RenderTarget2D* GetPositionBuffer() const { return _positionBuffer; }
			inline const RenderTarget2D* GetOutputBuffer() const { return _outputBuffers.First; }

			inline const MUint16 GetWidth() const { return _colorBuffer->GetWidth(); }
			inline const MUint16 GetHeight() const { return _colorBuffer->GetHeight(); }

#pragma endregion

#pragma endregion
		};
	}
}