// INCLUDE

#include "Lighting.hlsli"

// GLOBALS

/** Supported Lights
*	Ambient - 1
*	Directional - 4
*	Point - 8
*	Spot - 8
*	Area - 4
*	Total number of lights: 25
*/

static const uint COUNT_LIGHT_DIRECTIONAL = 4;
static const uint COUNT_LIGHT_POINT = 4;
static const uint COUNT_LIGHT_SPOT = 4;
static const uint COUNT_LIGHT_AREA = 4;

cbuffer BufferPerObject
{
	float4 gColBase;
	float4 gColSpecular;
	float gGloss;
	float3 padding;
};

cbuffer Global
{
	// just a viewpos
	float3 viewPos;
};

cbuffer BufferLightGlobal
{
	// directional
	float4 colDirectional[COUNT_LIGHT_DIRECTIONAL];
	float4 dirDirectional[COUNT_LIGHT_DIRECTIONAL];
	// ambient
	float4 colAmbient;
	uint numDirectional;
};

cbuffer BufferLightPoint
{
	uint numPoint;
	float4 colPoint[COUNT_LIGHT_POINT];
	float4 posPoint[COUNT_LIGHT_POINT];
	vector<float, COUNT_LIGHT_POINT> rangePoint;
};

cbuffer BufferLightSpot
{
	uint numSpot;
	float4 colSpot[COUNT_LIGHT_SPOT];
	float4 posSpot[COUNT_LIGHT_SPOT];
	float4 dirSpot[COUNT_LIGHT_SPOT];
	vector<float, COUNT_LIGHT_SPOT> rangeSpot;
	vector<float, COUNT_LIGHT_SPOT> angleCosSpot;
	vector<float, COUNT_LIGHT_SPOT> smoothSpot;
};

cbuffer BufferLightArea
{
	uint numArea;
	float4 colArea[COUNT_LIGHT_AREA];
	float4 posArea[COUNT_LIGHT_AREA];
	float4 dirArea[COUNT_LIGHT_AREA];
	float4 rightArea[COUNT_LIGHT_AREA];
	float4 sizeRangeSmoothArea[COUNT_LIGHT_AREA];
};

// TEXTURES

Texture2D texBase : register(t0);
SamplerState smpBase : register(s0);

Texture2D texNormal : register(t1);
SamplerState smpNormal : register(s1);

// STRUCTS

struct PixelInputMain : PixelInput
{

};


// MAIN

float4 main(PixelInputMain input) : SV_Target
{
	input.Normal = normalize(input.Normal);
	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 viewDir = normalize(viewPos - input.PositionWorld);

	float4 texBaseColor = texBase.Sample(smpBase, input.Uv);

	MaterialData mData;
	mData.colBase = gColBase * float4(texBaseColor.xyz, 1.0f);
	mData.colSpecular = float4(gColSpecular.xyz * texBaseColor.a, 1.0f);
	mData.gloss = gGloss;

	LightAmbient(input, colAmbient, viewDir, color);

	for (uint i = 0; i < numDirectional; ++i)
	{
		LightDirectional(input, colDirectional[i], dirDirectional[i].xyz, viewDir, mData, color);
	}

	for (i = 0; i < numPoint; ++i)
	{
		LightPoint(input, colPoint[i], posPoint[i].xyz, rangePoint[i], viewDir, mData, color);
	}

	for (i = 0; i < numSpot; ++i)
	{
		LightSpot(input, colSpot[i], posSpot[i].xyz, dirSpot[i].xyz, rangeSpot[i], 
			angleCosSpot[i], smoothSpot[i], viewDir, mData, color);
	}

	for (i = 0; i < numArea; ++i)
	{
		LightArea(input, colArea[i], posArea[i].xyz, dirArea[i].xyz, rightArea[i].xyz, 
			sizeRangeSmoothArea[i].xy, sizeRangeSmoothArea[i].z, sizeRangeSmoothArea[i].w, 
			viewDir, mData, color);
	}

	color.a = gColBase.a;

	return color;
}