#include "../_global/GlobalDefines.hlsli"

DPixelInput main(in uint vertexID : SV_VertexID)
{
	DPixelInput dp = GenerateDPixelInput(vertexID);
	dp.Uv *= 2.0f;
	return dp;
}