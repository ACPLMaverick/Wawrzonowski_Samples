#include "Deferred.hlsli"
#include "Lighting.hlsli"


PixelOutputDeferred main(in PixelInput pi)
{
	PixelOutputDeferred output;
	output.Color = float4(1.0f, 0.0f, 1.0f, 1.0f);
	output.Normal = float4(normalize(pi.Normal), 1.0f);
	output.WorldPos = float4(pi.PositionWorld, 1.0f);
	return output;
}