// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MapIconImage.h"
#include "MapIconDrawMode.generated.h"

/**
 * 
 */
UCLASS()
class THEWHITEDEATH_API UMapIconDrawMode : public UMapIconImage
{
	GENERATED_BODY()
	
protected:
	
public:

	UMapIconDrawMode(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	virtual bool CanBeClonedAndPlaced() override { return false; }
	virtual void OnMapPlacementAction(const FGeometry& ExtGeometry, const FPointerEvent& ExtPointerEvent, class UMapWidget* map, const FVector& worldPosition) override;
	virtual void OnMapEraseAction(const FGeometry& ExtGeometry, const FPointerEvent& ExtPointerEvent) override;
};
