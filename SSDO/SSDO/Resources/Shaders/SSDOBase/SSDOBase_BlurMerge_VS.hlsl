#include "../_global/GlobalDefines.hlsli"

cbuffer BlurMerge : register(b1)
{
	float2 gTexelSize;
	bool gHorizontalBlur;
}

DPixelInput main(in uint vertexID : SV_VertexID)
{
	DPixelInput dp = GenerateDPixelInput(vertexID);
	if (!gHorizontalBlur)
	{
		dp.Uv *= 2.0f;
	}
	return dp;
}