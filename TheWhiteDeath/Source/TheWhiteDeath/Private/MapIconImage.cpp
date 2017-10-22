// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "MapIconImage.h"
#include "Public/MapWidget.h"

UMapIconImage::UMapIconImage(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	_worldPosition(0.0f, 0.0f, 0.0f),
	_map(NULL),
	_placedOnMap(false)
{
	IconType = EMapIconType::Ammo;	// this is teh default type, will be overriden in deriving classes
}

bool UMapIconImage::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	return true;
}

void UMapIconImage::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UMapIconImage::OnMapPlacementAction(const FGeometry & ExtGeometry, const FPointerEvent & ExtPointerEvent, UMapWidget * map, const FVector& worldPosition)
{
	_worldPosition = worldPosition;
	_map = map;
	_placedOnMap = true;
}

void UMapIconImage::OnMapEraseAction(const FGeometry & ExtGeometry, const FPointerEvent & ExtPointerEvent)
{
	_map = NULL;
	_placedOnMap = false;
}
