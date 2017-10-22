// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "CoverMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class FPPCOVERSYSTEM_API UCoverMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:

#pragma region Protected

	/** Maximum speed that player can reach when sprinting. */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float MaxSprintWalkSpeed = 1200.0f;

	/** Acceleration that is applied when player is sprinting. Should be higher than default acceleration. */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float SprintAcceleration = 2048.0f;

	const float _crouchSpeedBase = 8.0f;
	float _oldUnscaledHalfHeight;
	float _oldUnscaledRadius;
	float _oldMaxAcceleration;
	float _oldMaxWalkSpeed;

	bool _bCrouchInProgress;
	bool _bUnCrouchInProgress;
	float _crouchSpeed;
	float _maxCharacterWalkSpeedCrouchedInterpolated = MaxWalkSpeed;

#pragma endregion
	
public:

#pragma region Functions Public

	UCoverMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	* Checks if new capsule size fits (no encroachment), and call CharacterOwner->OnStartCrouch() if successful.
	* In general you should set bWantsToCrouch instead to have the crouch persist during movement, or just use the crouch functions on the owning Character.
	* @param	bClientSimulation	true when called when bIsCrouched is replicated to non owned clients, to update collision cylinder and offset.
	*/
	virtual void Crouch(bool bClientSimulation = false) override;

	/**
	* Checks if default capsule size fits (no encroachment), and trigger OnEndCrouch() on the owner if successful.
	* @param	bClientSimulation	true when called when bIsCrouched is replicated to non owned clients, to update collision cylinder and offset.
	*/
	virtual void UnCrouch(bool bClientSimulation = false) override;

	/** Manually changes capsule half height, change it is bound by <CrouchHeight, NormalHeight>. */
	virtual void ManualCapsuleSizeAdd(float addition);
	virtual void ManualCapsuleSizeReset();
	virtual void ManualCapsuleSizeTotalReset();

	//BEGIN UMovementComponent Interface
	virtual float GetMaxSpeed() const override;
	//END UMovementComponent Interface

	//Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void InitializeComponent() override;
	//End UActorComponent Interface

	virtual void Sprint();
	virtual void UnSprint();

#pragma region Accessors

	bool GetCrouchInProgress() { return _bCrouchInProgress; }
	bool GetUnCrouchInProgress() { return _bUnCrouchInProgress; }

	float GetCrouchHalfHeightDifference() { return _oldUnscaledHalfHeight - CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(); }
	float GetCrouchHalfHeightMaxDifference() { return _oldUnscaledHalfHeight - CrouchedHalfHeight; }

#pragma endregion

#pragma endregion
	
};
