// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "CoverCharacter.generated.h"
 

/**
 * Example of declaring a UObject in a plugin module
 */
UCLASS()
class FPPCOVERSYSTEM_API ACoverCharacter : public ACharacter
{
	GENERATED_BODY()

protected:

	/** AnimMontage to play each time we reload bolt action rifle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* VaultCoverAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* SoundTakeCover;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	class UInputComponent* _myInputComponent;

	FRotator _vaultCoverDestRotation;
	FVector _vaultCoverDestination;
	FVector2D _coverSpeedMultipliers;
	FVector2D _coverBeginPitchYawTarget;

	class ACover* _currentCover;

	const float _coverVaultSpeed = 5.0f;

	bool _bBeginCover;
	bool _bEndCover;
	bool _bCoverBeginRotationLerp;
	bool _bCanLeaveCoverThisFrame;
	bool _bVaultCover;

	virtual void ApplyCoverRotationLimit();
	virtual void BeginCover(float dt);
	virtual void EndCover(float dt);
	virtual void TickCover(float dt);
	virtual void TickVaultCover(float dt);
	virtual void BeginVaultCover();
	virtual void EndVaultCover();

	virtual class USkeletalMeshComponent* GetCharacterMesh() { return NULL; }

public:

	ACoverCharacter(const class FObjectInitializer& PCIP);

	virtual void OnEnterCoverArea(class ACover* cover);
	virtual void OnExitCoverArea();

	UFUNCTION(BlueprintCallable, Category = "Cover Character")
	virtual bool CanVaultCover();

	UFUNCTION(BlueprintCallable, Category = "Cover Character")
	virtual bool IsCurrentlyInCover();
};


