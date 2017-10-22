#include "Constants.fxh"

float4x4 World;
float4x4 WorldViewProj;
float4x4 WorldInverseTranspose;

float3 AmbientLightColor;

float3 DirLight0Direction;
float3 DirLight0DiffuseColor;
float3 DirLight0SpecularColor;
float4x4 DirLight0WorldViewProj;
float4x4 DirLight0WorldViewProj1;
float4x4 DirLight0WorldViewProj2;
texture DirLight0ShadowMap;
sampler DirLight0ShadowMapSampler = sampler_state
{
	texture = <DirLight0ShadowMap>;
	MipFilter = Point;
	MinFilter = Point;
	MagFilter = Point;
	AddressU = clamp;
	AddressV = clamp;
};

float3 DirLight1Direction;
float3 DirLight1DiffuseColor;
float3 DirLight1SpecularColor;
float3 DirLight2Direction;
float3 DirLight2DiffuseColor;
float3 DirLight2SpecularColor;

float3 PointLightDiffuseColors[POINT_MAX_LIGHTS_PER_OBJECT];
float3 PointLightPositions[POINT_MAX_LIGHTS_PER_OBJECT];
float3 PointLightSpecularColors[POINT_MAX_LIGHTS_PER_OBJECT];
float PointLightAttenuations[POINT_MAX_LIGHTS_PER_OBJECT];
uint PointLightCount;
float ShadowSampleAddition;

textureCUBE Point0ShadowMap;
samplerCUBE Point0ShadowMapSampler = sampler_state
{
	texture = <Point0ShadowMap>;
	MipFilter = Point;
	MinFilter = Point;
	MagFilter = Point;
};

float4 BoundingFrustum[4];
float4 CustomClippingPlane;

float4x3 Bones[SKINNED_EFFECT_MAX_BONES];

texture DiffuseMap;
sampler DiffuseSampler = sampler_state
{
	texture = <DiffuseMap>;
	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	AddressU = Wrap;
	AddressV = Wrap;
};

texture NormalMap;
sampler NormalSampler = sampler_state
{
	texture = <NormalMap>;
	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	AddressU = Wrap;
	AddressV = Wrap;
};

float3 EyePosition;

float3 DiffuseColor;
float3 SpecularColor;
float Glossiness;
float Transparency;
float3 padding01;

struct VertexShaderInput
{
    float4 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL;
};

struct VertexShaderInputSkinned
{
	float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL;
	int4 Indices : BLENDINDICES0;
	float4 Weights : BLENDWEIGHT0;
};

struct VertexShaderOutput
{
	float4 Position : POSITION;
	float4 PositionWS : TEXCOORD0;
	float2 TexCoord : TEXCOORD1;
	float3 Normal : TEXCOORD2;
	float4 ClipPlanes : TEXCOORD3;
	float CustomClipPlane : TEXCOORD4;
};

struct VertexShaderOutputShadows
{
	float4 Position : POSITION0;
	float4 PositionWS : TEXCOORD0;
	float2 TexCoord : TEXCOORD1;
	float3 Normal : TEXCOORD2;
	float4 ClipPlanes : TEXCOORD3;
	float CustomClipPlane : TEXCOORD4;
	float4 PositionDLS : TEXCOORD5;
	float4 PositionDLS_1 : TEXCOORD6;
	float4 PositionDLS_2 : TEXCOORD7;
};

struct ColorPair
{
	float3 Diffuse;
	float3 Specular;
};

inline void ComputeSingleLight(float3 L, float3 color, float3 specularColor, float3 E, float3 N, inout ColorPair pair)
{
	float3 H = normalize(normalize(E) + L);
	float intensity = max(dot(L, N), 0.0f);
	float3 specular = pow(max(0.0000001f, dot(H, N)), Glossiness) * length(specularColor);

	pair.Diffuse += intensity * color;
	pair.Specular += specular * specularColor * pair.Diffuse;
}

