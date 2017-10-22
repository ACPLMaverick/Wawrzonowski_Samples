// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SnowArea.generated.h"

UCLASS()
class THEWHITEDEATH_API ASnowArea : public AActor
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SnowArea")
	class UBoxComponent* Extents;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SnowArea")
	class UParticleSystemComponent* SnowFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SnowArea")
	class APlayerCharacter* ReferenceCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SnowArea")
	float SnowViewDistance = 10000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SnowArea")
	float SnowHeight = 300.0f;


	TArray<UParticleSystemComponent*> _snowParticleSystems;
	FVector2D _snowBounds;

public:	
	// Sets default values for this actor's properties
	ASnowArea(const class FObjectInitializer& PCIP);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
