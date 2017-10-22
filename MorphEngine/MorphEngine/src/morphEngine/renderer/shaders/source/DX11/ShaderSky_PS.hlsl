#include "Deferred.hlsli"

cbuffer BufferPerObject : register(b0)
{
	float4 gColor;
	float gContrast;
}

struct PixelInput
{
	float4 Position : SV_POSITION;
	float3 PositionView : TEXCOORD1;
	float2 Uv : TEXCOORD0;
};

TextureCube texBase : register(t0);
SamplerState smpBase : register(s0);

float4 main(PixelInput input) : SV_TARGET
{
	return pow(texBase.Sample(smpBase, normalize(input.PositionView)) * gColor, gContrast);
	//return (float4(1.0f, 0.0f, 0.0f, 1.0f));
}