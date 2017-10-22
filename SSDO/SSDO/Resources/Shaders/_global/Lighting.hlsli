/// /////////////////////////////////////////
/// STRUCTS

struct PixelInput
{
	float4 Position : SV_POSITION;
	float3 PositionView : TEXCOORD1;
	float3 Normal : NORMAL;
	float2 Uv : TEXCOORD0;
};

// component a of colSpecular is specular intensity

struct MaterialData
{
	float4 colBase;
	float4 colSpecular;
	float gloss;
};

/// /// /////////////////////////////////////

// FUNCTIONS

void PhongBlinn(float4 inColor,
	float inAtt,
	float3 normal,
	float3 lightDir,
	float3 viewDir,
	MaterialData mData,
	inout float4 color)
{
	float diff = max(dot(normal, lightDir), 0.0f);
	float spec = pow(max(dot(normal, normalize(lightDir + viewDir)), 0.0f), mData.gloss);

	color += diff * inAtt * (inColor * mData.colBase + float4(mData.colSpecular.xyz, 1.0f) * spec * mData.colSpecular.a);
}

/// /////////////////////////////////////
/// 
// LIGHT IMPLEMENTATIONS

void LightAmbient(PixelInput input, float4 color, float3 viewDir, inout float4 outColor)
{
	outColor += color;
}

void LightDirectional(PixelInput input,
	float4 color, float3 direction,
	float3 viewDir, MaterialData mData, inout float4 outColor)
{
	PhongBlinn(color, 1.0f, input.Normal, direction, viewDir, mData, outColor);
}

void LightPoint(PixelInput input,
	float4 color, float3 position, float range,
	float3 viewDir, MaterialData mData, inout float4 outColor)
{
	float3 dir = position - input.PositionView;
	float att = range / (dot(dir, dir));
	dir = normalize(dir);
	PhongBlinn(color, att, input.Normal, dir, viewDir, mData, outColor);
}

void LightSpot(PixelInput input,
	float4 color, float3 position, float3 direction, float range, float angleCos, float smooth,
	float3 viewDir, MaterialData mData, inout float4 outColor)
{
	float3 dir = position - input.PositionView;

	float att = range / (dot(dir, dir));
	dir = normalize(dir);

	float spotDot = dot(direction, dir);
	spotDot *= smoothstep(angleCos * 0.8f, angleCos, spotDot);
	att *= saturate(pow(spotDot, smooth));

	PhongBlinn(color, att, input.Normal, dir, viewDir, mData, outColor);
}

float3 ProjectOnPlane(float3 pos, float3 planePos, float3 planeNormal)
{
	float distance = dot(planeNormal, pos - planePos);
	return pos - distance * planeNormal;
}

float SideOfPlane(float3 pos, float3 planePos, float3 planeNormal)
{
	return step(0.0f, dot(pos - planePos, planeNormal));
}

float3 LinePlaneIntersect(float3 linePos, float3 lineDir, float3 planePos, float3 planeNormal)
{
	return linePos + lineDir * (dot(planeNormal, planePos - linePos) / dot(planeNormal, lineDir));
}

void LightArea(PixelInput input,
	float4 color, float3 position, float3 direction, float3 pRight, float2 size, float range, float smooth,
	float3 viewDir, MaterialData mData, inout float4 outColor)
{
	// calculate plane-related vectors
	float3 pNormal = -direction;
	float3 pUp = cross(pRight, pNormal);

	// project onto plane and calculate direction from light center to the projection.
	float3 projection = ProjectOnPlane(input.PositionView, position, pNormal);
	float3 dirToProjected = projection - position;

	// calculate distance from area
	float2 diagonal = float2(dot(dirToProjected, pRight), dot(dirToProjected, pUp));
	float2 halfSize = size * 0.5f;
	float2 nearest2D = float2(clamp(diagonal.x, -halfSize.x, halfSize.x), clamp(diagonal.y, -halfSize.y, halfSize.y));
	float3 nearestPointInside = float3(position + pRight * nearest2D.x + pUp * nearest2D.y);

	// real distance to area rectangle
	float3 dir = nearestPointInside - input.PositionView;

	float att = range / (dot(dir, dir));
	dir = normalize(dir);

	att *= saturate(pow(dot(direction, dir), smooth));

	PhongBlinn(color, att, input.Normal, dir, viewDir, mData, outColor);
}