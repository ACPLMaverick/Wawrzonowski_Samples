// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Public/MapIconImage.h"
#include "MapIconWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEWHITEDEATH_API UMapIconWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	class UMapIconImage* _icon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "IconWidget", meta = (AllowPrivateAccess = "true"))
	bool Highlighted = false;

	UPROPERTY(Category = "IconImage", EditAnywhere, BlueprintReadWrite)
	EMapIconType IconType;

public:

	// The class that will be used for skill tree notifications
	UPROPERTY(Category = "IconWidget", EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UMapIconImage> IconClass;
	
	UMapIconWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void CounterRotateIcon(float counterAngle);
	class UMapIconImage* GetIcon() { return _icon; }
	bool IsHighlighted() { return Highlighted; }
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "IconWidget")
		void Highlight();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "IconWidget")
		void UnHighlight();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "IconWidget")
		void UnHighlightImmediately();
};
