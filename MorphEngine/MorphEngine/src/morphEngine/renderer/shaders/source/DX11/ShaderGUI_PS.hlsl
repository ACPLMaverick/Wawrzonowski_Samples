
// CBUFFERS

cbuffer BufferPerObject
{
	float4 gColBase;
	float4 gColBase1;
	float4 gColBase2;
	float4 gColBase3;
	uint2 gIndices;	// [0] == 4 <- is text; otherwise [0] and [1] indicate indices of textures to lerp between
	float gLerp;
};

// TEXTURES

Texture2D texBase : register(t0);
SamplerState smpBase : register(s0);

Texture2D texBase1 : register(t1);
SamplerState smpBase1 : register(s1);

Texture2D texBase2 : register(t2);
SamplerState smpBase2 : register(s2);

Texture2D texBase3 : register(t3);
SamplerState smpBase3 : register(s3);

// STRUCTS

struct PixelInput
{
	float4 Position : SV_POSITION;
	float2 Uv : TEXCOORD0;
};

// MAIN

float4 main(PixelInput input) : SV_Target
{
	float4 col = texBase.Sample(smpBase, input.Uv);
	if (gIndices[0] == 4)
	{
		// text
		return col.rrrr * gColBase;
	}
	else if(gIndices[0] == 0 && gIndices[1] == -1)
	{
		// image
		return col * gColBase;
	}
	else
	{
		// button
		float4 cols[4] = 
		{ 
			col * gColBase, 
			texBase1.Sample(smpBase1, input.Uv) * gColBase1, 
			texBase2.Sample(smpBase2, input.Uv) * gColBase2, 
			texBase3.Sample(smpBase3, input.Uv) * gColBase3 
		};

		if (gIndices[0] == gIndices[1])
		{
			return cols[gIndices[0]];
		}
		else
		{
			return lerp(cols[gIndices[0]], cols[gIndices[1]], gLerp);
		}
	}
}