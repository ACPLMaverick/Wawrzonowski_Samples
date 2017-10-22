#include "../_global/GlobalDefines.hlsli"

DPixelInput main(in uint vertexID : SV_VertexID)
{
	return GenerateDPixelInput(vertexID);
}