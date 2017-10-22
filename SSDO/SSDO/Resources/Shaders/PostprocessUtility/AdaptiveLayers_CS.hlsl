#include "../_global/GlobalDefines.hlsli"
#include "SATUtility.hlsli"

// Group size lowered to make able for shader to compute two elements of two sets at once
#define GROUP_SIZE_X 256
#define GROUP_SIZE_Y 1

// INPUT

cbuffer InputGlobal : register(b1)
{
	const float4 gSatDimensions;
	const float gBoxHalfSize;
	const uint gInputLevel;
	const uint gIndexOffset;
	const bool gInputSATIsDifferential;
};

Texture2D<float4> InA : register(t4);
Texture2D<float4> InB : register(t5); 
Texture2D<float4> InASat : register(t6);	// In these two slots there can be simply input SAT or sibling's sat if differential calculation is needed.
Texture2D<float4> InBSat : register(t7);	// It depends on the gInputSATIsDifferential flag.
Texture2D<float4> InAParentSat : register(t8);	// If gInputSATIsDifferential is false, these two will not be assigned.
Texture2D<float4> InBParentSat : register(t9);
SamplerState SatSampler : register(s0);

RWTexture2D<float4> OutALayerA : register(u0);
RWTexture2D<float4> OutBLayerA : register(u1);
RWTexture2D<float4> OutALayerB : register(u2);
RWTexture2D<float4> OutBLayerB : register(u3);
RWTexture2D<float4> OutIndices : register(u4);

// FUNCTIONS

void ComputeAdaptiveLayerForCoord(in const uint2 coord)
{
	const float2 coordFloat = float2(coord);

	// Sample current pixel's depth
	const float4 inSampleA = InA.mips[gInputLevel][coord];
	const float4 inSampleB = InB.mips[gInputLevel][coord];
	const float depth = inSampleA.a;

	// Compute current pixel's average depth.

	float2 samplePoints[4];
	float areaRec;

	GenerateSamplePointsAndAreaTexSpace(gSatDimensions, gBoxHalfSize,
		coordFloat, depth, samplePoints, areaRec);

	float4 avgSampleA;
	if (gInputSATIsDifferential)
	{
		CalculateAverageDifferential(gSatDimensions, InASat, InAParentSat, SatSampler, SatSampler, samplePoints, areaRec, avgSampleA);
	}
	else
	{
		CalculateAverage(gSatDimensions, InASat, SatSampler, samplePoints, areaRec, avgSampleA);
	}

	const float avgDepth = avgSampleA.a;

	// Assign current pixel to either one of two layers based on depth difference, and update indices accordingly.

	const float layerAffilation = step(0.00001f, depth);	// if no depth is stored in this pixel, no computation is necessary

	const float depthDiff = depth - avgDepth;	// if > 0, assign to A, if not assign to B
	float indexA = step(0.0f, depthDiff);
	float indexB = 1.0f - indexA;
	indexA *= layerAffilation;
	indexB *= layerAffilation;

	float4 oldOutIndicesVal = OutIndices[coord];	// So changes to other channels are not overwritten.
	float4 outIndicesVal = oldOutIndicesVal;
	if (gIndexOffset == 0)
	{
		outIndicesVal.x = indexA;
		outIndicesVal.y = indexB;
	}
	else if (gIndexOffset == 2)
	{
		outIndicesVal.z = indexA;
		outIndicesVal.w = indexB;
	}

	//outIndicesVal.a = depthDiff;

	const float4 fillValue = 0.0f;

	OutIndices[coord] = outIndicesVal;
	OutALayerA[coord] = lerp(fillValue, inSampleA, indexA);
	OutBLayerA[coord] = lerp(fillValue, inSampleB, indexA);
	OutALayerB[coord] = lerp(fillValue, inSampleA, indexB);
	OutBLayerB[coord] = lerp(fillValue, inSampleB, indexB);
}

[numthreads(GROUP_SIZE_X, GROUP_SIZE_Y, 1)]
void main( uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex )
{
	if (DTid.x >= uint(gSatDimensions.x) / 2)
		return;

	const uint2 coordA = uint2(2 * DTid.x, DTid.y);
	const uint2 coordB = uint2(2 * DTid.x + 1, DTid.y);

	ComputeAdaptiveLayerForCoord(coordA);
	ComputeAdaptiveLayerForCoord(coordB);
}