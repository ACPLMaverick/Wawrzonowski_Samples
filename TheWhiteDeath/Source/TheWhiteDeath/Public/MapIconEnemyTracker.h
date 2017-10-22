// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MapIconImage.h"
#include "Public/EnemyCharacter.h"
#include "MapIconEnemyTracker.generated.h"

/**
 * 
 */
UCLASS()
class THEWHITEDEATH_API UMapIconEnemyTracker : public UMapIconImage
{
	GENERATED_BODY()
	
protected:

	class AMapEnemyTrackerRecognitionZone* _tracker = NULL;
	class AEnemyCharacter* _enemy = NULL;

	TArray<UTexture2D*> _detectionStateTextures;
	TArray<class UImage*> _widgetsDetectionLevels;
	EPlayerDetectionState _prevPlayerDetection = EPlayerDetectionState::Unaware;
	class UCanvasPanel* _widgetRoot;
	class UImage* _widgetDefaultIcon;
	class UImage* _widgetFieldOfView;

	void ChangePlayerDetection();

	UFUNCTION()
	void OnEnemyRecognized(class AEnemyCharacter* enemyChar);
	UFUNCTION()
	void OnEnemyDestroyed(AActor* DestroyedActor);
	
public:
	
	UMapIconEnemyTracker(const FObjectInitializer& ObjectInitializer);

	const TArray<UTexture2D*>& GetDetectionStateTextures() { return _detectionStateTextures; }

	virtual bool Initialize() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void OnMapPlacementAction(const FGeometry& ExtGeometry, const FPointerEvent& ExtPointerEvent, class UMapWidget* map, const FVector& worldPosition) override;
	virtual void OnMapEraseAction(const FGeometry& ExtGeometry, const FPointerEvent& ExtPointerEvent) override;
};