inline float ChebyshevUpperBound(float2 moments, float t, float minVariance)
{
	// One-tailed inequality if t > Moments.x
	float p = (t <= moments.x);

	// compute variance
	float variance = moments.y - (moments.x * moments.x);
	variance = max(variance, minVariance);

	// compute probabilistic upper bound
	float d = t - moments.x;
	float pMax = variance / (variance + d * d);
	
	return max(p, pMax);
}

inline float LinStep(float min, float max, float v)
{
	return clamp((v - min) / (max - min), 0.0f, 1.0f);
}

inline float ShadowContribution(float pixelDepth, float4 dirPos, float4 dirPos1, float4 dirPos2, float theta)
{
	[branch]
	if (pixelDepth >= CAM_DIRECTIONAL_BOUNDARY_2)
		return 1.0f;

	float2 sampleVec = float2(0.0f, 0.0f);

	float minVariance = MIN_VARIANCE_0;
	float blurOffset = BLUR_OFFSET_0;

	[branch]
	if (pixelDepth > CAM_DIRECTIONAL_BOUNDARY_0 && pixelDepth <= CAM_DIRECTIONAL_BOUNDARY_1)
	{
		dirPos = dirPos1;
		sampleVec = float2(0.5f, 0.0f);
		minVariance = MIN_VARIANCE_1;
		blurOffset = BLUR_OFFSET_1;
	}
	else if (pixelDepth > CAM_DIRECTIONAL_BOUNDARY_1)
	{
		dirPos = dirPos2;
		sampleVec = float2(0.0f, 0.5f);
		minVariance = MIN_VARIANCE_2;
		blurOffset = BLUR_OFFSET_2;
	}

	float2 projectedDLScoords;
	projectedDLScoords.x = ((dirPos.x / dirPos.w)/ 2.0f + 0.5f) / 2.0f + sampleVec.x;
	projectedDLScoords.y = ((-dirPos.y / dirPos.w)/ 2.0f + 0.5f) / 2.0f + sampleVec.y;

	float2 depth = float2(0.0f, 0.0f);
	float ctr = 0.0f;
	for (float i = -BLUR_SIZE; i <= BLUR_SIZE; i += 1.0f)
	{
		for (float j = -BLUR_SIZE; j <= BLUR_SIZE; j += 1.0f)
		{
			depth = depth + tex2Dproj(DirLight0ShadowMapSampler, float4(
				projectedDLScoords + float2(i, j) * blurOffset, 1.0f, 1.0f));
			ctr += 1.0f;
		}
	}

	depth = depth / ctr;

	float shadow = ChebyshevUpperBound(depth, dirPos.z / dirPos.w, minVariance * theta);
	shadow = LinStep(BLEED_REDUCTION, 1.0f, shadow);
	return shadow;
}

inline float ShadowContributionPoint(float pixelDepth, float3 dirPos, float att, float theta)
{
	dirPos = normalize(-(float3(dirPos.x, dirPos.y, -dirPos.z) * att));

	float2 depth = float2(0.0f, 0.0f);
		float ctr = 0.0f;

	for (float i = -BLUR_SIZE_POINT; i <= BLUR_SIZE_POINT; i += 1.0f)
	{
		for (float j = -BLUR_SIZE_POINT; j <= BLUR_SIZE_POINT; j += 1.0f)
		{
			depth = depth + texCUBE(Point0ShadowMapSampler, dirPos + float3(i, j, -BLUR_SIZE) * BLUR_OFFSET_POINT).rg;
			depth = depth + texCUBE(Point0ShadowMapSampler, dirPos + float3(i, j, 0.0f) * BLUR_OFFSET_POINT).rg;
			depth = depth + texCUBE(Point0ShadowMapSampler, dirPos + float3(i, j, BLUR_SIZE) * BLUR_OFFSET_POINT).rg;
			ctr += 3.0f;
		}
	}
	depth = depth / ctr;

	//depth = texCUBE(Point0ShadowMapSampler, dirPos).rg;

	float shadow = ChebyshevUpperBound(depth, pixelDepth / SHADOW_POINT_MAX_DIST, MIN_VARIANCE_0 * theta / 10.0f);
	shadow = LinStep(BLEED_REDUCTION, 1.0f, shadow);
	return shadow;
}

