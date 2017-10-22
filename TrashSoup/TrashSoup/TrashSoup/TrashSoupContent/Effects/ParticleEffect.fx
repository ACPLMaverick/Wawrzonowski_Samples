float4x4 WorldViewProj;

texture DiffuseMap;
sampler2D texSampler = sampler_state{
	texture = <DiffuseMap>;
};

float3 DiffuseColor;
float3 Position;
float3 Up;
float3 Side;
float2 Size;
float Transparency;
float4x4 Rotation;

struct VertexShaderInput
{
    float4 Position : POSITION0;
	float2 UV : TEXCOORD0;
	float3 Color : TEXCOORD1;
};

struct VertexShaderOutput
{
    float4 Position : POSITION0;
	float2 UV : TEXCOORD0;
};

VertexShaderOutput VertexShaderFunction(VertexShaderInput input)
{
    VertexShaderOutput output;

	float3 position = input.Position;

	float4 uvn = float4(input.UV, 0.0f, 1.0f);
	uvn = mul(uvn, Rotation);

	//Move to billboard corner
	float2 offset = Size * float2((uvn.x - 0.5f) * 2.0f,
		-(uvn.y - 0.5f) * 2.0f);
	position += offset.x * Side + offset.y * Up;

	//Move the vertex along its movement direction and the wind direction
	position += (Position);
	
	//Transform the final position by the view and projection matrices
	output.Position = mul(float4(position, 1.0f), WorldViewProj);

	output.UV = input.UV;

    return output;
}

float4 PixelShaderFunction(VertexShaderOutput input) : COLOR0
{
	//Sample texture
	float4 color = tex2D(texSampler, input.UV);

	//Return color * fade amount
	return color * float4(DiffuseColor, 1.0f) * Transparency;
}

technique Technique1
{
    pass Pass1
    {
        // TODO: set renderstates here.

        VertexShader = compile vs_2_0 VertexShaderFunction();
        PixelShader = compile ps_2_0 PixelShaderFunction();
    }
}
