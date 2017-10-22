#pragma once

#include "ShaderObject.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			class ShaderLegacy :
				public ShaderObject
			{
			public:

				static const char* TYPE_NAME;

			protected:

#pragma region Static Const Protected

				static const MUint32 COUNT_LIGHT_DIRECTIONAL = 4;
				static const MUint32 COUNT_LIGHT_POINT = 4;
				static const MUint32 COUNT_LIGHT_SPOT = 4;
				static const MUint32 COUNT_LIGHT_AREA = 4;

#pragma endregion

#pragma region Structs Protected

				struct BufferDataPS
				{
					utility::MColor _colBase;
					utility::MColor _colSpec;
					MFloat32 _gloss;
					utility::MVector3 _padding;
				};

				__declspec(align(16))
				struct LightGlobalBufferData
				{
					utility::MColor _colDirectional[COUNT_LIGHT_DIRECTIONAL];
					utility::MVector4 _dirDirectional[COUNT_LIGHT_DIRECTIONAL];
					utility::MColor _colAmbient;
					MUint32 _numDirectional;
				};

				struct LightPointBufferData
				{
					MUint32 _numPoint;
					utility::MVector3 _padding;
					utility::MColor _colPoint[COUNT_LIGHT_POINT];
					utility::MVector4 _posPoint[COUNT_LIGHT_POINT];
					MFloat32 _rangePoint[COUNT_LIGHT_POINT];
				};

				struct LightSpotBufferData
				{
					MUint32 _numSpot;
					utility::MVector3 _padding;
					utility::MColor _colSpot[COUNT_LIGHT_SPOT];
					utility::MVector4 _posSpot[COUNT_LIGHT_SPOT];
					utility::MVector4 _dirSpot[COUNT_LIGHT_SPOT];
					MFloat32 _rangeSpot[COUNT_LIGHT_SPOT];
					MFloat32 _angleCosSpot[COUNT_LIGHT_SPOT];
					MFloat32 _smoothSpot[COUNT_LIGHT_SPOT];
				};

				struct LightAreaBufferData
				{
					MUint32 _numArea;
					utility::MVector3 _padding;
					utility::MColor _colArea[COUNT_LIGHT_AREA];
					utility::MVector4 _posArea[COUNT_LIGHT_AREA];
					utility::MVector4 _dirArea[COUNT_LIGHT_AREA];
					utility::MVector4 _rightArea[COUNT_LIGHT_AREA];
					utility::MVector4 _sizeRangeSmoothArea[COUNT_LIGHT_AREA];
				};

#pragma endregion

#pragma region Protected

				device::PixelShaderResource* _ps;
				device::PixelShaderResource* _psDef;
				device::ConstantBufferResource* _cBufferPS;
				device::ConstantBufferResource* _gBufferPS;
				device::ConstantBufferResource* _glBuffer;
				device::ConstantBufferResource* _plBuffer;
				device::ConstantBufferResource* _slBuffer;
				device::ConstantBufferResource* _alBuffer;

#pragma endregion

				inline void SetupLightsForward() const;

			public:
				ShaderLegacy();
				virtual ~ShaderLegacy();

				// Inherited via Shader
				virtual void Initialize() override;
				virtual void Shutdown() override;
				virtual void SetupMaterialInterface(assetLibrary::MMaterial & mat) override;
				virtual void Set(MUint8 passIndex = 0) const override { _vs->Set(); _ps->Set(); }
				virtual void Set(const ShaderGlobalData& globalData, const ShaderLocalData& localData, MUint8 passIndex = 0) const override;
				virtual void SetGlobal(const ShaderGlobalData& globalData, MUint8 passIndex = 0) const override;
				virtual void SetGlobalDeferred(const ShaderGlobalData& globalData, MUint8 passIndex = 0) const override;
				virtual void SetGlobalSkinned(const ShaderGlobalData& globalData, MUint8 passIndex = 0) const override;
				virtual void SetGlobalSkinnedDeferred(const ShaderGlobalData& globalData, MUint8 passIndex = 0) const override;
				virtual void SetMaterialWise(const assetLibrary::MMaterial& mat) const override;
				virtual void SetMaterialWiseDeferred(const assetLibrary::MMaterial& mat) const override;
				virtual void SetLocal(const assetLibrary::MMaterial& mat, const gom::Camera & camera, const gom::Transform & transform, const assetLibrary::MMesh& mesh) const override;
				virtual void SetLocalDeferred(const assetLibrary::MMaterial& mat, const gom::Camera& camera, const gom::Transform& transform, const assetLibrary::MMesh& mesh) const override;
			};
		}
	}
}