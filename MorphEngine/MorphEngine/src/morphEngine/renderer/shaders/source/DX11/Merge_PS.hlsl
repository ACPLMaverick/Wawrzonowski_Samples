#include "Deferred.hlsli"
#include "Lighting.hlsli"

cbuffer Global : register(b1)
{
	float3 viewPos;
};

Texture2D TexInputA : register(t0);
SamplerState SmpInputA : register(s0);
Texture2D TexInputB : register(t1);
SamplerState SmpInputB : register(s1);

float4 main(PixelInputDeferred input) : SV_TARGET
{
	float4 sampleA = TexInputA.Sample(SmpInputA, input.Uv);
	float4 sampleB = TexInputB.Sample(SmpInputB, input.Uv);

	return lerp(sampleA, sampleB, sampleB.a);
}