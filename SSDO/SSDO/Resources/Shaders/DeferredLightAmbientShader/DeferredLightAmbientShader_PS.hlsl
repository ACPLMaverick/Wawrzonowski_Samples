#include "../_global/GlobalDefines.hlsli"

cbuffer LightAmbient
{
	float4 gColor;
};

BASE_TEXTURES

float4 main(DPixelInput input) : SV_TARGET
{
	float4 color = TexColor.Sample(SmpColor, input.Uv);
	return color * gColor;
}