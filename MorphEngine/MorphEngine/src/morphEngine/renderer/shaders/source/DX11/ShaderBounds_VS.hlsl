// GLOBALS

cbuffer BufferPerObject
{
	float4x4 gTransform;
};

// STRUCTURES

struct VertexInput
{
	float3 Position : POSITION;
};

struct GeometryInput
{
	float4 Position : SV_POSITION;
};

// MAIN

GeometryInput main(VertexInput input, in uint vertexID : SV_VertexID)
{
	GeometryInput output;

	output.Position = mul(gTransform, float4(input.Position, 1.0f));

	return output;
}