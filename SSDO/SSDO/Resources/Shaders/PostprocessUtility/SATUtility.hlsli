
// FUNCTIONS

void GenerateSamplePointsAndAreaInternal(in const float4 satDimensions, in const float filterHalfWidth,
	in const float2 centerPoint, in const float depth,
	out float2 samplePoints[4], out float areaRec)
{
	const uint sampleCount = 4;
	float tSize = filterHalfWidth * min(satDimensions.x, satDimensions.y) * pow(depth, 0.8f); // how many pixels
	//float diffX = abs(min(centerPoint.x - tSize, 0.0f)) + max(centerPoint.x + tSize - satDimensions.x, 0.0f);
	//float diffY = abs(min(centerPoint.y - tSize, 0.0f)) + max(centerPoint.y + tSize - satDimensions.y, 0.0f);
	//float tSizeX = tSize - diffX + diffY;
	//float tSizeY = tSize - diffY + diffX;

	// TL, TR, BR, BL
	samplePoints[0] = float2(max(centerPoint.x - tSize - 1, -1.0f), max(centerPoint.y - tSize - 1, -1.0f));
	samplePoints[1] = float2(min(centerPoint.x + tSize, satDimensions.x - 1), max(centerPoint.y - tSize - 1, -1.0f));
	samplePoints[2] = float2(min(centerPoint.x + tSize, satDimensions.x - 1), min(centerPoint.y + tSize, satDimensions.y - 1));
	samplePoints[3] = float2(max(centerPoint.x - tSize - 1, -1.0f), min(centerPoint.y + tSize, satDimensions.y - 1));

	areaRec = 1.0f / float(
		(samplePoints[2].x - samplePoints[3].x) *
		(samplePoints[2].y - samplePoints[1].y));
}

void GenerateSamplePointsAndArea(in const float4 satDimensions, in const float filterHalfWidth,
	in const float2 uv, in const float depth,
	out float2 samplePoints[4], out float areaRec)
{
	float2 coord = uv * satDimensions.xy;
	GenerateSamplePointsAndAreaInternal(satDimensions, filterHalfWidth, coord, depth, samplePoints, areaRec);
}

void GenerateSamplePointsAndAreaTexSpace(in const float4 satDimensions, in const float filterHalfWidth,
	in const float2 coord, in const float depth,
	out float2 samplePoints[4], out float areaRec)
{
	GenerateSamplePointsAndAreaInternal(satDimensions, filterHalfWidth, coord, depth, samplePoints, areaRec);
}

void CalculateSum(in const float4 satDimensions, in Texture2D sat, in SamplerState smp, in const float2 samplePoints[4], out float4 sumValue)
{
	float4 values[4] =
	{
		sat.SampleLevel(smp, samplePoints[0] * satDimensions.zw, 0),
		sat.SampleLevel(smp, samplePoints[1] * satDimensions.zw, 0),
		sat.SampleLevel(smp, samplePoints[2] * satDimensions.zw, 0),
		sat.SampleLevel(smp, samplePoints[3] * satDimensions.zw, 0),
	};

	sumValue = (values[2] - values[3] - values[1] + values[0]);
}

void CalculateAverage(in const float4 satDimensions, in Texture2D sat, in SamplerState smp,
	in const float2 samplePoints[4], in const float areaRec, out float4 average)
{
	// SAT: (BR - BL - TR + TL) / area
	float4 values[4] =
	{
		sat.SampleLevel(smp, samplePoints[0] * satDimensions.zw, 0),
		sat.SampleLevel(smp, samplePoints[1] * satDimensions.zw, 0),
		sat.SampleLevel(smp, samplePoints[2] * satDimensions.zw, 0),
		sat.SampleLevel(smp, samplePoints[3] * satDimensions.zw, 0),
	};
	average = (values[2] - values[3] - values[1] + values[0]) * areaRec;
}

void CalculateAverageDifferential(in const float4 satDimensions, in Texture2D satSibling, in Texture2D satParent, 
	in SamplerState smpSibling, in SamplerState smpParent, in const float2 samplePoints[4], in const float areaRec, out float4 average)
{
	// SAT: (BR - BL - TR + TL) / area
	float4 valuesSibling[4] =
	{
		satSibling.SampleLevel(smpSibling, samplePoints[0] * satDimensions.zw, 0),
		satSibling.SampleLevel(smpSibling, samplePoints[1] * satDimensions.zw, 0),
		satSibling.SampleLevel(smpSibling, samplePoints[2] * satDimensions.zw, 0),
		satSibling.SampleLevel(smpSibling, samplePoints[3] * satDimensions.zw, 0),
	};

	float4 valuesParent[4] =
	{
		satParent.SampleLevel(smpParent, samplePoints[0] * satDimensions.zw, 0),
		satParent.SampleLevel(smpParent, samplePoints[1] * satDimensions.zw, 0),
		satParent.SampleLevel(smpParent, samplePoints[2] * satDimensions.zw, 0),
		satParent.SampleLevel(smpParent, samplePoints[3] * satDimensions.zw, 0),
	};

	float4 values[4] = 
	{
		valuesParent[0] - valuesSibling[0],
		valuesParent[1] - valuesSibling[1],
		valuesParent[2] - valuesSibling[2],
		valuesParent[3] - valuesSibling[3],
	};

	average = (values[2] - values[3] - values[1] + values[0]) * areaRec;
}

void CalculateAverageDifferentialGrandparent(in const float4 satDimensions, in Texture2D satSibling, in Texture2D satUncle, in Texture2D satGrandparent,
	in SamplerState smpSibling, in SamplerState smpUncle, in SamplerState smpGrandparent, in const float2 samplePoints[4], in const float areaRec, out float4 average)
{
	float4 valuesSibling[4] =
	{
		satSibling.SampleLevel(smpSibling, samplePoints[0] * satDimensions.zw, 0),
		satSibling.SampleLevel(smpSibling, samplePoints[1] * satDimensions.zw, 0),
		satSibling.SampleLevel(smpSibling, samplePoints[2] * satDimensions.zw, 0),
		satSibling.SampleLevel(smpSibling, samplePoints[3] * satDimensions.zw, 0),
	};

	float4 valuesUncle[4] =	// are calculated twice
	{
		satUncle.SampleLevel(smpUncle, samplePoints[0] * satDimensions.zw, 0),
		satUncle.SampleLevel(smpUncle, samplePoints[1] * satDimensions.zw, 0),
		satUncle.SampleLevel(smpUncle, samplePoints[2] * satDimensions.zw, 0),
		satUncle.SampleLevel(smpUncle, samplePoints[3] * satDimensions.zw, 0),
	};

	float4 valuesGrandparent[4] =
	{
		satGrandparent.SampleLevel(smpGrandparent, samplePoints[0] * satDimensions.zw, 0),
		satGrandparent.SampleLevel(smpGrandparent, samplePoints[1] * satDimensions.zw, 0),
		satGrandparent.SampleLevel(smpGrandparent, samplePoints[2] * satDimensions.zw, 0),
		satGrandparent.SampleLevel(smpGrandparent, samplePoints[3] * satDimensions.zw, 0),
	};

	float4 values[4] =
	{
		(valuesGrandparent[0] - valuesUncle[0]) - valuesSibling[0],
		(valuesGrandparent[1] - valuesUncle[1]) - valuesSibling[1],
		(valuesGrandparent[2] - valuesUncle[2]) - valuesSibling[2],
		(valuesGrandparent[3] - valuesUncle[3]) - valuesSibling[3],
	};

	average = (values[2] - values[3] - values[1] + values[0]) * areaRec;
}