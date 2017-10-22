#define WEIGHT_0 1.0f
#define WEIGHT_1 0.9f
#define WEIGHT_2 0.55f
#define WEIGHT_3 0.18f
#define WEIGHT_4 0.1f

#define OFFSET_MUL 0.001f
#define FILTER_SIZE 4

float4x4 WorldViewProj;

float ScreenWidth;
float ScreenHeight;

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
	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	/*float ctr = 0.0f;
	[unroll]
	for (int i = -FILTER_SIZE; i <= FILTER_SIZE; ++i)
	{
		color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + (float)i * OFFSET_MUL, input.TexCoord.y));
		ctr = ctr + 1.0f;
		
	}

	[unroll]
	for (int j = -FILTER_SIZE; j <= FILTER_SIZE; ++j)
	{
		color = color + tex2D(ScreenSampler, float2(input.TexCoord.x, input.TexCoord.y + (float)j * OFFSET_MUL));
		ctr = ctr + 1.0f;
	}

	color = color / ctr;*/

	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + -2.0f * OFFSET_MUL, input.TexCoord.y + -2.0f * OFFSET_MUL));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + -1.0f * OFFSET_MUL, input.TexCoord.y + -2.0f * OFFSET_MUL));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x, input.TexCoord.y + -2.0f * OFFSET_MUL));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + 1.0f * OFFSET_MUL, input.TexCoord.y + -2.0f * OFFSET_MUL));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + 2.0f * OFFSET_MUL, input.TexCoord.y + -2.0f * OFFSET_MUL));

	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + -2.0f * OFFSET_MUL, input.TexCoord.y + -1.0f * OFFSET_MUL));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + -2.0f * OFFSET_MUL, input.TexCoord.y));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + -2.0f * OFFSET_MUL, input.TexCoord.y + 1.0f * OFFSET_MUL));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + -2.0f * OFFSET_MUL, input.TexCoord.y + 2.0f * OFFSET_MUL));

	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + -1.0f * OFFSET_MUL, input.TexCoord.y + -1.0f * OFFSET_MUL));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x, input.TexCoord.y));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + 1.0f * OFFSET_MUL, input.TexCoord.y + 1.0f * OFFSET_MUL));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + 2.0f * OFFSET_MUL, input.TexCoord.y + 2.0f * OFFSET_MUL));

	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + 2.0f * OFFSET_MUL, input.TexCoord.y + -1.0f * OFFSET_MUL));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + 2.0f * OFFSET_MUL, input.TexCoord.y));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + 2.0f * OFFSET_MUL, input.TexCoord.y + 1.0f * OFFSET_MUL));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + 1.0f * OFFSET_MUL, input.TexCoord.y + -1.0f * OFFSET_MUL));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + 1.0f * OFFSET_MUL, input.TexCoord.y));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + 1.0f * OFFSET_MUL, input.TexCoord.y + 1.0f * OFFSET_MUL));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x, input.TexCoord.y + -1.0f * OFFSET_MUL));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x, input.TexCoord.y + 1.0f * OFFSET_MUL));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x, input.TexCoord.y + 2.0f * OFFSET_MUL));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + -1.0f * OFFSET_MUL, input.TexCoord.y));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + -1.0f * OFFSET_MUL, input.TexCoord.y + 1.0f * OFFSET_MUL));
	color = color + tex2D(ScreenSampler, float2(input.TexCoord.x + -1.0f * OFFSET_MUL, input.TexCoord.y + 2.0f * OFFSET_MUL));

	color = color / 25.0f;

	color.a = 1.0f;

    return color;
}

technique Main
{
	pass Pass1
	{
		VertexShader = compile vs_3_0 VertexShaderFunction();
		PixelShader = compile ps_3_0 PixelShaderFunction();
	}
}
