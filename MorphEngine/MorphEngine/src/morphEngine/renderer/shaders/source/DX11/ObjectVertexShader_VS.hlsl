// GLOBALS

cbuffer BufferPerObject
{
	float4x4 gMatWVP;
	float4x4 gMatW;
	float4x4 gMatWInvTransp;
};

// STRUCTURES

struct VertexInput
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Uv : TEXCOORD0;
};

struct PixelInput
{
	float4 Position : SV_POSITION;
	float3 PositionWorld : TEXCOORD1;
	float3 Normal : NORMAL;
	float2 Uv : TEXCOORD0;
};

// MAIN

PixelInput main(VertexInput input)
{
	PixelInput output;

	output.Position = mul(float4(input.Position, 1.0f), gMatWVP);
	output.PositionWorld = mul(float4(input.Position, 1.0f), gMatW).xyz;
	output.Normal = mul(float4(input.Normal, 0.0f), gMatWInvTransp).xyz;
	output.Uv = input.Uv;

	return output;
}