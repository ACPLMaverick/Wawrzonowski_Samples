// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Public/MapIconImage.h"
#include "MapFiltersWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFilterChanged, EMapIconType, TypeChanged, bool, NewState);

/**
 * 
 */
UCLASS()
class THEWHITEDEATH_API UMapFiltersWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(Category = "FiltersWidget", EditAnywhere, BlueprintReadWrite)
	FVector2D EmergedPositionDelta;

	UPROPERTY(Category = "FiltersWidget", EditAnywhere, BlueprintReadWrite)
	UTexture2D* ButtonFilterActiveTexture;

	UPROPERTY(Category = "FiltersWidget|Colors", EditAnywhere, BlueprintReadWrite)
	FLinearColor EnabledButtonColor;
	UPROPERTY(Category = "FiltersWidget|Colors", EditAnywhere, BlueprintReadWrite)
	FLinearColor EnabledTextColor;
	UPROPERTY(Category = "FiltersWidget|Colors", EditAnywhere, BlueprintReadWrite)
	FLinearColor EnabledImageColor;
	UPROPERTY(Category = "FiltersWidget|Colors", EditAnywhere, BlueprintReadWrite)
	FLinearColor DisabledButtonColor;
	UPROPERTY(Category = "FiltersWidget|Colors", EditAnywhere, BlueprintReadWrite)
	FLinearColor DisabledTextColor;
	UPROPERTY(Category = "FiltersWidget|Colors", EditAnywhere, BlueprintReadWrite)
	FLinearColor DisabledImageColor;

	TArray<UButton*> _btns;
	TArray<UImage*> _imgs;
	TArray<bool> _states;

	FVector2D _beginPosition;
	FVector2D _emergedPosition;
	FVector2D _targetPosition;

	UTexture2D* _buttonDefaultTexture;
	UTexture2D* _buttonHoveredTexture;

	class UMapWidget* _map;

	bool _bIsMouseInArea = false;
	bool _bIsShown = false;
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent);

	void OnButtonClick(int32 index);

	UFUNCTION()
	void OnButtonClickEnemies();
	UFUNCTION()
	void OnButtonClickDrawings();
	UFUNCTION()
	void OnButtonClickSights();
	UFUNCTION()
	void OnButtonClickDoors();
	UFUNCTION()
	void OnButtonClickAmmo();
	UFUNCTION()
	void OnButtonClickObjectives();

public:

	FFilterChanged DelegateFilterChanged;

	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void InitData(class UMapWidget* map);

	void Show();

	void Hide();

	bool GetIsShown() { return _bIsShown; }

	TSharedPtr<SWidget> GetShowFocusWidget() const { return _btns[0]->TakeWidget(); }
};
