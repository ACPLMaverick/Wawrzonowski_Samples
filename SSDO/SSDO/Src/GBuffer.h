#pragma once

#include "GlobalDefines.h"
#include "Buffer.h"
#include "Renderer.h"

class Shader;
class Camera;

namespace Postprocesses
{
	class Postprocess;
}

namespace Lights
{
	class LightAmbient;
	class LightDirectional;
	class LightPoint;
}

class GBuffer
{
public:

#pragma region Structs

	struct RenderTarget
	{
		ID3D11Texture2D* Texture = nullptr;
		ID3D11RenderTargetView* View = nullptr;

		ID3D11SamplerState* Sampler = nullptr;
		ID3D11ShaderResourceView* SRV = nullptr;

		inline virtual void Shutdown()
		{
			if (Texture != nullptr)
			{
				Texture->Release();
				Texture = nullptr;
			}
			if (View != nullptr)
			{
				View->Release();
				View = nullptr;
			}
			if (Sampler != nullptr)
			{
				Sampler->Release();
				Sampler = nullptr;
			}
			if (SRV != nullptr)
			{
				SRV->Release();
				SRV = nullptr;
			}
		}

		inline void GenerateMipmaps()
		{
			ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();
			deviceContext->GenerateMips(SRV);
		}

		virtual RenderTarget& operator=(const RenderTarget& other)
		{
			Texture = other.Texture;
			View = other.View;
			Sampler = other.Sampler;
			SRV = other.SRV;
			return *this;
		}
	};

	struct RenderTargetDepthBuffer : RenderTarget
	{
		ID3D11DepthStencilView* DepthStencilView = nullptr;

		inline virtual void Shutdown() override
		{
			RenderTarget::Shutdown();
			if (DepthStencilView != nullptr)
			{
				DepthStencilView->Release();
				DepthStencilView = nullptr;
			}
		}

		virtual RenderTargetDepthBuffer& operator=(const RenderTargetDepthBuffer& other)
		{
			RenderTarget::operator=(other);
			DepthStencilView = other.DepthStencilView;
			return *this;
		}
	};

#pragma endregion

#pragma region Public Const

	static const size_t PP_BUFFER_COUNT = 4;
	static const uint32_t PP_BUFFER_SIZE_DIVISOR = 2;

#pragma endregion


protected:

#pragma region Protected

	const Camera& _camera;

	RenderTarget _color;
	RenderTarget _normal;
	RenderTargetDepthBuffer _depth;
				
	RenderTarget _outputA;
	RenderTarget _outputB;
	RenderTarget _postprocessBuffers[PP_BUFFER_COUNT];

	ID3D11BlendState* _additiveBlendState;

	Shader* _shaderDraw;
	Shader* _shaderMerge;
	Shader* _shaderLightAmbient;
	Shader* _shaderLightDirectional;
	Shader* _shaderLightPoint;

#pragma endregion

#pragma region Functions Protected

	inline void SetMapData();
	inline void UnsetMapData();
	inline void DrawFullscreenPlane();

#pragma endregion

public:

#pragma region Functions Public

	GBuffer(const Camera& camera);
	~GBuffer();

	void SetDrawMeshes();

	void SetDrawLights();

	void SetDrawLightAmbient();
	void SetDrawLightDirectional();
	void SetDrawLightPoint();

	// Before calling this function, SetDrawLightAmbient must be called.
	void DrawLightAmbient(const Lights::LightAmbient& lightAmbient);
	// Before calling this function, SetDrawLightDirectional must be called.
	void DrawLightDirectional(const Lights::LightDirectional& lightAmbient);
	// Before calling this function, SetDrawLightPoint must be called.
	void DrawLightPoint(const Lights::LightPoint& lightAmbient);

	void SetDrawPostprocesses();

	void DrawPostprocess(const Postprocesses::Postprocess& pp);

	// Postprocess interface

	inline const RenderTarget* PPGetBuffers() const { return _postprocessBuffers; }
	inline const RenderTarget* PPGetOutputBuffer() const { return &_outputA; }
	inline const RenderTarget* PPGetOutputBBuffer() const { return &_outputB; }
	void PPSetBuffersAsInput(const RenderTarget** bufferPtrArray, const int32_t slotArray[], size_t bufferCount);
	void PPSetBuffersAsOutput(const RenderTarget** bufferPtrArray, uint32_t bufferCount, const RenderTargetDepthBuffer* depthBufferPtr);
	void PPClearBuffersAsInput(const int32_t* slotArray, size_t bufferCount);
	void PPClearBuffersAsOutput(uint32_t bufferCount);

	// End postprocess interface

	void SetDrawTexts();

	void EndFrame();

	void Draw() const;

	void Merge(const GBuffer& other);

	void FlipOutputs();

	inline const RenderTarget* GetColorBuffer() { return &_color; }
	inline const RenderTarget* GetNormalDepthBuffer() { return &_normal; }
	inline const RenderTarget* GetDepthBuffer() { return &_depth; }
	inline const RenderTarget* GetOutputBufferA() { return &_outputA; }
	inline const RenderTarget* GetOutputBufferB() { return &_outputB; }

#pragma endregion

};