//inline float ShadowContributionOld(float pixelDepth, float4 dirPos, float4 dirPos1, float4 dirPos2)
//{
//	[branch]
//	if (pixelDepth >= CAM_DIRECTIONAL_BOUNDARY_2)
//		return 1.0f;
//
//	float3 depthVec = float3(1.0f, 0.0f, 0.0f);
//
//		[branch]
//	if (pixelDepth > CAM_DIRECTIONAL_BOUNDARY_0 && pixelDepth <= CAM_DIRECTIONAL_BOUNDARY_1)
//	{
//		dirPos = dirPos1;
//		depthVec = float3(0.0f, 1.0f, 0.0f);
//	}
//	else if (pixelDepth > CAM_DIRECTIONAL_BOUNDARY_1)
//	{
//		dirPos = dirPos2;
//		depthVec = float3(0.0f, 0.0f, 1.0f);
//	}
//
//	float2 projectedDLScoords;
//	projectedDLScoords.x = (dirPos.x / dirPos.w) / 2.0f + 0.5f;
//	projectedDLScoords.y = (-dirPos.y / dirPos.w) / 2.0f + 0.5f;
//
//	float2 depth = tex2D(DirLight0ShadowMapSampler, projectedDLScoords).rg * depthVec.x;
//
//	depth = max(depth, max(depth1, depth2));
//
//	float shadow = saturate(exp(max(ESM_MIN, ESM_K * (depth - ((dirPos.z / dirPos.w) - SHADOW_BIAS)))));
//	shadow = 1.0f - (ESM_DIFFUSE_SCALE * (1.0f - shadow));
//	shadow = saturate(shadow);
//
//	shadow = LinStep(BLEED_REDUCTION, 1.0f, shadow);
//	return shadow;
//}

inline ColorPair ComputeLight(float3 posWS, float3 E, float3 N)
{
	E = normalize(E);
	N = normalize(N);

	ColorPair result;
	ColorPair temp;

	result.Diffuse = AmbientLightColor;
	result.Specular = 0;
	temp.Diffuse = 0;
	temp.Specular = 0;

	// DirLight0
	ComputeSingleLight(-DirLight0Direction, DirLight0DiffuseColor,
		float3(DirLight0SpecularColor.x * SpecularColor.x, DirLight0SpecularColor.y * SpecularColor.y, DirLight0SpecularColor.z * SpecularColor.z), E, N, result);

	// DirLight1
	ComputeSingleLight(-DirLight1Direction, DirLight1DiffuseColor,
		float3(DirLight1SpecularColor.x * SpecularColor.x, DirLight1SpecularColor.y * SpecularColor.y, DirLight1SpecularColor.z * SpecularColor.z), E, N, result);

	// DirLight2
	ComputeSingleLight(-DirLight2Direction, DirLight2DiffuseColor,
		float3(DirLight2SpecularColor.x * SpecularColor.x, DirLight2SpecularColor.y * SpecularColor.y, DirLight2SpecularColor.z * SpecularColor.z), E, N, result);

	// point lights
	float3 L;
	float Llength;
	float att;
	for (uint i = 0; i < PointLightCount; ++i)
	{
		L = PointLightPositions[i] - posWS;
		Llength = length(L);
		ComputeSingleLight(normalize(L), PointLightDiffuseColors[i],
			float3(PointLightSpecularColors[i].x * SpecularColor.x, PointLightSpecularColors[i].y * SpecularColor.y, PointLightSpecularColors[i].z * SpecularColor.z),
			E, N, temp);

		att = saturate(ATTENUATION_MULTIPLIER * length(PointLightDiffuseColors[i]) * PointLightAttenuations[i] / max(Llength * Llength, MINIMUM_LENGTH_VALUE));
		temp.Diffuse = temp.Diffuse * att;
		temp.Specular = temp.Specular * att;
		result.Diffuse += temp.Diffuse;
		result.Specular += temp.Specular;

		temp.Diffuse = 0;
		temp.Specular = 0;
	}


	return result;
}

