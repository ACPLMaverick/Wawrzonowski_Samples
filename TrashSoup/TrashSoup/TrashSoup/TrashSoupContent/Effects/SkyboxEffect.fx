float4x4 World;
float4x4 WorldViewProj;
float4x4 WorldInverseTranspose;

float3 AmbientLightColor;

float4 BoundingFrustum[4];
float4 CustomClippingPlane;

texture CubeMap;
samplerCUBE CubeSampler = sampler_state
{
	texture = <CubeMap>;
	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	AddressU = Mirror;
	AddressV = Mirror;
};

texture CubeMap1;
samplerCUBE CubeSampler1 = sampler_state
{
	texture = <CubeMap1>;
	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	AddressU = Mirror;
	AddressV = Mirror;
};

texture CubeMap2;
samplerCUBE CubeSampler2 = sampler_state
{
	texture = <CubeMap2>;
	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	AddressU = Mirror;
	AddressV = Mirror;
};

texture CubeMap3;
samplerCUBE CubeSampler3 = sampler_state
{
	texture = <CubeMap3>;
	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	AddressU = Mirror;
	AddressV = Mirror;
};

float4 Probes;
float3 DiffuseColor;
float3 EyePosition;

struct VertexShaderInput
{
    float4 Position : POSITION0;
};

struct VertexShaderOutput
{
    float4 Position : POSITION0;
	float3 TexCoord : TEXCOORD0;
	float4 ClipPlanes : TEXCOORD1;
	float CustomClipPlane : TEXCOORD2;
};

VertexShaderOutput VertexShaderFunction(VertexShaderInput input)
{
    VertexShaderOutput output;

    output.Position = mul(input.Position, WorldViewProj);

	float4 vertPos = mul(input.Position, World);
	output.TexCoord = vertPos - EyePosition;

	output.ClipPlanes.x = dot(vertPos, BoundingFrustum[0]);
	output.ClipPlanes.y = dot(vertPos, BoundingFrustum[1]);
	output.ClipPlanes.z = dot(vertPos, BoundingFrustum[2]);
	output.ClipPlanes.w = dot(vertPos, BoundingFrustum[3]);
	output.CustomClipPlane = dot(vertPos, CustomClippingPlane);

    return output;
}

float4 PixelShaderFunction(VertexShaderOutput input) : COLOR0
{
	// clippin

	clip(input.ClipPlanes.x);
	clip(input.ClipPlanes.y);
	clip(input.ClipPlanes.z);
	clip(input.ClipPlanes.w);
	clip(input.CustomClipPlane);

	//////

	float4 probe0 = texCUBE(CubeSampler, normalize(input.TexCoord)) * Probes.x;
	float4 probe1 = texCUBE(CubeSampler1, normalize(input.TexCoord)) * Probes.y;
	float4 probe2 = texCUBE(CubeSampler2, normalize(input.TexCoord)) * Probes.z;
	float4 probe3 = texCUBE(CubeSampler3, normalize(input.TexCoord)) * Probes.w;

	float4 color = float4(DiffuseColor, 1.0f) * (probe0 + probe1 + probe2 + probe3);
	color.a = 1;
    return color;
}

technique Main
{
    pass Pass1
    {
        VertexShader = compile vs_3_0 VertexShaderFunction();
        PixelShader = compile ps_3_0 PixelShaderFunction();
    }
}
