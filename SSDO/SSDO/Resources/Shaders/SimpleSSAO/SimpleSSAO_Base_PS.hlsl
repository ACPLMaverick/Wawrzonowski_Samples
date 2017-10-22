#include "../_global/GlobalDefines.hlsli"

#define SAMPLE_COUNT 14

cbuffer LightCommon : register(b0)
{
	float4x4 projInverse;
};

cbuffer SimpleSSAO : register(b1)
{
	float4x4 gProj;
	float4 gOffsets[SAMPLE_COUNT];
	float4 gParams;
};

BASE_TEXTURES

Texture2D TexInput : register(t3);
SamplerState SmpInput : register(s3);

Texture2D TexBuffer : register(t4);
SamplerState SmpBuffer : register(s4);

Texture2D TexRandomVectors : register(t6);
SamplerState SmpRandomVectors : register(s6);

float Occlusion(float distZ)
{
	const float EPSILON = gParams.z;
	const float FADE_START = gParams.y;
	const float FADE_END = gParams.x;

	float occlusion = 0.0f;

	if (distZ > EPSILON)
	{
		float fadeLength = FADE_END - FADE_START;
		occlusion = saturate((FADE_END - distZ) / fadeLength);
	}

	return occlusion;
}

float4 main(DPixelInput input) : SV_TARGET
{
	float4 normalDepth = TexNormalDepth.SampleLevel(SmpNormalDepth, input.Uv, 0);
	float depth = normalDepth.w;
	float3 normal = normalDepth.xyz;
	float3 viewPos = ViewPositionFromDepth(projInverse, input.Uv, depth);
	float3 randomVec = TexRandomVectors.Sample(SmpRandomVectors, input.Uv).xyz;
	float maxDist = gParams.x;

	float occlusionCounter = 0.0f;
	float occlusionDivisor = SAMPLE_COUNT;

	[unroll]
	for (int i = 0; i < SAMPLE_COUNT; ++i)
	{
		// Acquire sample position
		// Reflecting an offset by a random normal will give random but uniformly distributed vectors.
		float3 offset = reflect(gOffsets[i].xyz, randomVec);
		float3 normalizedOffset = normalize(offset);
		float flip = sign(dot(normalizedOffset, normal));
		float4 samplePos = float4(viewPos + maxDist * offset * flip, 1.0f);
		float3 viewSamplePos = samplePos.xyz;

		// Go from view position to screen position (so the depth buffer could be directly used)
		samplePos = mul(samplePos, gProj);
		samplePos /= samplePos.w;

		// Get sample position from depth buffer
		float sampleDepth = samplePos.z;
		float2 mapUv = (samplePos.xy + 1.0f) * 0.5f;
		mapUv.y = 1.0f - mapUv.y;
		float mapDepth = TexNormalDepth.SampleLevel(SmpNormalDepth, mapUv, 0.0f).w;
		float3 mapViewPos = ViewPositionFromDepth(projInverse, mapUv, mapDepth);

		// Compute occlusion for this sample
		float distZ = viewSamplePos.z - mapViewPos.z;
		float dp = max(dot(normal, normalize(mapViewPos - viewSamplePos)), 0.0f);
		occlusionCounter += dp * Occlusion(distZ);

		//occlusionCounter += min(max(sampleDepth - mapDepth, 0.0f), maxDist) / maxDist;
		//occlusionCounter += max(sampleDepth - mapDepth, 0.0f) * 10.0f;

		//float3 sampleDir = normalize(gOffsets[i]);
	}

	float occlusion = 1.0f - (occlusionCounter / occlusionDivisor);
	occlusion = pow(occlusion, 8.0f);

	return float4(occlusion, occlusion, occlusion, 1.0f);
}