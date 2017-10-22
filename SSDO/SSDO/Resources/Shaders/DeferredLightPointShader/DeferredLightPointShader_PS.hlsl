#include "../_global/GlobalDefines.hlsli"
#include "../_global/Lighting.hlsli"

cbuffer LightCommon : register(b0)
{
	float4x4 projInverse;
};

cbuffer LightPoint : register(b1)
{
	float4 gColor;
	float4 gPosition;
	float4 gRange;
};

BASE_TEXTURES

float4 main(DPixelInput input) : SV_TARGET
{
	float4 normalDepth = TexNormalDepth.Sample(SmpNormalDepth, input.Uv);
	float depth = normalDepth.w;
	float3 normal = normalDepth.xyz;

	PixelInput pInput;
	pInput.Position = input.Position;
	pInput.PositionView = ViewPositionFromDepth(projInverse, input.Uv, normalDepth.w);
	pInput.Normal = normalize(normalDepth.xyz);
	pInput.Uv = input.Uv;

	MaterialData pData;
	pData.colBase = TexColor.Sample(SmpColor, input.Uv);
	pData.colSpecular = float4(1.0f, 1.0f, 1.0f, 1.0f);
	pData.gloss = 50.0f;

	float4 inColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 viewDir = normalize(-pInput.PositionView);

	LightPoint(pInput, gColor, gPosition.xyz, gRange.x, viewDir, pData, inColor);

	return inColor;
	//return float4(viewInverse[2][3], 0.0f, 0.0f, 1.0f);
}