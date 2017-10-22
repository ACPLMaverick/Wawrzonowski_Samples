#pragma once

#include "../stdafx.h"
#include "../Buffer.h"
#include "../Color32.h"
#include "../Float3.h"
#include "../Float2.h"
#include "../UShort3.h"
#include "../Int2.h"
#include "../Matrix4x4.h"
#include "../light/LightAmbient.h"
#include "../light/LightDirectional.h"
#include "../light/LightSpot.h"
#include "../Texture.h"
#include "../Material.h"

//#define PERSP_CORRECT

namespace rendererMav
{
	class GraphicsDevice
	{
	protected:

#pragma region Protected Structs

		struct VertexInput
		{
			math::Float3 Position;
			math::Float3 Normal;
			math::Float2 Uv;
		};

		struct VertexOutput
		{
			math::Float3 Position;
			math::Float3 WorldPosition;
			math::Float3 Normal;
			math::Float2 Uv;
		};

		struct PixelInput
		{
			math::Float3 WorldPosition;
			math::Float3 Normal;
			math::Float2 Uv;
			math::Int2 Position;
		};

		struct PixelOutput
		{
			Color32 color;
		};

#pragma endregion

#pragma region Protected const

		static const int _LIGHT_AMBIENT_COUNT = 1;
		static const int _LIGHT_DIRECTIONAL_COUNT = 4;
		static const int _LIGHT_SPOT_COUNT = 2;
		const float _fltInv255 = 1.0f / 255.0f;

#pragma endregion

#pragma region Protected

		Buffer<Color32>* _bufferColor;
		Buffer<float>* _bufferDepth;

		const math::Float3* _vb;
		const math::Float3* _nb;
		const math::Float2* _ub;

		const math::Float3* _camPos;

		const math::Matrix4x4* _wvpMat;
		const math::Matrix4x4* _wMat;
		const math::Matrix4x4* _wInvTransMat;

		const Material* _material;

		light::LightAmbient _lightAmb;
		light::LightDirectional _lightsDir[_LIGHT_DIRECTIONAL_COUNT];
		light::LightSpot _lightsSpot[_LIGHT_SPOT_COUNT];
		uint8_t _lAmbCount = 0;
		uint8_t _lDirCount = 0;
		uint8_t _lSpotCount = 0;

#pragma endregion

#pragma region Functions Protected

		virtual inline int32_t ConvertFromScreenToBuffer(float point, uint16_t maxValue);

		virtual inline void VertexShader
		(
			const VertexInput& in,
			VertexOutput& out
		);

		virtual inline void Rasterizer
		(
			VertexOutput& in1,
			VertexOutput& in2,
			VertexOutput& in3
		);

		virtual inline void PixelShader
		(
			const PixelInput& in,
			Color32& out
		);

#pragma endregion

	public:

#pragma region Functions Public

		GraphicsDevice();
		~GraphicsDevice();

		void Initialize(Buffer<Color32>* cb, Buffer<float>* db);
		void Shutdown();

		void Draw(size_t triangleNum);
		void DrawIndexed(size_t triangleNum, const math::UShort3* ib);

		void SetVertexBuffer(const math::Float3* buf);
		void SetNormalBuffer(const math::Float3* buf);
		void SetUVBuffer(const math::Float2* buf);

		void SetWorldViewProjMatrix(const math::Matrix4x4* m);
		void SetWorldMatrix(const math::Matrix4x4* m);
		void SetWorldInverseTransposeMatrix(const math::Matrix4x4* m);

		void SetCameraPosition(const math::Float3* pos);

		void SetMaterial(const Material* mat);

		uint8_t EnableLightAmbient(const Color32* color);
		uint8_t EnableDirectionalLight(const Color32* col, const math::Float3* direction);
		uint8_t EnableSpotLight(
			const Color32* col,
			const math::Float3* dir,
			const math::Float3* pos,
			float attC,
			float attL,
			float attQ,
			float umbra,
			float penumbra,
			float falloff
		);

		light::LightAmbient* GetLightAmbient(uint8_t id);
		light::LightDirectional* GetLightDirectional(uint8_t id);
		light::LightSpot* GetLightSpot(uint8_t id);

		bool DisableLightAmbient(uint8_t id);
		bool DisableLightDirectional(uint8_t id);
		bool DisableLightSpot(uint8_t id);
		bool DisableNextLightAmbient();
		bool DisableNextLightDirectional();
		bool DisableNextLightSpot();

#pragma endregion
	};


}
