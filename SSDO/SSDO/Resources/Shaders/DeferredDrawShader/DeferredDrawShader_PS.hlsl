#include "../_global/GlobalDefines.hlsli"

Texture2D TexColor : register(t0);
SamplerState SmpColor : register(s0);

Texture2D TexNormal : register(t1);
SamplerState SmpNormal : register(s1);

Texture2D TexDepth : register(t2);
SamplerState SmpDepth : register(s2);

float4 main(DPixelInput input) : SV_TARGET
{
	float4 color = TexColor.Sample(SmpColor, input.Uv);
	float3 normal = normalize(TexNormal.Sample(SmpNormal, input.Uv).xyz);
	float depth = TexDepth.Sample(SmpDepth, input.Uv).x;

	return color;
}