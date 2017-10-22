// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "MapCursorWidget.generated.h"

UENUM(BlueprintType)
enum class EMapCursorMode : uint8
{
	Invisible		UMETA(DisplayName = "Invisible"),
	MoveMap			UMETA(DisplayName = "MoveMap"),
	Draw			UMETA(DisplayName = "Draw")
};


/**
 * 
 */
UCLASS()
class THEWHITEDEATH_API UMapCursorWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region Properties
		
protected:

	UPROPERTY(Category = "MapCursor", EditAnywhere, BlueprintReadWrite)
	EMapCursorMode CursorMode = EMapCursorMode::MoveMap;

	UPROPERTY(Category = "MapCursor", EditAnywhere, BlueprintReadWrite)
	TArray<UTexture2D*> CursorTextures;

#pragma endregion

#pragma region Properties

	UImage* _cursorImage;

#pragma endregion
	
#pragma region Functions Public

public:

	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	EMapCursorMode GetCursorMode() { return CursorMode; }

	void SetCursorMode(EMapCursorMode mode);

#pragma endregion
};
