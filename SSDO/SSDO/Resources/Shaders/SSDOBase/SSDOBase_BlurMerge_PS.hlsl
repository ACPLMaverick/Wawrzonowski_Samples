#include "../_global/GlobalDefines.hlsli"

#define KERNEL_SIZE 5
#define KERNEL_RADIUS 5.0f

cbuffer LightCommon : register(b0)
{
	float4x4 gProjInverse;
};

cbuffer BlurMerge : register(b1)
{
	float2 gTexelSize;
	bool gHorizontalBlur;
}

cbuffer BlurMergeConst
{
	static float gWeights[11] =
	{
		0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f
	};
};

struct PixelOutput
{
	float4 ao : SV_TARGET0;
	float4 indirect : SV_TARGET1;
};

BASE_TEXTURES;

Texture2D TexInput : register(t3);
SamplerState SmpInput : register(s3);

Texture2D TexBuffer : register(t4);
SamplerState SmpBuffer : register(s4);

Texture2D TexBufferB : register(t5);
SamplerState SmpBufferB : register(s5);

Texture2D TexRandomVectors : register(t6);
SamplerState SmpRandomVectors : register(s6);

PixelOutput main(DPixelInput input)
{
	PixelOutput output;

	float4 normalDepthSample = TexNormalDepth.Sample(SmpNormalDepth, input.Uv);
	float3 normal = normalDepthSample.xyz;
	float depth = normalDepthSample.w;
	float4 baseAO = TexBuffer.Sample(SmpBuffer, input.Uv);
	output.ao = gWeights[5] * baseAO;
	float4 baseIndirect = TexBufferB.Sample(SmpBufferB, input.Uv);
	output.indirect = gWeights[5] * baseIndirect;
	float totalWeight = gWeights[5];

	float2 texOffset;
	if (gHorizontalBlur)
	{
		texOffset = float2(gTexelSize.x, 0.0f);
	}
	else
	{
		texOffset = float2(0.0f, gTexelSize.y);
	}

	for (float i = -KERNEL_RADIUS; i <= KERNEL_RADIUS; ++i)
	{
		if (i == 0)
			continue;

		float2 neighUv = input.Uv + i * texOffset;
		float4 neighNormalDepthSample = TexNormalDepth.Sample(SmpNormalDepth, input.Uv);
		float3 neighNormal = neighNormalDepthSample.xyz;
		float neighDepth = neighNormalDepthSample.w;

		
		if (dot(neighNormal, normal) >= 0.8f && abs(neighDepth - depth) <= 0.2f)
		{
			float4 neighAo = TexBuffer.Sample(SmpBuffer, neighUv);
			float4 neighIndirect = TexBufferB.Sample(SmpBufferB, neighUv);
			float weight = gWeights[i + KERNEL_RADIUS];

			output.ao += weight * neighAo;
			output.indirect += weight * neighIndirect;

			totalWeight += weight;
		}
	}

	output.ao /= totalWeight;
	output.indirect /= totalWeight;

	// perform merge with first output when finishing blur
	if (gHorizontalBlur)
	{
		float4 inputSample = TexInput.Sample(SmpInput, input.Uv);
		output.ao = saturate(output.ao) * inputSample + output.indirect;
	}

	return output;
	/*
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
			float4 indirectSample = TexBufferB.Sample(SmpBufferB, input.Uv + offset);
			float3 normalSample = TexNormal.Sample(SmpNormal, input.Uv + offset).xyz;
			float depthSample = TexDepth.Sample(SmpDepth, input.Uv + offset).r;

			if (dot(normalSample, normal) < 0.8f || abs(depthSample - depth) > 0.2f)
				continue;

			ao += aoSample * weight;
			indirect += indirectSample * weight;
			weightSum += weight;
		}
	}

	ao = ao / weightSum;
	indirect = indirect / weightSum;

	float4 inputSample = TexInput.Sample(SmpInput, input.Uv);
	return saturate(ao) * inputSample + indirect;
	//return indirect;
	//return baseIndirect;
	//return saturate(ao);
	//return baseAO;
	*/
}