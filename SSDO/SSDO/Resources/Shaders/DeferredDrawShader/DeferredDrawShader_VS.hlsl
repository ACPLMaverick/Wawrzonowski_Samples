#include "../_global/GlobalDefines.hlsli"

DPixelInput main( DVertexInput input )
{
	DPixelInput output;
	output.Position = float4(input.Position, 1.0f);
	output.Uv = input.Uv;
	return output;
}