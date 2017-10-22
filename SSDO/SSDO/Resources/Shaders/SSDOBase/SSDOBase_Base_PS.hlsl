#include "../_global/GlobalDefines.hlsli"
#include "../_global/Lighting.hlsli"

#define SAMPLE_COUNT 14

cbuffer LightCommon : register(b0)
{
	float4x4 projInverse;
};

cbuffer SSDOBase : register(b1)
{
	float4x4 gProj;
	float4 gOffsets[SAMPLE_COUNT];
	float4 gParams;
	float4 gLightColor;
	float3 gLightDirection;
};

struct PixelOutput
{
	float4 direct : SV_TARGET0;
	float4 indirectAdd : SV_TARGET1;
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

PixelOutput main(DPixelInput input)
{
	float4 normalSample = TexNormalDepth.SampleLevel(SmpNormalDepth, input.Uv, 0.0f);
	float depth = normalSample.w;
	float3 normal = normalSample.xyz;
	float3 viewPos = ViewPositionFromDepth(projInverse, input.Uv, depth);
	float3 randomVec = TexRandomVectors.Sample(SmpRandomVectors, input.Uv).xyz;
	const float maxDist = gParams.x;
	const float powFactor = gParams.w;

	float occlusionCounter = 0.0f;
	float occlusionDivisor = SAMPLE_COUNT;

	MaterialData pData;
	pData.colBase = float4(1.0f, 1.0f, 1.0f, 1.0f);
	pData.colSpecular = float4(1.0f, 1.0f, 1.0f, 1.0f);
	pData.gloss = 50.0f;

	PixelOutput output;

	output.direct = float4(0.0f, 0.0f, 0.0f, 1.0f);
	output.indirectAdd = float4(0.0f, 0.0f, 0.0f, 1.0f);

	[unroll]
	for (int i = 0; i < SAMPLE_COUNT; ++i)
	{
		// Acquire sample position
		// Reflecting an offset by a random normal will give random but uniformly distributed vectors.
		float3 offset = reflect(gOffsets[i].xyz, randomVec);
		offset = normalize(offset);
		float flip = sign(dot(offset, normal));
		offset = offset * flip;
		float4 samplePos = float4(viewPos + maxDist * offset, 1.0f);
		float3 viewSamplePos = samplePos.xyz;

		// Go from view position to screen position (so the depth buffer could be directly used)
		samplePos = mul(samplePos, gProj);
		samplePos /= samplePos.w;

		// Get sample position from depth buffer
		float sampleDepth = samplePos.z;
		float2 mapUv = (samplePos.xy + 1.0f) * 0.5f;
		mapUv.y = 1.0f - mapUv.y;
		float4 mapNormalSample = TexNormalDepth.SampleLevel(SmpNormalDepth, mapUv, 0.0f);
		float mapDepth = mapNormalSample.w;
		float3 mapViewPos = ViewPositionFromDepth(projInverse, mapUv, mapDepth);

		// Compute occlusion for this sample
		float distZ = mapViewPos.z - viewSamplePos.z;
		float3 sampleDirection = normalize(viewSamplePos - mapViewPos);
		// Directional scale factor for occlusion (based on angle between normal and sample point direction)
		float dp = max(dot(normal, offset), 0.0f);
		float finalOcclusion = dp * Occlusion(-distZ);
		occlusionCounter += finalOcclusion;

		// Lit from this sample
		float4 smpColor = 0.0f;
		float4 smpBaseColor = TexInput.SampleLevel(SmpInput, mapUv, 0.0f);
		//pData.colBase = smpBaseColor;

		PhongBlinn(gLightColor,
			1.0f,
			normalize(viewSamplePos - viewPos),
			gLightDirection,
			-normalize(viewSamplePos),
			pData,
			smpColor);
		
		// take this sample into consideration or not, depending whether it is above surface or not.
		float4 lit = smpColor;
		float litFactor = sign(max(distZ, 0.0f));
		litFactor *= (dp);

		// calculate indirecity
		float3 mapNormal = mapNormalSample.xyz;
		float indFactor = -sign(min(distZ, 0.0f));

		float3 transmittanceDirection = -(viewPos - mapViewPos);
		float tdLength = length(transmittanceDirection);
		transmittanceDirection /= tdLength;
		float dotSender = max(dot(-mapNormal, transmittanceDirection), 0.0f);
		float dotReceiver = max(dot(normal, transmittanceDirection), 0.0f);
		float dPowTwoRec = max(tdLength, 0.001f);
		dPowTwoRec = 1.0f / (dPowTwoRec * dPowTwoRec);
		float a = 5.0f;

		indFactor *= a * dotSender * dotReceiver * dPowTwoRec;
		// Multiplying by light factor here, avoiding doing second pass.
		indFactor *= max(dot(mapNormal, gLightDirection), 0.0f);

		// if maximum distance is exceeded, add to lit factor to avoid "shadow / light bleeding"
		if (abs(distZ) > (5.0f * maxDist))
		{
			litFactor += 0.5f;
			indFactor = 0.0f;
		}

		// add to global vars

		lit *= litFactor;
		output.direct += lit;

		output.indirectAdd += indFactor * smpBaseColor * gLightColor;
	}

	float occlusion = 1.0f - saturate(10.0f * (occlusionCounter / occlusionDivisor));
	occlusion = pow(occlusion, powFactor);
	//final.y = final.z = final.x = occlusion;
	output.direct += occlusion;

	output.direct = pow(max(output.direct, 0.0f), powFactor);
	output.direct = saturate(output.direct);

	output.indirectAdd *= occlusion * 0.3f;
	output.indirectAdd = (output.indirectAdd / SAMPLE_COUNT);

	return output;
}