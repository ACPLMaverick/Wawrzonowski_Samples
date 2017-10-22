#include "Deferred.hlsli"

PixelInputDeferred main(in uint vertexID : SV_VertexID)
{
	return GeneratePixelInputDeferred(vertexID);
}