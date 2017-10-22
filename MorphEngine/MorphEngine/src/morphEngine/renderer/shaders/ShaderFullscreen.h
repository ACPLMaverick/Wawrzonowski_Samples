#pragma once
#include "Shader.h"
#include "renderer/RenderingManager.h"
#include "renderer/device/GraphicsDevice.h"

namespace morphEngine
{
	namespace renderer
	{
		class RenderTarget2D;

		namespace shaders
		{
			struct ShaderFullscreenGlobalData : public ShaderGlobalData
			{
				const RenderTarget2D* TextureColorDepth = nullptr;
				const RenderTarget2D* TextureNormal = nullptr;
				const RenderTarget2D* TexturePosition = nullptr;
			};

			struct ShaderPostprocessSimpleData : public ShaderLocalData
			{
				const RenderTarget2D* TextureInput;
			};

			class ShaderFullscreen :
				public Shader
			{
			public:

				static const char* TYPE_NAME;

			protected:

				__declspec(align(16))
					struct LightGlobalData
				{
					utility::MVector3 CameraPosition;
				};

				device::VertexShaderResource* _vs;
				device::PixelShaderResource* _ps;
				device::ConstantBufferResource* _cGlobal;

				virtual inline void GetPixelShaderName(utility::MString& name) = 0;

			public:

				ShaderFullscreen();
				virtual ~ShaderFullscreen();

				virtual void Initialize() override;
				virtual void Shutdown() override;

				virtual void Set(MUint8 passIndex = 0) const;
				virtual void Set(const ShaderGlobalData& globalData, const ShaderLocalData& localData, MUint8 passIndex = 0) const override;
				virtual void SetGlobal(const ShaderGlobalData& globalData, MUint8 passIndex = 0) const override;
				virtual void SetLocal(const ShaderLocalData& localData) const = 0;
				// For postprocesses requiring only input buffers and acquiring additional data from within.
				virtual void SetLocal(const RenderTarget2D* input) const;

				virtual void UnsetGlobal() const override;
			};

			typedef ShaderFullscreen Postprocess;
		}
	}
}