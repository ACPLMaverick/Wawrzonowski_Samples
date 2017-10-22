// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "MapIconMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEWHITEDEATH_API UMapIconMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	TArray<class UMapIconWidget*> _widgetsIcon;
	FVector2D _lastGamepadInput = FVector2D(0.0f, 0.0f);

	class APlayerCharacter* _player;
	class UCanvasPanel* _widgetRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IconMenu")
	float InternalRadius = 25.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IconMenu")
	float IconPlacementRadius = 65.0f;

	float _anglePerIcon = 0.0f;
	int _slotSelectedId = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IconMenu")
	bool bExternalMouseEvent = true;

	virtual void Show_Implementation();
	virtual void Hide_Implementation();

	void GpadWorldMarkHor(float val);
	void GpadWorldMarkVert(float val);

	virtual FReply InternalOnMouseMove(const FGeometry& ExtGeometry, const FPointerEvent& ExtMouseEvent, const class UMapWidget* caller);

	virtual FReply NativeOnMouseMove(const FGeometry& ExtGeometry, const FPointerEvent& ExtMouseEvent) override;

	virtual void SolveMouseHighlight(FVector2D mouseDir);

public:

	UMapIconMenuWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;
	virtual void InitData(class APlayerCharacter* player);
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FReply ExternalOnMouseMove(const FGeometry& ExtGeometry, const FPointerEvent& ExtMouseEvent, const class UMapWidget* caller);

	/// <summary>
	/// This function can return NULL ptr meaning no icon is currently selected. (i.e. player cursor / thumbstick is in center of the menu / neutral position)
	/// </summary>
	virtual class UMapIconImage* GetIconSelected();
	virtual class UMapIconImage* GetIconByID(int32 id);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MapIconMenuWidget")
		void Show();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MapIconMenuWidget")
		void Hide();

	bool GetIfExternalMouseEvent() const { return bExternalMouseEvent; }
	void SetIfExternalMouseEvent(bool val) { bExternalMouseEvent = val; }

	TSharedPtr<SWidget> GetShowFocusWidget() const { return Cast<UCanvasPanel>(GetRootWidget())->GetCanvasWidget(); }
};
