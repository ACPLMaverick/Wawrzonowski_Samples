// GLOBALS

cbuffer BufferPerObject
{
	float4x4 gMatGUI;
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
};

// MAIN

PixelInput main(VertexInput input)
{
	PixelInput output;

	output.Position = mul(float4(input.Position, 1.0f), gMatGUI);
	output.Uv = input.Uv;

	return output;
}