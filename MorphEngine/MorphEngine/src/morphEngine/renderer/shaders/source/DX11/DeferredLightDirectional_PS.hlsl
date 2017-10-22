#include "Deferred.hlsli"
#include "Lighting.hlsli"

cbuffer LightDirectional : register(b0)
{
	float4 gColor;
	float3 gDirection;
};

cbuffer LightCommon : register(b1)
{
	float3 gViewPosition;
};

TEXTURE_INPUT_DEFERRED

float4 main(PixelInputDeferred input) : SV_TARGET
{
	float4 colorSample = TexColor.Sample(SmpColor, input.Uv);
	float4 normalSample = TexNormal.Sample(SmpNormal, input.Uv);
	float4 positionSample = TexWorldPos.Sample(SmpWorldPos, input.Uv);

	PixelInput pInput;
	pInput.Position = input.Position;
	pInput.PositionWorld = positionSample.xyz;
	pInput.Normal = normalize(normalSample.xyz);
	pInput.Uv = input.Uv;

	MaterialData pData;
	pData.colBase = float4(colorSample.xyz, 1.0f);
	pData.colSpecular = float4(Float3Unpack(positionSample.a) * colorSample.a, 1.0f);
	pData.gloss = normalSample.a;

	float4 inColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 viewDir = normalize(gViewPosition - pInput.PositionWorld);

	LightDirectional(pInput, gColor, gDirection, viewDir, pData, inColor);

	return inColor;
}