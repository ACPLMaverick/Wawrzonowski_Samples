#include "GBuffer.h"
#include "renderer/shaders/ShaderDeferredLightAmbient.h"
#include "renderer/shaders/ShaderDeferredLightDirectional.h"
#include "renderer/shaders/ShaderDeferredLightPoint.h"
#include "renderer/shaders/ShaderDeferredLightSpot.h"
#include "renderer/shaders/ShaderDeferredLightArea.h"
#include "resourceManagement/ResourceManager.h"
#include "gom/Camera.h"

#define Device() RenderingManager::GetInstance()->GetDevice()

namespace morphEngine
{
	using namespace resourceManagement;

	namespace renderer
	{
		using namespace device;
		using namespace shaders;

		GBuffer::GBuffer()
		{
		}

		GBuffer::GBuffer(const GBuffer & copy) :
			_camera(copy._camera),
			_colorBuffer(copy._colorBuffer),
			_normalBuffer(copy._normalBuffer),
			_positionBuffer(copy._positionBuffer),
			_outputBuffers(copy._outputBuffers),
			_shaderAmbient(copy._shaderAmbient),
			_shaderDirectional(copy._shaderDirectional),
			_shaderPoint(copy._shaderPoint),
			_shaderSpot(copy._shaderSpot),
			_shaderArea(copy._shaderArea),
			_bInputBuffers(copy._bInputBuffers)
		{
		}

		GBuffer::~GBuffer()
		{
			if (_colorBuffer != nullptr)
			{
				Shutdown();
			}
		}

		GBuffer & GBuffer::operator=(const GBuffer & copy)
		{
			ME_ASSERT(false, "GBuffer: Assignment operation not allowed.");
			return *this;
		}

		void GBuffer::Initialize(memoryManagement::Handle<gom::Camera> camera, MUint16 width, MUint16 height, RenderTarget2D** inputBuffers)
		{
			_camera = camera;

			if (inputBuffers != nullptr)
			{
				_bInputBuffers = true;
				_colorBuffer = inputBuffers[0];
				_normalBuffer = inputBuffers[1];
				_positionBuffer = inputBuffers[2];
				_outputBuffers.First = inputBuffers[3];
			}
			else
			{
				_bInputBuffers = false;
				_colorBuffer = RenderingManager::GetInstance()->CreateRenderTarget(width, height, GraphicDataFormat::UNORM_R8G8B8A8, true, GraphicDataFormat::FLOAT_R32);
				_normalBuffer = RenderingManager::GetInstance()->CreateRenderTarget(width, height, GraphicDataFormat::FLOAT_R32G32B32A32, false);
				_positionBuffer = RenderingManager::GetInstance()->CreateRenderTarget(width, height, GraphicDataFormat::FLOAT_R32G32B32A32, false);
				_outputBuffers.First = RenderingManager::GetInstance()->CreateRenderTarget(width, height, GraphicDataFormat::UNORM_R8G8B8A8, false);
				_colorBuffer->Initialize();
				_normalBuffer->Initialize();
				_positionBuffer->Initialize();
				_outputBuffers.First->Initialize();
				_colorBuffer->SetResizesWithWindow(true);
				_normalBuffer->SetResizesWithWindow(true);
				_positionBuffer->SetResizesWithWindow(true);
				_outputBuffers.First->SetResizesWithWindow(true);
			}
			_outputBuffers.Second = RenderingManager::GetInstance()->CreateRenderTarget(width, height, GraphicDataFormat::UNORM_R8G8B8A8, false);
			_outputBuffers.Second->Initialize();
			_outputBuffers.Second->SetResizesWithWindow(true);

			_shaderAmbient = reinterpret_cast<ShaderDeferredLightAmbient*>(ResourceManager::GetInstance()->GetShader("ShaderDeferredLightAmbient"));
			_shaderDirectional = reinterpret_cast<ShaderDeferredLightDirectional*>(ResourceManager::GetInstance()->GetShader("ShaderDeferredLightDirectional"));
			_shaderPoint = reinterpret_cast<ShaderDeferredLightPoint*>(ResourceManager::GetInstance()->GetShader("ShaderDeferredLightPoint"));
			_shaderSpot = reinterpret_cast<ShaderDeferredLightSpot*>(ResourceManager::GetInstance()->GetShader("ShaderDeferredLightSpot"));
			_shaderArea = reinterpret_cast<ShaderDeferredLightArea*>(ResourceManager::GetInstance()->GetShader("ShaderDeferredLightArea"));
		}

		void GBuffer::Shutdown()
		{

		}

		void GBuffer::SaveBuffers()
		{
			_bInputBuffers = true;
		}

		void GBuffer::SetDrawMeshes()
		{
			Device()->SetBlendState(GraphicsDevice::BlendState::SOLID);
			RenderTarget2D::SetMultipleAsOutput(&_colorBuffer, 3, 0);
			_colorBuffer->ClearWithDepth();
			_normalBuffer->ClearNoDepth();
			_positionBuffer->ClearNoDepth();
			_outputBuffers.First->ClearNoDepth();
			_outputBuffers.Second->ClearNoDepth();
		}

		void GBuffer::SetDrawLights()
		{
			Device()->ClearBoundRenderTargets(3);
			Device()->ClearBoundVertexIndexBuffers();
			Device()->SetBlendState(GraphicsDevice::BlendState::ADDITIVE);
		}

