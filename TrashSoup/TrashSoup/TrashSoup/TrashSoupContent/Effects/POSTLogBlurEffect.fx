#define WEIGHT_0 1.0f
#define WEIGHT_1 0.9f
#define WEIGHT_2 0.55f
#define WEIGHT_3 0.18f
#define WEIGHT_4 0.1f

static const int g_cKernelSize = 5;

float2 PixelKernel[g_cKernelSize] = 
{
	{ -5.5f, 0.0f },
	{ -2.5f, 0.0f },
	{ 0.0f, 0.0f },
	{ 2.5f, 0.0f },
	{5.5f, 0.0f}
};

static const float BlurWeights[g_cKernelSize] =
{
	0.0625f,
	0.25f,
	0.375f,
	0.25f,
	0.0625
};

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
	float4 ProjPos : TEXCOORD0;
	float2 TexCoord : TEXCOORD1;
	float2 pixelSize : TEXCOORD2;
};

VertexShaderOutput VertexShaderFunction(VertexShaderInput input)
{
    VertexShaderOutput output;

    output.Position = mul(input.Position, WorldViewProj);
	output.ProjPos = output.Position;
	output.TexCoord = input.TexCoord;

	output.pixelSize = float2(1.0f / ScreenWidth, 1.0f / ScreenHeight);

    return output;
}

inline float log_conv(float x0, float X, float y0, float Y)
{
	return (X + log(x0 + (y0 * exp(Y - X))));
}

float4 PixelShaderFunction(VertexShaderOutput input, float2 screenPos : VPOS) : COLOR0
{
	float2 texelOffset = input.pixelSize * 0.5f;

	float2 coord = screenPos * input.pixelSize;
	coord += texelOffset;

	float2 sampleCoord = 0.0f;

	float sm[g_cKernelSize];

	[unroll]
	for (int i = 0; i < g_cKernelSize; ++i)
	{
		sampleCoord = coord + PixelKernel[i] * input.pixelSize;
		sm[i] = tex2D(ScreenSampler, sampleCoord).r;
	}

	float accum = log_conv(BlurWeights[0], sm[0], BlurWeights[1], sm[1]);
	[unroll]
	for (int j = 2; j < g_cKernelSize; ++j)
	{
		accum = log_conv(1.0f, accum, BlurWeights[j], sm[j]);
	}

	float4 color = float4(accum, 0.0f, 0.0f, 1.0f);

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