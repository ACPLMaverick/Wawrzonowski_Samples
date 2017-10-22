#include "Deferred.hlsli"

TEXTURE_INPUT_DEFERRED

TEXTURE_INPUT_POSTPROCESS

float4 main(PixelInputDeferred input) : SV_TARGET
{
	float4 inputSample = TexInput.Sample(SmpInput, input.Uv);
	inputSample.r = (inputSample.r * 0.393f) + (inputSample.g * 0.769f) + (inputSample.b * 0.189f);
	inputSample.g = (inputSample.r * 0.349f) + (inputSample.g * 0.686f) + (inputSample.b * 0.168f);
	inputSample.b = (inputSample.r * 0.272f) + (inputSample.g * 0.534f) + (inputSample.b * 0.131f);
	return inputSample;
}