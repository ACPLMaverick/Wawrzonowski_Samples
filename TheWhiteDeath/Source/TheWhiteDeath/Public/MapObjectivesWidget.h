// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "MapObjectivesWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEWHITEDEATH_API UMapObjectivesWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	TMap<class UObjectiveComponent*, UHorizontalBox*> _mapComponentToWidget;
	TMap<class UObjectiveComponent*, class UMapIconImage*> _mapComponentToMapImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectivesWidget")
	FLinearColor ImageColorCompleted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectivesWidget")
	FLinearColor TextColorCompleted;

	UVerticalBox* _widgetVerticalBox;
	UHorizontalBox* _widgetTemplatePrimary;
	UHorizontalBox* _widgetTemplateSecondary;

	class UMapWidget* _map;

	class UMapIconImage* CreateMapIcon(const class UObjectiveComponent* obj, const UImage* refImage);

	UFUNCTION()
	void OnObjectiveCompleted(class UObjectiveComponent* obj);
	UFUNCTION()
	void OnObjectiveMarkedVisible(class UObjectiveComponent* obj);
	UFUNCTION()
	void OnObjectiveMarkedOnMap(class UObjectiveComponent* obj);
	UFUNCTION()
	void OnObjectiveMoved(class UObjectiveComponent* obj, FVector location);

public:

	UMapObjectivesWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	void InitData(class UMapWidget* map, const TArray<class UObjectiveComponent*>& objectives);
};
