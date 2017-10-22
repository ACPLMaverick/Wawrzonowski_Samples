#include "Constants.fxh"

float4x4 World;
float4x4 WorldViewProj;

float4x4 ViewProj1;
float4x4 ViewProj2;

float4 BoundingFrustum[4];
float4 CustomClippingPlane;

float4 BoundingFrustum1[4];
float4 CustomClippingPlane1;

float4 BoundingFrustum2[4];
float4 CustomClippingPlane2;

float4x3 Bones[SKINNED_EFFECT_MAX_BONES];

struct VertexShaderInput
{
    float4 Position : POSITION0;
};

struct VertexShaderOutput
{
	float4 Position : POSITION0;
	float4 Position2D : TEXCOORD0;
	float4 ClipPlanes : TEXCOORD1;
	float CustomClipPlane : TEXCOORD2;
};


VertexShaderOutput VertexShaderFunction0(VertexShaderInput input)
{
    VertexShaderOutput output;

	output.Position = mul(input.Position, WorldViewProj);
	output.Position.x = output.Position.x / 2.0f - 0.5f;
	output.Position.y = output.Position.y / 2.0f + 0.5f;
	output.Position2D = output.Position;
	float4 positionWS = mul(input.Position, World);

	output.ClipPlanes.x = dot(positionWS, BoundingFrustum[0]);
	output.ClipPlanes.y = dot(positionWS, BoundingFrustum[1]);
	output.ClipPlanes.z = dot(positionWS, BoundingFrustum[2]);
	output.ClipPlanes.w = dot(positionWS, BoundingFrustum[3]);
	output.CustomClipPlane = dot(positionWS, CustomClippingPlane);

    return output;
}

VertexShaderOutput VertexShaderFunction1(VertexShaderInput input)
{
	VertexShaderOutput output;

	output.Position = mul(input.Position, mul(World, ViewProj1));
	output.Position.x = output.Position.x / 2.0f + 0.5f;
	output.Position.y = output.Position.y / 2.0f + 0.5f;
	output.Position2D = output.Position;
	float4 positionWS = mul(input.Position, World);

		output.ClipPlanes.x = dot(positionWS, BoundingFrustum1[0]);
	output.ClipPlanes.y = dot(positionWS, BoundingFrustum1[1]);
	output.ClipPlanes.z = dot(positionWS, BoundingFrustum1[2]);
	output.ClipPlanes.w = dot(positionWS, BoundingFrustum1[3]);
	output.CustomClipPlane = dot(positionWS, CustomClippingPlane1);

	return output;
}

VertexShaderOutput VertexShaderFunction2(VertexShaderInput input)
{
	VertexShaderOutput output;

	output.Position = mul(input.Position, mul(World, ViewProj2));
	output.Position.x = output.Position.x / 2.0f - 0.5f;
	output.Position.y = output.Position.y / 2.0f - 0.5f;
	output.Position2D = output.Position;
	float4 positionWS = mul(input.Position, World);

		output.ClipPlanes.x = dot(positionWS, BoundingFrustum2[0]);
	output.ClipPlanes.y = dot(positionWS, BoundingFrustum2[1]);
	output.ClipPlanes.z = dot(positionWS, BoundingFrustum2[2]);
	output.ClipPlanes.w = dot(positionWS, BoundingFrustum2[3]);
	output.CustomClipPlane = dot(positionWS, CustomClippingPlane2);

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
	float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float depth = (input.Position2D.z / (input.Position2D.w));
	color.r = depth;
	float dx = ddx(depth);
	float dy = ddy(depth);
	color.g = depth*depth + 0.25f*(dx*dx + dy*dy);

	return color;
}

technique Main
{
    pass Pass0
    {
        VertexShader = compile vs_3_0 VertexShaderFunction0();
        PixelShader = compile ps_3_0 PixelShaderFunction();
    }
	pass Pass1
	{
		VertexShader = compile vs_3_0 VertexShaderFunction1();
		PixelShader = compile ps_3_0 PixelShaderFunction();
	}
	pass Pass2
	{
		VertexShader = compile vs_3_0 VertexShaderFunction2();
		PixelShader = compile ps_3_0 PixelShaderFunction();
	}
};

