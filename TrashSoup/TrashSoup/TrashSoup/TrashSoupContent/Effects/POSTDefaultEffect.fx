#define WEIGHT_0 1.0f
#define WEIGHT_1 0.9f
#define WEIGHT_2 0.55f
#define WEIGHT_3 0.18f
#define WEIGHT_4 0.1f

float4x4 WorldViewProj;

float3 ColorAddition;
float3 ColorMultiplication;
float3 VignetteColor;
float2 VignetteRadius;
float Contrast;

texture ScreenTexture;
sampler ScreenSampler = sampler_state
{
	texture = <ScreenTexture>;
	MipFilter = Point;
	MinFilter = Point;
	MagFilter = Point;
	AddressU = clamp;
	AddressV = clamp;
};

struct VertexShaderInput
{
    float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;
};

struct VertexShaderOutput
{
    float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;
};

VertexShaderOutput VertexShaderFunction(VertexShaderInput input)
{
    VertexShaderOutput output;

    output.Position = mul(input.Position, WorldViewProj);
	output.TexCoord = input.TexCoord;

    return output;
}

float4 PixelShaderFunction(VertexShaderOutput input) : COLOR0
{
	float dist = distance(input.TexCoord, float2(0.5f, 0.5f)) * 0.7f;
	dist = smoothstep(VignetteRadius.x, VignetteRadius.y, dist);

	float3 vCol = 1.0f / max(dist, 0.0000000000001f) * VignetteColor;

	float4 color = tex2D(ScreenSampler, input.TexCoord);
	color.r = (color.r * ColorMultiplication.r + ColorAddition.r);
	color.g = (color.g * ColorMultiplication.g + ColorAddition.g);
	color.b = (color.b * ColorMultiplication.b + ColorAddition.b);

	color.rgb = ((color.rgb - 0.5f) * max(Contrast, 0.0f)) + 0.5f;

	color.rgb = lerp(VignetteColor, color.rgb, dist);

    return color;
}

technique Main
{
	pass Pass1
	{
		PixelShader = compile ps_2_0 PixelShaderFunction();
	}
}
