// GLOBALS

// STRUCTS

struct PixelInput
{
	float4 Position : SV_POSITION;
	float2 Uv : TEXCOORD0;
	float4 Color : TEXCOORD1;
};

struct PixelOutput
{
	float4 Color : SV_Target0;
};

// TEXTUES

Texture2D TexFont : register(t0);
SamplerState SmpFont : register(s0);

// MAIN

PixelOutput main(PixelInput input)
{
	float fColor = TexFont.Sample(SmpFont, input.Uv).r;

	PixelOutput output;
	output.Color = input.Color;
	output.Color.a = fColor;
	return output;
}