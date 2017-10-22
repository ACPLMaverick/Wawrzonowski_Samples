// GLOBALS

cbuffer BufferVS
{
	float4 gColor;
	float4 gPositionScale;
};

// STRUCTURES

struct VertexInput
{
	float3 Position : POSITION;
	float2 Uv : TEXCOORD0;
};

struct PixelInput
{
	float4 Position : SV_POSITION;
	float2 Uv : TEXCOORD0;
	float4 Color : TEXCOORD1;
};

// MAIN

PixelInput main(VertexInput input)
{
	PixelInput output;

	output.Position = float4(float3(input.Position.xy * gPositionScale.zw + gPositionScale.xy, 0.0f), 1.0f);
	output.Uv = input.Uv;
	output.Color = gColor;

	return output;
}