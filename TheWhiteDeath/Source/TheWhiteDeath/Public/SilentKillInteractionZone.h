// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InteractionZone.h"
#include "SilentKillInteractionZone.generated.h"

/**
 * 
 */
UCLASS()
class THEWHITEDEATH_API USilentKillInteractionZone : public UInteractionZone
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(Category = "SilentKillInteractionZone", EditAnywhere, BlueprintReadWrite)
	class USceneComponent* KillPosition;

	/** How much will be the silent kill damage (100% of character's Health) reduced. For one-hit kills this should be zero. One means this character is immune to silent kill. */
	UPROPERTY(Category = "SilentKillInteractionZone", EditAnywhere, BlueprintReadWrite)
	float DamageProtection = 0.0f;

	/** Angle of tolerance when it is determined if killer is "behind" this character and can silent kill. */
	UPROPERTY(Category = "SilentKillInteractionZone", EditAnywhere, BlueprintReadWrite)
	float ActionHalfAngle = 30.0f;

	virtual void Interaction_Implementation(APlayerCharacter* pc) override;
	
public:

	USilentKillInteractionZone(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	virtual bool CanExecuteInteraction(APlayerCharacter * pc) override;

	float GetTotalDamageTakes(float inHealth) { return inHealth * (FMath::Max(1.0f - DamageProtection, 0.0f)); }

	FVector GetKillWorldLocation() { return KillPosition->GetComponentLocation(); }
	
};