inline ColorPair ComputeLightShadows(float3 posWS, float3 E, float3 N, float4 dirPos, float4 dirPos1, float4 dirPos2)
{
	float pixelDepth = length(E);

	E = normalize(E);
	N = normalize(N);

	ColorPair result;
	ColorPair temp;

	result.Diffuse = AmbientLightColor;
	result.Specular = 0;
	temp.Diffuse = 0;
	temp.Specular = 0;

	float theta = tan(acos(saturate(dot(N, -DirLight0Direction))));

	// DirLight0
	ComputeSingleLight(-DirLight0Direction, DirLight0DiffuseColor,
		float3(DirLight0SpecularColor.x * SpecularColor.x, DirLight0SpecularColor.y * SpecularColor.y, DirLight0SpecularColor.z * SpecularColor.z), E, N, result);

	float shadow = ShadowContribution(pixelDepth, dirPos, dirPos1, dirPos2, theta);
	result.Diffuse = lerp(AmbientLightColor, result.Diffuse, shadow);
	result.Specular = lerp(0.0f, result.Specular, shadow);

	// DirLight1
	ComputeSingleLight(-DirLight1Direction, DirLight1DiffuseColor,
		float3(DirLight1SpecularColor.x * SpecularColor.x, DirLight1SpecularColor.y * SpecularColor.y, DirLight1SpecularColor.z * SpecularColor.z), E, N, result);

	// DirLight2
	ComputeSingleLight(-DirLight2Direction, DirLight2DiffuseColor,
		float3(DirLight2SpecularColor.x * SpecularColor.x, DirLight2SpecularColor.y * SpecularColor.y, DirLight2SpecularColor.z * SpecularColor.z), E, N, result);

	// point lights
	float3 L;
	float Llength;
	float att;

	if (PointLightCount < 1) return result;

	// point light 01

	L = PointLightPositions[0] - posWS;
	//L.z = -L.z;
	Llength = length(L);
	att = saturate(ATTENUATION_MULTIPLIER * length(PointLightDiffuseColors[0]) * PointLightAttenuations[0] / max(Llength * Llength, MINIMUM_LENGTH_VALUE));

	ComputeSingleLight(normalize(L), PointLightDiffuseColors[0],
		float3(PointLightSpecularColors[0].x * SpecularColor.x, PointLightSpecularColors[0].y * SpecularColor.y, PointLightSpecularColors[0].z * SpecularColor.z),
		E, N, temp);

	float shadowP = ShadowContributionPoint(Llength, L, att, theta) + ShadowSampleAddition;

	temp.Diffuse = temp.Diffuse * att;
	temp.Diffuse = lerp(0.0f, temp.Diffuse, saturate(shadowP));
	temp.Specular = lerp(0.0f, temp.Specular, saturate(shadowP)) * att;
	result.Diffuse += temp.Diffuse;
	result.Specular += temp.Specular;

	temp.Diffuse = 0;
	temp.Specular = 0;

#ifdef RELEASE

	for (uint i = 1; i < PointLightCount; ++i)
	{
		L = PointLightPositions[i] - posWS;
		Llength = length(L);
		ComputeSingleLight(normalize(L), PointLightDiffuseColors[i],
			float3(PointLightSpecularColors[i].x * SpecularColor.x, PointLightSpecularColors[i].y * SpecularColor.y, PointLightSpecularColors[i].z * SpecularColor.z),
			E, N, temp);

		att = saturate(ATTENUATION_MULTIPLIER * length(PointLightDiffuseColors[i]) * PointLightAttenuations[i] / max(Llength * Llength, MINIMUM_LENGTH_VALUE));
		temp.Diffuse = temp.Diffuse * att;
		temp.Specular = temp.Specular * att;
		result.Diffuse += temp.Diffuse;
		result.Specular += temp.Specular;

		temp.Diffuse = 0;
		temp.Specular = 0;
	}

#endif

	return result;
}

inline void Skin(inout VertexShaderInputSkinned input)
{
	float4x3 skinning = 0;

		[unroll]
	for (int i = 0; i < WEIGHTS_PER_VERTEX; ++i)
	{
		skinning += Bones[input.Indices[i]] * input.Weights[i];
	}

	input.Position.xyz = mul(input.Position, skinning);
	input.Normal = mul(input.Normal, (float3x3)skinning);
}

