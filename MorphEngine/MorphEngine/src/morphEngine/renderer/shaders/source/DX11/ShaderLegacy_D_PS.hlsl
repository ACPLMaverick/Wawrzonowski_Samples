// INCLUDE

#include "Deferred.hlsli"
#include "Lighting.hlsli"

// GLOBALS

cbuffer BufferPS
{
	float4 gColBase;
	float4 gColSpecular;
	float gGloss;
	float3 padding;
};


// TEXTURES

Texture2D texBase : register(t0);
SamplerState smpBase : register(s0);

Texture2D texNormal : register(t1);
SamplerState smpNormal : register(s1);

// STRUCTS

struct PixelInputMain : PixelInput
{

};


// MAIN

PixelOutputDeferred main(PixelInputMain input)
{
	PixelOutputDeferred output;

	float4 baseSample = texBase.Sample(smpBase, input.Uv);

	output.Color = float4(baseSample.xyz * gColBase.xyz, baseSample.w);
	output.Normal = float4(normalize(input.Normal), gGloss);
	output.WorldPos = float4(input.PositionWorld, Float3Pack(gColSpecular.xyz));

	return output;
}