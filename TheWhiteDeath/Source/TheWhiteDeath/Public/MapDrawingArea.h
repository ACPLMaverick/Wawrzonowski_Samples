// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "MapDrawingArea.generated.h"

/**
 * 
 */
UCLASS()
class THEWHITEDEATH_API UMapDrawingArea : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	TArray<FVector2D> _lineSegmentsToDraw;
	FGeometry _mGeometry;
	const FLinearColor _markingDrawColor;
	FLinearColor _drawColor;
	FVector2D _cachedSum;
	FVector2D _cachedMiddlePoint;

	const float _colorMplier = 0.05f;
	const float _alpha = 0.8f;
	const float _minDrawDistance = 40.0f;
	const float _minCircleDistance = 150.0f;
	const float _maxCircleDistance = 75.0f;
	const int32 _minPointsToBeCircle = 8;

	float _sqrRadius = 0.0f;

	bool _bIsMarkingObjective = false;

	virtual void NativePaint(FPaintContext& InContext) const override;

public:

	UMapDrawingArea(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SetAsMarkingObjective();

	void AddLinePointToDraw(FVector2D line);
	bool IsMarkingObjective() const { return _bIsMarkingObjective; }
	bool CheckIfDrawnCircle() const;
	FVector2D GetMiddlePoint() const;
	float GetRadius() const { return 0.5f * FMath::Sqrt(_sqrRadius); }
};
