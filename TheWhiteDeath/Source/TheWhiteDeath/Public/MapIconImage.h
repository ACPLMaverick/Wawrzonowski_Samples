// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "MapIconImage.generated.h"

UENUM(BlueprintType)
enum class EMapIconType : uint8
{
	Enemy			UMETA(DisplayName = "Enemy"),
	Draw			UMETA(DisplayName = "Draw mode"),
	Sight			UMETA(DisplayName = "Lookout place"),
	Doors			UMETA(DisplayName = "Entrance"),
	Ammo			UMETA(DisplayName = "Ammunition"),
	Objective		UMETA(DisplayName = "Objective")
};
/**
 * 
 */
UCLASS()
class THEWHITEDEATH_API UMapIconImage : public UUserWidget
{
	GENERATED_BODY()

protected:

	FVector _worldPosition;

	class UMapWidget* _map;
	bool _placedOnMap;

public:

	UPROPERTY(Category = "IconImage", EditAnywhere, BlueprintReadWrite)
	EMapIconType IconType;

	UMapIconImage(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	EMapIconType GetIconType() { return IconType; }
	int32 GetIconTypeAsInt() { return (int32)IconType; }
	virtual bool CanBeClonedAndPlaced() { return true; }
	virtual void OnMapPlacementAction(const FGeometry& ExtGeometry, const FPointerEvent& ExtPointerEvent, class UMapWidget* map, const FVector& worldPosition);
	virtual void OnMapEraseAction(const FGeometry& ExtGeometry, const FPointerEvent& ExtPointerEvent);
};
