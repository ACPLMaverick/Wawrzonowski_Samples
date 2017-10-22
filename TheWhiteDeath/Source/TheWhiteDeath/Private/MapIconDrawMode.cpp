// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "MapIconDrawMode.h"
#include "Public/MapWidget.h"


UMapIconDrawMode::UMapIconDrawMode(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

bool UMapIconDrawMode::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	return true;
}

void UMapIconDrawMode::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UMapIconDrawMode::OnMapPlacementAction(const FGeometry& ExtGeometry, const FPointerEvent& ExtPointerEvent, class UMapWidget* map, const FVector& worldPosition)
{
	Super::OnMapPlacementAction(ExtGeometry, ExtPointerEvent, map, worldPosition);
	map->EnableDrawMode();
}

void UMapIconDrawMode::OnMapEraseAction(const FGeometry& ExtGeometry, const FPointerEvent& ExtPointerEvent)
{
	Super::OnMapEraseAction(ExtGeometry, ExtPointerEvent);
}