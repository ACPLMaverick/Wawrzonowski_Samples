// GLOBALS

cbuffer BufferVS
{
	float4x4 gMatWVP;
	float4x4 gMatWV;
	float4x4 gMatWInvTranspV;
};

// STRUCTURES

struct VertexInput
{
	float3 Position : POSITION;
	float2 Uv : TEXCOORD0;
	float3 Normal : NORMAL;
};

struct PixelInput
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Uv : TEXCOORD0;
};

// MAIN

PixelInput main(VertexInput input)
{
	PixelInput output;

	output.Position = mul(float4(input.Position, 1.0f), gMatWVP);
	output.Normal = mul(float4(input.Normal, 0.0f), gMatWInvTranspV).xyz;
	output.Uv = input.Uv;

	return output;
}