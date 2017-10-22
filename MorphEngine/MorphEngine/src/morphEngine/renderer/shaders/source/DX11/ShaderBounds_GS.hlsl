struct GeometryInput
{
	float4 Position : SV_POSITION;
};

struct PixelInput
{
	float4 Position : SV_POSITION;
	float4 Barycentric : TEXCOORD0;
};

[maxvertexcount(3)]
void main(
	triangle GeometryInput input[3],
	inout TriangleStream< PixelInput > output
)
{
	PixelInput elements[3];
	[unroll]
	for (uint i = 0; i < 3; i++)
	{
		elements[i].Position = input[i].Position;
	}

	float distanceSumRec = 
		distance(input[0].Position, input[1].Position) + 
		distance(input[1].Position, input[2].Position) + 
		distance(input[0].Position, input[2].Position);
	distanceSumRec = pow(1.0f / max(distanceSumRec, 0.00001f), 0.8f);

	elements[0].Barycentric = float4(1.0f, 0.0f, 0.0f, distanceSumRec);
	elements[1].Barycentric = float4(0.0f, 1.0f, 0.0f, distanceSumRec);
	elements[2].Barycentric = float4(0.0f, 0.0f, 1.0f, distanceSumRec);

	output.Append(elements[0]);
	output.Append(elements[1]);
	output.Append(elements[2]);
}