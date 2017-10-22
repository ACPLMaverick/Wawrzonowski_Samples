// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "MapDrawingArea.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

UMapDrawingArea::UMapDrawingArea(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	_markingDrawColor(0.1f, 1.0f, 0.2f),
	_cachedSum(0.0f, 0.0f),
	_cachedMiddlePoint(0.0f, 0.0f)
{

}

bool UMapDrawingArea::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	
	_drawColor = FLinearColor(FMath::FRandRange(0.0f, _colorMplier), FMath::FRandRange(0.0f, _colorMplier), FMath::FRandRange(0.0f, _colorMplier), _alpha);
	_lineSegmentsToDraw.Empty();

	return true;
}

void UMapDrawingArea::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	_mGeometry = MyGeometry;
}

void UMapDrawingArea::SetAsMarkingObjective()
{
	_bIsMarkingObjective = true;
	_drawColor = _markingDrawColor;
}

void UMapDrawingArea::AddLinePointToDraw(FVector2D line)
{
	if (_mGeometry.GetLocalSize() != FVector2D::ZeroVector)
	{
		int32 num = _lineSegmentsToDraw.Num();

		if (num == 0)
		{
			FVector2D nextPoint = _mGeometry.AbsoluteToLocal(line);
			_lineSegmentsToDraw.Add(nextPoint);
			_cachedSum += nextPoint;
		}
		else
		{
			FVector2D localPos = _mGeometry.AbsoluteToLocal(line);
			if (FVector2D::Distance(_lineSegmentsToDraw.Last(), localPos) >= _minDrawDistance)
			{
				// this is non optimal af
				for (int32 i = 0; i < num; ++i)
				{
					float dist = FVector2D::DistSquared(_lineSegmentsToDraw[i], localPos);
					if (dist > _sqrRadius)
					{
						_sqrRadius = dist;
					}
				}

				_lineSegmentsToDraw.Add(localPos);
				_cachedSum += localPos;
			}
		}
	}
}

bool UMapDrawingArea::CheckIfDrawnCircle() const
{
	if (_sqrRadius >= _minCircleDistance * _minCircleDistance &&
		_lineSegmentsToDraw.Num() >= _minPointsToBeCircle &&
		(FVector2D::Distance(_lineSegmentsToDraw[0], _lineSegmentsToDraw.Last()) <= _maxCircleDistance))
	{
		return true;
	}
	else
	{
		return false;
	}
}

FVector2D UMapDrawingArea::GetMiddlePoint() const
{
	return _mGeometry.LocalToAbsolute(_cachedSum / (float)_lineSegmentsToDraw.Num());
}

void UMapDrawingArea::NativePaint(FPaintContext & InContext) const
{
	Super::NativePaint(InContext);

	int32 linesNum = _lineSegmentsToDraw.Num();
	if (linesNum > 1)
	{
		UWidgetBlueprintLibrary::DrawLines(InContext, _lineSegmentsToDraw, _drawColor);
	}
}