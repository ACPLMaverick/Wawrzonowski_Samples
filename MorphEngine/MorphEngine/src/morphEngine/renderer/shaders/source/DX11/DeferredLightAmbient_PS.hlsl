#include "Deferred.hlsli"

cbuffer LightAmbient : register(b0)
{
	float4 gColor;
};

cbuffer Global : register(b1)
{
	float3 viewPos;
};

TEXTURE_INPUT_DEFERRED

float4 main(PixelInputDeferred input) : SV_TARGET
{
	float4 color = TexColor.Sample(SmpColor, input.Uv);
	return color * gColor;
}