struct VertexShaderInputSkinned
{
	float4 Position : POSITION0;
	int4 Indices : BLENDINDICES0;
	float4 Weights : BLENDWEIGHT0;
};

inline void Skin(inout VertexShaderInputSkinned input)
{
	float4x3 skinning = 0;

	[unroll]
	for (int i = 0; i < WEIGHTS_PER_VERTEX; ++i)
	{
		skinning += Bones[input.Indices[i]] * input.Weights[i];
	}

	input.Position.xyz = mul(input.Position, skinning);
}

VertexShaderOutput VertexShaderFunctionSkinned0(VertexShaderInputSkinned input)
{
	VertexShaderOutput output;

	Skin(input);

	output.Position = mul(input.Position, WorldViewProj);
	output.Position.x = output.Position.x / 2.0f - 0.5f;
	output.Position.y = output.Position.y / 2.0f + 0.5f;
	output.Position2D = output.Position;
	float4 positionWS = mul(input.Position, World);

	output.ClipPlanes.x = dot(positionWS, BoundingFrustum[0]);
	output.ClipPlanes.y = dot(positionWS, BoundingFrustum[1]);
	output.ClipPlanes.z = dot(positionWS, BoundingFrustum[2]);
	output.ClipPlanes.w = dot(positionWS, BoundingFrustum[3]);
	output.CustomClipPlane = dot(positionWS, CustomClippingPlane);

	return output;
}

VertexShaderOutput VertexShaderFunctionSkinned1(VertexShaderInputSkinned input)
{
	VertexShaderOutput output;

	Skin(input);

	output.Position = mul(input.Position, mul(World, ViewProj1));
	output.Position.x = output.Position.x / 2.0f + 0.5f;
	output.Position.y = output.Position.y / 2.0f + 0.5f;
	output.Position2D = output.Position;
	float4 positionWS = mul(input.Position, World);

		output.ClipPlanes.x = dot(positionWS, BoundingFrustum1[0]);
	output.ClipPlanes.y = dot(positionWS, BoundingFrustum1[1]);
	output.ClipPlanes.z = dot(positionWS, BoundingFrustum1[2]);
	output.ClipPlanes.w = dot(positionWS, BoundingFrustum1[3]);
	output.CustomClipPlane = dot(positionWS, CustomClippingPlane1);

	return output;
}

VertexShaderOutput VertexShaderFunctionSkinned2(VertexShaderInputSkinned input)
{
	VertexShaderOutput output;

	Skin(input);

	output.Position = mul(input.Position, mul(World, ViewProj2));
	output.Position.x = output.Position.x / 2.0f - 0.5f;
	output.Position.y = output.Position.y / 2.0f - 0.5f;
	output.Position2D = output.Position;
	float4 positionWS = mul(input.Position, World);

		output.ClipPlanes.x = dot(positionWS, BoundingFrustum2[0]);
	output.ClipPlanes.y = dot(positionWS, BoundingFrustum2[1]);
	output.ClipPlanes.z = dot(positionWS, BoundingFrustum2[2]);
	output.ClipPlanes.w = dot(positionWS, BoundingFrustum2[3]);
	output.CustomClipPlane = dot(positionWS, CustomClippingPlane2);

	return output;
}

technique Skinned
{
	pass Pass0
	{
		VertexShader = compile vs_3_0 VertexShaderFunctionSkinned0();
		PixelShader = compile ps_3_0 PixelShaderFunction();
	}
	pass Pass1
	{
		VertexShader = compile vs_3_0 VertexShaderFunctionSkinned1();
		PixelShader = compile ps_3_0 PixelShaderFunction();
	}
	pass Pass2
	{
		VertexShader = compile vs_3_0 VertexShaderFunctionSkinned2();
		PixelShader = compile ps_3_0 PixelShaderFunction();
	}
};
