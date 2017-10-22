// GLOBALS

#define BONES_PER_MESH 96

cbuffer BufferPerObject : register(b0)
{
	float4x4 gMatWVP;
	float4x4 gMatW;
	float4x4 gMatWInvTransp;
};

cbuffer BufferSkin : register(b1)
{
	float4x4 gSkinData[BONES_PER_MESH];
};

// STRUCTURES

struct VertexInput
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Uv : TEXCOORD0;
	float4 Weight : TEXCOORD1;
	int4 Index : TEXCOORD2;
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

	float3 pos = float3(0.0f, 0.0f, 0.0f);
	float3 normal = float3(0.0f, 0.0f, 0.0f);
	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		pos += input.Weight[i] * mul(float4(input.Position, 1.0f), gSkinData[input.Index[i]]).xyz;
		normal += input.Weight[i] * mul(gSkinData[input.Index[i]], float4(input.Normal, 1.0f)).xyz;
	}

	output.Position = mul(float4(pos, 1.0f), gMatWVP);
	output.PositionWorld = mul(float4(pos, 1.0f), gMatW).xyz;
	output.Normal = mul(float4(normal, 0.0f), gMatWInvTransp).xyz;
	output.Uv = input.Uv;

	return output;
}