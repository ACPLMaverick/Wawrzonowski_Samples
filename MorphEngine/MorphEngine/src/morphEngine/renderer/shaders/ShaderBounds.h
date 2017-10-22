#pragma once
#include "Shader.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			class ShaderBounds :
				public Shader
			{
			public:

				static const char* TYPE_NAME;

			protected:

				device::VertexShaderResource* _vs = nullptr;
				device::ConstantBufferResource* _cBufferVS = nullptr;
				device::GeometryShaderResource* _gs = nullptr;
				device::PixelShaderResource* _ps = nullptr;

			public:
				ShaderBounds();
				virtual ~ShaderBounds();

				virtual void Initialize();
				virtual void Shutdown();

				// Simply sets shader resources, without setting any additional data.
				virtual void Set(MUint8 passIndex = 0) const;
				// Sets shader and all its resources from custom data. Most versatile function, though least performant.
				virtual void Set(const ShaderGlobalData& globalData, const ShaderLocalData& localData, MUint8 passIndex = 0) const;
				virtual void SetGlobal(const ShaderGlobalData& globalData, MUint8 passIndex = 0) const;

				virtual void UnsetGlobal() const;

				virtual void SetLocal(const utility::MMatrix& tr, const assetLibrary::MMesh& mesh) const;
			};

		}
	}
}

