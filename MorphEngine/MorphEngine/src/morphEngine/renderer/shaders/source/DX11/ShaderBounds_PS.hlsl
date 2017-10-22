struct PixelInput
{
	float4 Position : SV_POSITION;
	float4 Barycentric : TEXCOORD0;
};

float4 main(in PixelInput pi) : SV_TARGET
{
	float sizeFunction = pi.Barycentric.w;
	float3 d = (fwidth(pi.Barycentric.xyz) + float3(0.05f, 0.05f, 0.01f)) * sizeFunction * 10.0f;
	float3 a3 = smoothstep(float3(0.0f, 0.0f, 0.0f), d, pi.Barycentric.xyz);
	float edgeFactor = min(min(a3.x, a3.y), a3.z);
	edgeFactor = clamp(1.0f - edgeFactor, 0.0f, 1.0f);

	return float4(0.85f, 0.85f, 0.85f, edgeFactor);
}