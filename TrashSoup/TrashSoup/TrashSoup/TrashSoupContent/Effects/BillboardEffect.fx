#include "Constants.fxh"

float4x4 World;
float4x4 WorldViewProj;
float4x4 WorldInverseTranspose;

float3 DiffuseColor;

float4 BoundingFrustum[4];
float4 CustomClippingPlane;

float Transparency;

float3 CameraUp;
float3 CameraRight;
float2 Size;

texture DiffuseMap;
sampler DiffuseSampler = sampler_state
{
	texture = <DiffuseMap>;
	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
};

struct VertexShaderInput
{
	float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;
};

struct VertexShaderOutput
{
	float4 Position : POSITION0;
	float4 PositionWS : TEXCOORD2;
	float2 TexCoord : TEXCOORD0;
	float4 ClipPlanes : TEXCOORD3;
	float CustomClipPlane : TEXCOORD4;
};

VertexShaderOutput VertexShaderFunction(VertexShaderInput input)
{
	VertexShaderOutput output;

	float3 position = input.Position.xyz;

	//Move to billboard corner
	float2 offset = Size * float2((input.TexCoord.x - 0.5f) * 2.0f,
		-(input.TexCoord.y - 0.5f) * 2.0f);
	position += offset.x * CameraRight + offset.y * CameraUp;

	output.Position = mul(float4(position, 1.0f), WorldViewProj);

	output.PositionWS = mul(float4(position, 1.0f), World);

	output.TexCoord = input.TexCoord;

	output.ClipPlanes.x = dot(output.PositionWS, BoundingFrustum[0]);
	output.ClipPlanes.y = dot(output.PositionWS, BoundingFrustum[1]);
	output.ClipPlanes.z = dot(output.PositionWS, BoundingFrustum[2]);
	output.ClipPlanes.w = dot(output.PositionWS, BoundingFrustum[3]);
	output.CustomClipPlane = dot(output.PositionWS, CustomClippingPlane);

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

	float4 color = float4(DiffuseColor, 1.0f) * tex2D(DiffuseSampler, input.TexCoord);

	color *= Transparency;

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