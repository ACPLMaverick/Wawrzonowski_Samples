
cbuffer BufferPerObject : register(b0)
{
	float4x4 gTransform;
	float4x4 gWV;
};

struct VertexInput
{
	float3 Position : POSITION;
	float2 Uv : TEXCOORD0;
};

struct PixelInput
{
	float4 Position : SV_POSITION;
	float3 PositionView : TEXCOORD1;
	float2 Uv : TEXCOORD0;
};

PixelInput main( VertexInput input )
{
	PixelInput output;

	output.Position = mul(gTransform, float4(input.Position, 1.0f));
	output.PositionView = input.Position;
	output.Uv = input.Uv;

	return output;
}