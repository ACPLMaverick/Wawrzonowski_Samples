// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GameController.generated.h"

UCLASS()
class THEWHITEDEATH_API AGameController : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(Category = "GameController|Map", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* MapTopLeftCorner;

	UPROPERTY(Category = "GameController|Map", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* MapBottomRightCorner;

	UPROPERTY(Category = "GameController|Wind", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float WindMinForce;

	UPROPERTY(Category = "GameController|Wind", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float WindMaxForce;

	UPROPERTY(Category = "GameController|Wind", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float WindChangeSpeed;

	UPROPERTY(Category = "GameController|Wind", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float WindMinChangeDelay;

	UPROPERTY(Category = "GameController|Wind", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float WindMaxChangeDelay;

	UPROPERTY(Category = "GameController|Wind", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float WindVariationMultiplier;

	UPROPERTY(Category = "GameController|Wind", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float WindVariationMinChangeDelay;

	UPROPERTY(Category = "GameController|Wind", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float WindVariationMaxChangeDelay;

	static AGameController* _lastInstance;

	FVector _wind;
	FVector _newWind;
	FVector _windVariation;
	FVector2D _variationChangeMultipliers;
	FVector2D _newVariationChangeMultipliers;
	float _windTimer;
	float _windCurrentDelay;
	float _windVariationTimer;
	float _windVariationCurrentDelay;

	void TickWind(float deltaTime);

	FVector GenerateWindVector();
	
public:	
	// Sets default values for this actor's properties
	AGameController();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category="GameController")
	FVector GetWindVector() { return _wind + _windVariation; }
	UFUNCTION(BlueprintCallable, Category = "GameController")
	FVector GetWorldTopLeftCorner() { return MapTopLeftCorner->GetComponentLocation(); }
	UFUNCTION(BlueprintCallable, Category = "GameController")
	FVector GetWorldBottomRightCorner() { return MapBottomRightCorner->GetComponentLocation(); }

	UFUNCTION(BlueprintCallable, Category = "GameController")
	static AGameController* GetInstance() { return _lastInstance; }
};
