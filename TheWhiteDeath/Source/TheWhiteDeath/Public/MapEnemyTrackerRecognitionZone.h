// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Public/MapIconEnemyTracker.h"
#include "MapEnemyTrackerRecognitionZone.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRecognitionZoneEnemyFound, class AEnemyCharacter*, EnemyCharacter);

UCLASS()
class THEWHITEDEATH_API AMapEnemyTrackerRecognitionZone : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(Category = "RecognitionZone", EditAnywhere, BlueprintReadWrite)
	class USphereComponent* RecoSphere;

	UPROPERTY(Category = "RecognitionZone", EditAnywhere, BlueprintReadWrite)
	class UBillboardComponent* TrackerWorldIcon;

	TArray<UTexture2D*> _detectionStateTextures;
	class AEnemyCharacter* _enemy;
	class UMapIconEnemyTracker* _mapTracker;
	bool _firstTick = false;

	void EnableWorldTracker();

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UFUNCTION()
	void OnEnemyDestroyed(AActor* DestroyedActor);

public:	

	UPROPERTY(BlueprintAssignable, Category = "InteractionZone")
	FRecognitionZoneEnemyFound DelegateEnemyFound;

	// Sets default values for this actor's properties
	AMapEnemyTrackerRecognitionZone();

	void RegisterMapTracker(class UMapIconEnemyTracker* tracker);
	void UpdateDetectionState(EPlayerDetectionState detection);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
