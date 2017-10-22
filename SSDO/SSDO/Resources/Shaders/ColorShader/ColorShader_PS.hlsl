// GLOBALS

cbuffer BufferPS
{
	float4 gColBase;
	float4 gColSpecular;
	float gGloss;
};

// STRUCTS

struct PixelInput
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Uv : TEXCOORD0;
};

struct PixelOutput
{
	float4 Color    : SV_Target0;
	float4 Normal	: SV_Target1;
};

// FUNCTIONS
float CalculateDepth(PixelInput input)
{
	return 1.0f - input.Position.z;
}

// MAIN

PixelOutput main(PixelInput input)
{
	PixelOutput output;
	output.Color = gColBase;
	output.Color.w = gGloss;
	output.Normal = float4(normalize(input.Normal), CalculateDepth(input));
	return output;
}