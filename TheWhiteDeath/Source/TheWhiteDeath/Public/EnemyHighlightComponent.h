// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"
#include "EnemyHighlightComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class THEWHITEDEATH_API UEnemyHighlightComponent : public USceneComponent
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highlight")
	float FadeInSeconds = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highlight")
	float FadeOutSeconds = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highlight")
	float TimeSeconds = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highlight")
	float RadiusSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highlight")
	class UPostProcessComponent* PostProcess;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Highlight")
	class UMaterial* MHighlight;

	UPROPERTY(BlueprintReadOnly, Category = "Highlight")
	class UMaterialInstanceDynamic* MIDHighlight;

	float _timer;
	float _radius;
	float _fadeMultiplier;
	bool _bIsHighlighting;

public:
	
	// Sets default values for this component's properties
	UEnemyHighlightComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Highlight")
	void Highlight();

	UFUNCTION(BlueprintCallable, Category = "Highlight")
	void UnHighlight();
};
