#include "../_global/GlobalDefines.hlsli"

// Group size lowered to make able for shader to compute two maps at once
#define GROUP_SIZE_X 512
#define GROUP_SIZE_Y 1

#define KERNEL_SIZE 5

// INPUT

cbuffer InputGlobal : register(b1)
{
	const uint gWidth;
	const uint gLevel;
	const float gFilterHalfSize;
	const float gFilterSampleSpacing;
	const bool gVertical;
};

cbuffer BlurMergeConst : register(b2)
{
	static float gWeights[11] =
	{
		0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f
	};
};


Texture2D<float4> InA : register(t0);
Texture2D<float4> InB : register(t1);

RWTexture2D<float4> OutA : register(u0);
RWTexture2D<float4> OutB : register(u1);

// FUNCTIONS

[numthreads(GROUP_SIZE_X, GROUP_SIZE_Y, 1)]
void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
	uint2 index = DTid.xy;
	uint2 texOffset = uint2(1, 0);

	if (index.x >= gWidth)
	{
		return;
	}

	if (gVertical)
	{
		index = index.yx;
		texOffset = texOffset.yx;
	}

	// Get input values
	float4 inputA = InA.mips[gLevel][index];
	float4 inputB = InB.mips[gLevel][index];

	// Get current depth value and assign weights
	float3 normal = normalize(inputA.xyz);
	float depth = inputA.w;
	inputA *= gWeights[5];
	inputB *= gWeights[5];
	float totalWeight = gWeights[5];

	const float depthFactor = pow(max(depth, 0.0f), 1.2f);
	const float depthBorder = gFilterHalfSize * depthFactor;

	// Get samples with weight based on depth difference

	for (int i = -KERNEL_SIZE; i <= KERNEL_SIZE; ++i)
	{
		if (i == 0)
			continue;

		uint2 neighIndex = index + (gFilterSampleSpacing + 1) * i * texOffset * 3.0f;
		if (gVertical)
		{
			neighIndex.y = clamp(neighIndex.y, 0, gWidth - 1);
		}
		else
		{
			neighIndex.x = clamp(neighIndex.x, 0, gWidth - 1);
		}

		float4 neighInputA = InA.mips[gLevel][neighIndex];
		float4 neighInputB = InB.mips[gLevel][neighIndex];
		float neighDepth = neighInputA.w;

		if (abs(neighDepth - depth) > depthBorder)
			continue;

		float weight = gWeights[i + KERNEL_SIZE];
		inputA += neighInputA * weight;
		inputB += neighInputB * weight;
		totalWeight += weight;
	}

	// Compute and save averages

	inputA /= totalWeight;
	inputB /= totalWeight;

	OutA[index] = inputA;
	OutB[index] = inputB;
}