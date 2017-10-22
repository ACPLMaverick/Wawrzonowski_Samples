#define WEIGHT_0 1.0f
#define WEIGHT_1 0.9f
#define WEIGHT_2 0.55f
#define WEIGHT_3 0.18f
#define WEIGHT_4 0.1f

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

struct VertexShaderOutputHorizontal
{
    float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;

	float2 hCoord01 : TEXCOORD1;
	float2 hCoord02 : TEXCOORD2;
	float2 hCoord03 : TEXCOORD3;
	float2 hCoord04 : TEXCOORD4;
	float2 hCoord05 : TEXCOORD5;
	float2 hCoord06 : TEXCOORD6;
	float2 hCoord07 : TEXCOORD7;
	float2 hCoord08 : TEXCOORD8;
	float2 hCoord09 : TEXCOORD9;
};

struct VertexShaderOutputVertical
{
	float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;

	float2 hCoord01 : TEXCOORD1;
	float2 hCoord02 : TEXCOORD2;
	float2 hCoord03 : TEXCOORD3;
	float2 hCoord04 : TEXCOORD4;
	float2 hCoord05 : TEXCOORD5;
	float2 hCoord06 : TEXCOORD6;
	float2 hCoord07 : TEXCOORD7;
	float2 hCoord08 : TEXCOORD8;
	float2 hCoord09 : TEXCOORD9;
};

VertexShaderOutputHorizontal VertexShaderFunctionHor(VertexShaderInput input)
{
    VertexShaderOutputHorizontal output;

    output.Position = mul(input.Position, WorldViewProj);
	output.TexCoord = input.TexCoord;

	float texelSize = 1.0f / ScreenWidth;

	output.hCoord01 = input.TexCoord + float2(texelSize * -4.0f, 0.0f);
	output.hCoord02 = input.TexCoord + float2(texelSize * -3.0f, 0.0f);
	output.hCoord03 = input.TexCoord + float2(texelSize * -2.0f, 0.0f);
	output.hCoord04 = input.TexCoord + float2(texelSize * -1.0f, 0.0f);
	output.hCoord05 = input.TexCoord + float2(texelSize * 0.0f, 0.0f);
	output.hCoord06 = input.TexCoord + float2(texelSize * 1.0f, 0.0f);
	output.hCoord07 = input.TexCoord + float2(texelSize * 2.0f, 0.0f);
	output.hCoord08 = input.TexCoord + float2(texelSize * 3.0f, 0.0f);
	output.hCoord09 = input.TexCoord + float2(texelSize * 4.0f, 0.0f);

    return output;
}

VertexShaderOutputVertical VertexShaderFunctionVer(VertexShaderInput input)
{
	VertexShaderOutputHorizontal output;

	output.Position = mul(input.Position, WorldViewProj);
	output.TexCoord = input.TexCoord;

	float texelSize = 1.0f / ScreenHeight;

	output.hCoord01 = input.TexCoord + float2(0.0f, texelSize * -4.0f);
	output.hCoord02 = input.TexCoord + float2(0.0f, texelSize * -3.0f);
	output.hCoord03 = input.TexCoord + float2(0.0f, texelSize * -2.0f);
	output.hCoord04 = input.TexCoord + float2(0.0f, texelSize * -1.0f);
	output.hCoord05 = input.TexCoord + float2(0.0f, texelSize * 0.0f);
	output.hCoord06 = input.TexCoord + float2(0.0f, texelSize * 1.0f);
	output.hCoord07 = input.TexCoord + float2(0.0f, texelSize * 2.0f);
	output.hCoord08 = input.TexCoord + float2(0.0f, texelSize * 3.0f);
	output.hCoord09 = input.TexCoord + float2(0.0f, texelSize * 4.0f);

	return output;
}

float4 PixelShaderFunction(VertexShaderOutputHorizontal input) : COLOR0
{
	float norm = (WEIGHT_0 + 2.0f * (WEIGHT_1 + WEIGHT_2 + WEIGHT_3 + WEIGHT_4));
	float weight0 = WEIGHT_0 / norm;
	float weight1 = WEIGHT_1 / norm;
	float weight2 = WEIGHT_2 / norm;
	float weight3 = WEIGHT_3 / norm;
	float weight4 = WEIGHT_4 / norm;

	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	color += tex2D(ScreenSampler, input.hCoord01) * weight4;
	color += tex2D(ScreenSampler, input.hCoord02) * weight3;
	color += tex2D(ScreenSampler, input.hCoord03) * weight2;
	color += tex2D(ScreenSampler, input.hCoord04) * weight1;
	color += tex2D(ScreenSampler, input.hCoord05) * weight0;
	color += tex2D(ScreenSampler, input.hCoord06) * weight1;
	color += tex2D(ScreenSampler, input.hCoord07) * weight2;
	color += tex2D(ScreenSampler, input.hCoord08) * weight3;
	color += tex2D(ScreenSampler, input.hCoord09) * weight4;

	color.a = 1.0f;

    return color;
}

technique BlurHorizontal
{
	pass Pass1
	{
		VertexShader = compile vs_3_0 VertexShaderFunctionHor();
		PixelShader = compile ps_3_0 PixelShaderFunction();
	}
}

technique BlurVertical
{
	pass Pass1
	{
		VertexShader = compile vs_3_0 VertexShaderFunctionVer();
		PixelShader = compile ps_3_0 PixelShaderFunction();
	}
}
