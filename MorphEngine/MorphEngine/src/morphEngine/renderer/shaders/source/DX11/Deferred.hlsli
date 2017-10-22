// Global defines

#define TEXTURE_INPUT_DEFERRED \
	Texture2D TexColor : register(t0); \
	SamplerState SmpColor : register(s0); \
	Texture2D TexDepth : register(t1); \
	SamplerState SmpDepth : register(s1); \
	Texture2D TexNormal : register(t2); \
	SamplerState SmpNormal : register(s2); \
	Texture2D TexWorldPos : register(t3); \
	SamplerState SmpWorldPos : register(s3);

#define TEXTURE_INPUT_POSTPROCESS \
	Texture2D TexInput : register(t4); \
	SamplerState SmpInput : register(s4);

// Data structures

struct VertexInputDeferred
{
	float3 Position : POSITION;
	float2 Uv : TEXCOORD0;
};

struct PixelInputDeferred
{
	float4 Position : SV_POSITION;
	float2 Uv : TEXCOORD0;
};

struct PixelOutputDeferred
{
	float4 Color    : SV_Target0;
	float4 Normal	: SV_Target1;
	float4 WorldPos : SV_Target2;
};

// Functions

PixelInputDeferred GeneratePixelInputDeferred(uint vertexID)
{
	PixelInputDeferred input;

	/*
	//See: https://web.archive.org/web/20140719063725/http://www.altdev.co/2011/08/08/interesting-vertex-shader-trick/

	1
	( 0, 2)
	[-1, 3]   [ 3, 3]
	.
	|`.
	|  `.
	|    `.
	'------`
	0         2
	( 0, 0)   ( 2, 0)
	[-1,-1]   [ 3,-1]

	ID=0 -> Pos=[-1,-1], Tex=(0,0)
	ID=1 -> Pos=[-1, 3], Tex=(0,2)
	ID=2 -> Pos=[ 3,-1], Tex=(2,0)
	*/

	input.Uv.x = (vertexID == 2) ? 2.0f : 0.0f;
	input.Uv.y = (vertexID == 1) ? 2.0f : 0.0f;

	input.Position = float4(input.Uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);

	return input;
}

/*
	http://diaryofagraphicsprogrammer.blogspot.com/2009/10/bitmasks-packing-data-into-fp-render.html
*/
float Float3Pack(float3 channel)
{
	// layout of a 32-bit fp register
	// SEEEEEEEEMMMMMMMMMMMMMMMMMMMMMMM
	// 1 sign bit; 8 bits for the exponent and 23 bits for the mantissa
	uint uValue;

	// pack x
	uValue = ((uint)(channel.x * 65535.0 + 0.5)); // goes from bit 0 to 15

												  // pack y in EMMMMMMM
	uValue |= ((uint)(channel.y * 255.0 + 0.5)) << 16;

		// pack z in SEEEEEEE
		// the last E will never be 1b because the upper value is 254
		// max value is 11111110 == 254
		// this prevents the bits of the exponents to become all 1
		// range is 1.. 254
		// to prevent an exponent that is 0 we add 1.0
	uValue |= ((uint)(channel.z * 253.0 + 1.5)) << 24;

	return asfloat(uValue);
}

// unpack three positive normalized values from a 32-bit float
float3 Float3Unpack(float fFloatFromFP32)
{
	float a, b, c, d;
	uint uValue;

	uint uInputFloat = asuint(fFloatFromFP32);

	// unpack a
	// mask out all the stuff above 16-bit with 0xFFFF
	a = ((uInputFloat) & 0xFFFF) / 65535.0;

	b = ((uInputFloat >> 16) & 0xFF) / 255.0;

	// extract the 1..254 value range and subtract 1
	// ending up with 0..253
	c = (((uInputFloat >> 24) & 0xFF) - 1.0) / 253.0;

	return float3(a, b, c);
}

float3 ViewPositionFromDepth(float4x4 projInverse, float2 pixelCoord, float depth)
{
	float4 projectedPos = float4(
		pixelCoord.x * 2.0f - 1.0f,
		(1.0f - pixelCoord.y) * 2.0f - 1.0f,
		depth/* * 2.0f - 1.0f*/,
		1.0f);
	float4 viewSpacePos = mul(projectedPos, projInverse);
	viewSpacePos = viewSpacePos / viewSpacePos.w;
	return viewSpacePos.xyz;
}