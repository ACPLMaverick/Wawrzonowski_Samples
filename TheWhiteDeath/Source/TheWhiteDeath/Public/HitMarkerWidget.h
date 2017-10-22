// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "HitMarkerWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEWHITEDEATH_API UHitMarkerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(Category = "Hit Marker", EditDefaultsOnly, BlueprintReadWrite)
	bool readyToDestroy = false;

	UFUNCTION(BlueprintNativeEvent)
	void PlayAppear(float angle);
	virtual void PlayAppear_Implementation(float angle);
};