		void GBuffer::SetDrawLightsAmbient()
		{
			ShaderFullscreenGlobalData sgData;
			FillShaderGlobalData(sgData);
			_shaderAmbient->SetGlobal(sgData);
		}

		void GBuffer::SetDrawLightsDirectional()
		{
			ShaderFullscreenGlobalData sgData;
			FillShaderGlobalData(sgData);
			_shaderDirectional->SetGlobal(sgData);
		}

		void GBuffer::SetDrawLightsPoint()
		{
			ShaderFullscreenGlobalData sgData;
			FillShaderGlobalData(sgData);
			_shaderPoint->SetGlobal(sgData);
		}

		void GBuffer::SetDrawLightsSpot()
		{
			ShaderFullscreenGlobalData sgData;
			FillShaderGlobalData(sgData);
			_shaderSpot->SetGlobal(sgData);
		}

		void GBuffer::SetDrawLightsArea()
		{
			ShaderFullscreenGlobalData sgData;
			FillShaderGlobalData(sgData);
			_shaderArea->SetGlobal(sgData);
		}

		void GBuffer::DrawLightAmbient(const LightAmbient & lightAmbient)
		{
			ShaderDeferredLightAmbientCustomData cd;
			cd.Light = &lightAmbient;
			
			_shaderAmbient->SetLocal(cd);
			_outputBuffers.First->SetAsOutput();

			Device()->Draw(3);
		}

		void GBuffer::DrawLightDirectional(const LightDirectional & lightDirectional)
		{
			ShaderDeferredLightDirectionalCustomData cd;
			cd.Light = &lightDirectional;

			_shaderDirectional->SetLocal(cd);
			_outputBuffers.First->SetAsOutput();

			Device()->Draw(3);
		}

		void GBuffer::DrawLightPoint(const LightPoint & lightPoint)
		{
			ShaderDeferredLightPointCustomData cd;
			cd.Light = &lightPoint;

			_shaderPoint->SetLocal(cd);
			_outputBuffers.First->SetAsOutput();

			Device()->Draw(3);
		}

		void GBuffer::DrawLightSpot(const LightSpot & lightSpot)
		{
			ShaderDeferredLightSpotCustomData cd;
			cd.Light = &lightSpot;
			_shaderSpot->SetLocal(cd);
			_outputBuffers.First->SetAsOutput();

			Device()->Draw(3);
		}

		void GBuffer::DrawLightArea(const LightArea & lightArea)
		{
			ShaderDeferredLightAreaCustomData cd;
			cd.Light = &lightArea;
			_shaderArea->SetLocal(cd);
			_outputBuffers.First->SetAsOutput();

			Device()->Draw(3);
		}

		void GBuffer::SetDrawAfterLighting()
		{
			Device()->ClearBoundShaderResourcesPS(0, 4);
			Device()->SetBlendState(GraphicsDevice::BlendState::SOLID);
			_outputBuffers.First->SetAsOutputWithExternalDepthBuffer(_colorBuffer->GetDepthResource());
		}

		void GBuffer::SetDrawTransparentAlphaBlend()
		{
			Device()->SetBlendState(GraphicsDevice::BlendState::ALPHA);
		}

		void GBuffer::SetDrawTransparentAdditive()
		{
			Device()->SetBlendState(GraphicsDevice::BlendState::ADDITIVE);
		}

		void GBuffer::SetDrawPostprocesses()
		{
			Device()->ClearBoundRenderTargets(1);
			_colorBuffer->SetAsInputWithDepth(0, 1);
			_normalBuffer->SetAsInputPS(2);
			_positionBuffer->SetAsInputPS(3);
			Device()->SetBlendState(GraphicsDevice::BlendState::SOLID);
		}

		void GBuffer::DrawPostprocess(const shaders::Postprocess& postprocess)
		{
			ShaderFullscreenGlobalData fsData;
			FillShaderGlobalData(fsData);
			postprocess.SetGlobal(fsData);
			postprocess.SetLocal(_outputBuffers.First);
			_outputBuffers.Second->SetAsOutput();
			Device()->Draw(3);
			Device()->ClearBoundRenderTargets(1);
			_outputBuffers.Flip();
		}

		void GBuffer::EndFrame()
		{
			Device()->ClearBoundShaderResourcesPS(0, 5);
			Device()->ClearBoundRenderTargets(1);
		}

		void GBuffer::PutToBackBuffer() const
		{
			Device()->CopyToBackBuffer(_outputBuffers.First->GetResource());
		}

		void GBuffer::Resize(MUint16 nWidth, MUint16 nHeight)
		{
			_colorBuffer->Resize(nWidth, nHeight);
			_normalBuffer->Resize(nWidth, nHeight);
			_positionBuffer->Resize(nWidth, nHeight);
			_outputBuffers.First->Resize(nWidth, nHeight);
			_outputBuffers.Second->Resize(nWidth, nHeight);
		}
		inline void GBuffer::FillShaderGlobalData(ShaderFullscreenGlobalData & outData)
		{
			outData.Camera = _camera.GetPointer();
			outData.TextureColorDepth = _colorBuffer;
			outData.TextureNormal = _normalBuffer;
			outData.TexturePosition = _positionBuffer;
		}
	}
}
