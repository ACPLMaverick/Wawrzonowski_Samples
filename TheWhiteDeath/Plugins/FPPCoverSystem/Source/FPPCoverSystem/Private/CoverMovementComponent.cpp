// Fill out your copyright notice in the Description page of Project Settings.

#include "UFPPCoverSystemPrivatePCH.h"
#include "CoverMovementComponent.h"

namespace CharacterMovementComponentStatics
{
	static const FName CrouchTraceName = FName(TEXT("CrouchTrace"));
	static const FName FindWaterLineName = FName(TEXT("FindWaterLine"));
	static const FName FallingTraceParamsTag = FName(TEXT("PhysFalling"));
	static const FName CheckLedgeDirectionName = FName(TEXT("CheckLedgeDirection"));
	static const FName ProjectLocationName = FName(TEXT("NavProjectLocation"));
	static const FName CheckWaterJumpName = FName(TEXT("CheckWaterJump"));
	static const FName ComputeFloorDistName = FName(TEXT("ComputeFloorDistSweep"));
	static const FName FloorLineTraceName = FName(TEXT("ComputeFloorDistLineTrace"));
	static const FName ImmersionDepthName = FName(TEXT("MovementComp_Character_ImmersionDepth"));
}

UCoverMovementComponent::UCoverMovementComponent(const FObjectInitializer & ObjectInitializer) :
	Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
}

void UCoverMovementComponent::Crouch(bool bClientSimulation)
{
	_bCrouchInProgress = true;
	_bUnCrouchInProgress = false;
	CharacterOwner->bIsCrouched = true;
	_crouchSpeed = _crouchSpeedBase;
}


void UCoverMovementComponent::UnCrouch(bool bClientSimulation)
{
	_bUnCrouchInProgress = true;
	_bCrouchInProgress = false;
	bWantsToCrouch = false;
	CharacterOwner->bIsCrouched = false;
	_crouchSpeed = _crouchSpeedBase;
}

void UCoverMovementComponent::ManualCapsuleSizeAdd(float addition)
{
	if (!_bCrouchInProgress && !_bUnCrouchInProgress)
	{
		float NewRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
		float NewHalfHeight = FMath::Clamp(CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() + addition, CrouchedHalfHeight, _oldUnscaledHalfHeight);
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(NewRadius, NewHalfHeight);
	}
}

void UCoverMovementComponent::ManualCapsuleSizeReset()
{
	if (_bCrouchInProgress || _bUnCrouchInProgress || IsCrouching())
	{
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(_oldUnscaledRadius, CrouchedHalfHeight);
	}
	else
	{
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(_oldUnscaledRadius, _oldUnscaledHalfHeight);
	}
}

void UCoverMovementComponent::ManualCapsuleSizeTotalReset()
{
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(_oldUnscaledRadius, _oldUnscaledHalfHeight);
	_bUnCrouchInProgress = false;
	_bCrouchInProgress = false;
	bWantsToCrouch = false;
	CharacterOwner->bIsCrouched = false;
}

float UCoverMovementComponent::GetMaxSpeed() const
{
	switch (MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
		return (_bCrouchInProgress || _bUnCrouchInProgress || IsCrouching()) ? _maxCharacterWalkSpeedCrouchedInterpolated : MaxWalkSpeed;
	case MOVE_Falling:
		return MaxWalkSpeed;
	case MOVE_Swimming:
		return MaxSwimSpeed;
	case MOVE_Flying:
		return MaxFlySpeed;
	case MOVE_Custom:
		return MaxCustomMovementSpeed;
	case MOVE_None:
	default:
		return 0.f;
	}
}

void UCoverMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	if (_bCrouchInProgress)
	{
		float NewRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
		float NewHalfHeight = FMath::FInterpTo(CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), CrouchedHalfHeight, DeltaTime, _crouchSpeed);
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(NewRadius, NewHalfHeight);

		_maxCharacterWalkSpeedCrouchedInterpolated = FMath::FInterpTo(_maxCharacterWalkSpeedCrouchedInterpolated, MaxWalkSpeedCrouched, DeltaTime, _crouchSpeed);

		if (NewHalfHeight <= (CrouchedHalfHeight + 0.5f))
		{
			_maxCharacterWalkSpeedCrouchedInterpolated = MaxWalkSpeedCrouched;
			CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(NewRadius, CrouchedHalfHeight);
			
			_bCrouchInProgress = false;
		}
	}
	else if (_bUnCrouchInProgress)
	{
		float NewRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
		float NewHalfHeight = FMath::FInterpTo(CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), _oldUnscaledHalfHeight, DeltaTime, _crouchSpeed);
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(NewRadius, NewHalfHeight);

		_maxCharacterWalkSpeedCrouchedInterpolated = FMath::FInterpTo(_maxCharacterWalkSpeedCrouchedInterpolated, MaxWalkSpeed, DeltaTime, _crouchSpeed);

		if (NewHalfHeight >= _oldUnscaledHalfHeight - 0.5f)
		{
			_maxCharacterWalkSpeedCrouchedInterpolated = MaxWalkSpeed;
			CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(NewRadius, _oldUnscaledHalfHeight);
			_bUnCrouchInProgress = false;
		}
	}

	//UE_LOG(LogTemp, Log, TEXT("%f %f %f"), Velocity.X, Velocity.Y, Velocity.Z);

	UCharacterMovementComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCoverMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	_oldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	_oldUnscaledRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	_oldMaxAcceleration = MaxAcceleration;
	_oldMaxWalkSpeed = MaxWalkSpeed;
}

void UCoverMovementComponent::Sprint()
{
	if (!_bCrouchInProgress && !_bUnCrouchInProgress && !IsCrouching())
	{
		MaxAcceleration = SprintAcceleration;
		MaxWalkSpeed = MaxSprintWalkSpeed;
	}
}

void UCoverMovementComponent::UnSprint()
{
	MaxAcceleration = _oldMaxAcceleration;
	MaxWalkSpeed = _oldMaxWalkSpeed;
}
