#include "../_global/GlobalDefines.hlsli"

#define KERNEL_SIZE 2
#define KERNEL_RADIUS 5.0f

cbuffer LightCommon : register(b0)
{
	float4x4 gProjInverse;
};

cbuffer BlurMerge : register(b1)
{
	float2 gTexelSize;
}

cbuffer BlurMergeConst
{
	static float weights[5][5] =
	{
		0.01f, 0.02f, 0.04f, 0.02f, 0.01f,
		0.02f, 0.04f, 0.08f, 0.04f, 0.02f,
		0.04f, 0.08f, 0.16f, 0.08f, 0.04f,
		0.02f, 0.04f, 0.08f, 0.04f, 0.02f,
		0.01f, 0.02f, 0.04f, 0.02f, 0.01f
	};
};

BASE_TEXTURES

Texture2D TexInput : register(t3);
SamplerState SmpInput : register(s3);

Texture2D TexBuffer : register(t4);
SamplerState SmpBuffer : register(s4);

float4 main(DPixelInput input) : SV_TARGET
{
	float4 normalDepth = TexNormalDepth.Sample(SmpNormalDepth, input.Uv);
	float depth = normalDepth.w;
	float3 normal = normalDepth.xyz;
	float4 baseAO = TexBuffer.Sample(SmpBuffer, input.Uv);
	float4 ao = weights[2][2] * baseAO;
	float weightSum = weights[2][2];

	[unroll]
	for (int i = -KERNEL_SIZE; i <= KERNEL_SIZE; ++i)
	{
		[unroll]
		for (int j = -KERNEL_SIZE; j <= KERNEL_SIZE; ++j)
		{
			if (i == 0 && j == 0)
				continue;

			float2 offset = float2(gTexelSize.x * j, gTexelSize.y * i) * KERNEL_RADIUS;
			float weight = weights[j + KERNEL_SIZE][i + KERNEL_SIZE];
			float4 aoSample = TexBuffer.Sample(SmpBuffer, input.Uv + offset);
			float4 normalDepthSample = TexNormalDepth.Sample(SmpNormalDepth, input.Uv);
			float depthSample = normalDepthSample.w;
			float3 normalSample = normalDepthSample.xyz;

			if (dot(normalSample, normal) < 0.8f || abs(depthSample - depth) > 0.2f)
				continue;

			ao += aoSample * weight;
			weightSum += weight;
		}
	}

	ao = ao / weightSum;

	float4 inputSample = TexInput.Sample(SmpInput, input.Uv);
	return saturate(ao) * inputSample;
}