VertexShaderOutput VertexShaderFunction(VertexShaderInput input)
{
    VertexShaderOutput output;

	output.Position = mul(input.Position, WorldViewProj);

	output.PositionWS = mul(input.Position, World);

	output.TexCoord = input.TexCoord;

	output.Normal = normalize(mul(input.Normal, WorldInverseTranspose));

	output.ClipPlanes.x = dot(output.PositionWS, BoundingFrustum[0]);
	output.ClipPlanes.y = dot(output.PositionWS, BoundingFrustum[1]);
	output.ClipPlanes.z = dot(output.PositionWS, BoundingFrustum[2]);
	output.ClipPlanes.w = dot(output.PositionWS, BoundingFrustum[3]);
	output.CustomClipPlane = dot(output.PositionWS, CustomClippingPlane);

    return output;
}


VertexShaderOutput VertexShaderFunctionSkinned(VertexShaderInputSkinned input)
{
	VertexShaderOutput output;

	Skin(input);

	output.Position = mul(input.Position, WorldViewProj);

	output.PositionWS = mul(input.Position, World);

	output.TexCoord = input.TexCoord;

	output.Normal = normalize(mul(input.Normal, WorldInverseTranspose));

	output.ClipPlanes.x = dot(output.PositionWS, BoundingFrustum[0]);
	output.ClipPlanes.y = dot(output.PositionWS, BoundingFrustum[1]);
	output.ClipPlanes.z = dot(output.PositionWS, BoundingFrustum[2]);
	output.ClipPlanes.w = dot(output.PositionWS, BoundingFrustum[3]);
	output.CustomClipPlane = dot(output.PositionWS, CustomClippingPlane);

	return output;
}

VertexShaderOutputShadows VertexShaderFunctionShadows(VertexShaderInput input)
{
	VertexShaderOutputShadows output;

	output.Position = mul(input.Position, WorldViewProj);

	output.PositionWS = mul(input.Position, World);

	output.TexCoord = input.TexCoord;

	output.Normal = normalize(mul(input.Normal, WorldInverseTranspose));

	output.ClipPlanes.x = dot(output.PositionWS, BoundingFrustum[0]);
	output.ClipPlanes.y = dot(output.PositionWS, BoundingFrustum[1]);
	output.ClipPlanes.z = dot(output.PositionWS, BoundingFrustum[2]);
	output.ClipPlanes.w = dot(output.PositionWS, BoundingFrustum[3]);
	output.CustomClipPlane = dot(output.PositionWS, CustomClippingPlane);

	output.PositionDLS = mul(input.Position, DirLight0WorldViewProj);
	output.PositionDLS_1 = mul(input.Position, DirLight0WorldViewProj1);
	output.PositionDLS_2 = mul(input.Position, DirLight0WorldViewProj2);

	return output;
}

VertexShaderOutputShadows VertexShaderFunctionSkinnedShadows(VertexShaderInputSkinned input)
{
	VertexShaderOutputShadows output;

	Skin(input);

	output.Position = mul(input.Position, WorldViewProj);

	output.PositionWS = mul(input.Position, World);

	output.TexCoord = input.TexCoord;

	output.Normal = normalize(mul(input.Normal, WorldInverseTranspose));

	output.ClipPlanes.x = dot(output.PositionWS, BoundingFrustum[0]);
	output.ClipPlanes.y = dot(output.PositionWS, BoundingFrustum[1]);
	output.ClipPlanes.z = dot(output.PositionWS, BoundingFrustum[2]);
	output.ClipPlanes.w = dot(output.PositionWS, BoundingFrustum[3]);
	output.CustomClipPlane = dot(output.PositionWS, CustomClippingPlane);

	output.PositionDLS = mul(input.Position, DirLight0WorldViewProj);
	output.PositionDLS_1 = mul(input.Position, DirLight0WorldViewProj1);
	output.PositionDLS_2 = mul(input.Position, DirLight0WorldViewProj2);

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

	float4 color = tex2D(DiffuseSampler, input.TexCoord);
	float alpha = color.a;
	color.a = 1.0f;

	// computin normals

	float4 nAdj = (tex2D(NormalSampler, input.TexCoord));
	input.Normal = normalize(input.Normal);

	nAdj.x = (nAdj.x * 2) - 1;
	nAdj.y = (nAdj.y * 2) - 1;
	nAdj.z = (nAdj.z) - 1;

	input.Normal = input.Normal + nAdj.xyz;
	input.Normal = normalize(input.Normal);

	////////

	ColorPair computedLight = ComputeLight(input.PositionWS.xyz, EyePosition - input.PositionWS.xyz, input.Normal);

	color = color * float4(DiffuseColor, 1.0f) * float4(computedLight.Diffuse, 1.0f) + alpha * float4(computedLight.Specular, 1.0f);

	color *= Transparency * nAdj.a;

    return color;
}

