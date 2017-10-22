// INCLUDE

// GLOBALS

cbuffer BufferPerObject
{
	uint2 gGameObjectID;
};

struct PixelInput
{
	float4 Position : SV_POSITION;
	float3 PositionWorld : TEXCOORD1;
	float3 Normal : NORMAL;
	float2 Uv : TEXCOORD0;
};


// MAIN

uint2 main(PixelInput input) : SV_Target
{
	return gGameObjectID;
}