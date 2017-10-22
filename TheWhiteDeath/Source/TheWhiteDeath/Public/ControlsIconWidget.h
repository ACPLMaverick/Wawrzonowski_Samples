// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "ControlsIconWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEWHITEDEATH_API UControlsIconWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(Category = "ControlsIcon", EditAnywhere, BlueprintReadWrite)
	TArray<UTexture2D*> Textures;

	UPROPERTY(Category = "ControlsIcon", EditAnywhere, BlueprintReadWrite)
	FVector2D ImageSize;

	TArray<UImage*> _contrImages;

public:

	virtual bool Initialize() override;

	void ChangeImage(int32 index);
	void ChangeImage(float index, float max);

};