float4 PixelShaderFunctionShadows(VertexShaderOutputShadows input) : COLOR0
{
	// clippin

	clip(input.ClipPlanes.x);
	clip(input.ClipPlanes.y);
	clip(input.ClipPlanes.z);
	clip(input.ClipPlanes.w);
	clip(input.CustomClipPlane);

	//////

	float4 color = tex2D(DiffuseSampler, input.TexCoord);
		float alpha = color.a;
	color.a = 1.0f;

	// computin normals

	float4 nAdj = (tex2D(NormalSampler, input.TexCoord));
		input.Normal = normalize(input.Normal);

	nAdj.x = (nAdj.x * 2) - 1;
	nAdj.y = (nAdj.y * 2) - 1;
	nAdj.z = (nAdj.z) - 1;

	input.Normal = input.Normal + nAdj.xyz;
	input.Normal = normalize(input.Normal);

	////////

	ColorPair computedLight = ComputeLightShadows(input.PositionWS.xyz, EyePosition - input.PositionWS.xyz, input.Normal, input.PositionDLS, input.PositionDLS_1, input.PositionDLS_2);

	color = color * float4(DiffuseColor, 1.0f) * float4(computedLight.Diffuse, 1.0f) + alpha * float4(computedLight.Specular, 1.0f);

	color *= Transparency * nAdj.a;

	return color;
}

float4 PixelShaderFunctionUnlit(VertexShaderOutput input) : COLOR0
{
	// clippin

	clip(input.ClipPlanes.x);
	clip(input.ClipPlanes.y);
	clip(input.ClipPlanes.z);
	clip(input.ClipPlanes.w);
	clip(input.CustomClipPlane);

	//////

	float4 color = tex2D(DiffuseSampler, input.TexCoord);
		float alpha = color.a;
	color.a = 1.0f;

	// computin normals

	float4 nAdj = (tex2D(NormalSampler, input.TexCoord));
		input.Normal = normalize(input.Normal);

	nAdj.x = (nAdj.x * 2) - 1;
	nAdj.y = (nAdj.y * 2) - 1;
	nAdj.z = (nAdj.z) - 1;

	input.Normal = input.Normal + nAdj.xyz;
	input.Normal = normalize(input.Normal);

	color *= Transparency * nAdj.a;

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

technique Skinned
{
	pass Pass1
	{
		VertexShader = compile vs_3_0 VertexShaderFunctionSkinned();
		PixelShader = compile ps_3_0 PixelShaderFunction();
	}
}

technique MainShadows
{
	pass Pass1
	{
		VertexShader = compile vs_3_0 VertexShaderFunctionShadows();
		PixelShader = compile ps_3_0 PixelShaderFunctionShadows();
	}
}

technique SkinnedShadows
{
	pass Pass1
	{
		VertexShader = compile vs_3_0 VertexShaderFunctionSkinnedShadows();
		PixelShader = compile ps_3_0 PixelShaderFunctionShadows();
	}
}

technique Unlit
{
	pass Pass1
	{
		VertexShader = compile vs_3_0 VertexShaderFunction();
		PixelShader = compile ps_3_0 PixelShaderFunctionUnlit();
	}
};