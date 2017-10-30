// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UFPPCoverSystemPrivatePCH.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Classes/Kismet/GameplayStatics.h"
#include "CoverMovementComponent.h"
#include "Cover.h"
#include "CoverCharacter.h"


ACoverCharacter::ACoverCharacter( const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCoverMovementComponent>(ACharacter::CharacterMovementComponentName)),
	_coverSpeedMultipliers(1.5f, 4.0f),
	_bBeginCover(false),
	_bEndCover(false),
	_bCanLeaveCoverThisFrame(false)
{
}


void ACoverCharacter::ApplyCoverRotationLimit()
{
	if (_currentCover != NULL && FirstPersonCameraComponent != NULL && _myInputComponent != NULL)
	{
		if (_currentCover->GetCoverEmergeDirection().Y != 0.0f)
		{
			FRotator rotation(GetController()->GetControlRotation());
			float clampYaw = _currentCover->GetCoverForward(GetActorLocation()).Rotation().Yaw;

			// check whether player is closer to left or right boundary of the cover
			FVector left = _currentCover->GetBoundLeftWorld();
			FVector right = _currentCover->GetBoundRightRearWorld();
			FVector loc = GetActorLocation();

			if (FVector::DistSquared(left, loc) < FVector::DistSquared(right, loc))
			{
				rotation.Yaw = FMath::Min(rotation.Yaw, clampYaw);
			}
			else
			{
				rotation.Yaw = FMath::Max(rotation.Yaw, clampYaw);
			}

			GetController()->SetControlRotation(rotation);
		}
	}
}


void ACoverCharacter::BeginCover(float dt)
{
	if (FirstPersonCameraComponent != NULL && _myInputComponent != NULL)
	{
		//UE_LOG(LogTemp, Log, TEXT("CoverBegin"));

		FVector dir = _currentCover->GetCoverEmergeDirection();

		float arbitraryAngleUp = -8.0f;
		float arbitraryAngleSide = -15.0f;
		float angleUp = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FirstPersonCameraComponent->GetUpVector(), _currentCover->GetActorUpVector()))) * 0.5f;

		if (FirstPersonCameraComponent->GetForwardVector().Z < _currentCover->GetCoverForward(GetActorLocation()).Z)
		{
			angleUp = -angleUp;
		}

		float angleSide = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FirstPersonCameraComponent->GetForwardVector(), _currentCover->GetCoverForward(GetActorLocation())))) * 0.5f;
		if (FirstPersonCameraComponent->GetRightVector().X < _currentCover->GetCoverRight(GetActorLocation()).X)
		{
			angleSide = -angleSide;
			arbitraryAngleSide = -arbitraryAngleSide;
		}

		_coverBeginPitchYawTarget = FVector2D(GetControlRotation().Pitch - dir.Z * (arbitraryAngleUp + angleUp), GetControlRotation().Yaw + dir.Y * (arbitraryAngleSide + angleSide));
		_bCoverBeginRotationLerp = true;

		if (SoundTakeCover != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, SoundTakeCover, GetActorLocation());
		}
	}
}

void ACoverCharacter::EndCover(float dt)
{
	//UE_LOG(LogTemp, Log, TEXT("CoverEnd"));
}

void ACoverCharacter::TickCover(float dt)
{
	if (FirstPersonCameraComponent != NULL && _myInputComponent != NULL)
	{
		if (_currentCover != NULL && !_bVaultCover)
		{
			UCoverMovementComponent* movement = Cast<UCoverMovementComponent>(GetMovementComponent());
			if (_bCoverBeginRotationLerp)
			{
				if (FMath::Abs(GetControlRotation().Pitch - _coverBeginPitchYawTarget.X) < 0.1f && FMath::Abs(GetControlRotation().Yaw - _coverBeginPitchYawTarget.Y) < 0.1f)
				{
					GetController()->SetControlRotation(FRotator(_coverBeginPitchYawTarget.X, _coverBeginPitchYawTarget.Y, GetControlRotation().Roll));
					_bCoverBeginRotationLerp = false;
				}
				else
				{
					const float speed = 25.0f;
					GetController()->SetControlRotation(FRotator(
						FMath::FInterpTo(GetControlRotation().Pitch, _coverBeginPitchYawTarget.X, dt, speed),
						FMath::FInterpTo(GetControlRotation().Yaw, _coverBeginPitchYawTarget.Y, dt, speed),
						GetControlRotation().Roll));
				}
			}
			else if ((_currentCover->GetIfNeedToCrouch() && movement->IsCrouching()) ||
				!_currentCover->GetIfNeedToCrouch())
			{
				if (!_bBeginCover)
				{
					BeginCover(dt);
					_bBeginCover = true;
				}

				FVector emerge(0.0f);
				bool ignoreBoundaryLimit = false;
				emerge.Z = _myInputComponent->GetAxisValue(TEXT("LookUp"));
				if (emerge.Z == 0.0f)
				{
					emerge.Z = _myInputComponent->GetAxisValue(TEXT("LookUpRate")) * BaseLookUpRate * dt;
				}
				emerge.Y = -_myInputComponent->GetAxisValue(TEXT("Turn"));
				if (emerge.Y == 0.0f)
				{
					emerge.Y = -_myInputComponent->GetAxisValue(TEXT("TurnRate")) * BaseTurnRate * dt;
				}

				// do not emerge on Z if camera's already pointing down and we're fully crouched
				if (FirstPersonCameraComponent->GetForwardVector().Z < 0.0f && GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() <= movement->CrouchedHalfHeight)
				{
					emerge.Z = FMath::Min(emerge.Z, 0.0f);
				}

				// do not emerge on X if we reach the border angle
				float clampYaw = _currentCover->GetCoverForward(GetActorLocation()).Rotation().Yaw;
				float playerYaw = FirstPersonCameraComponent->GetForwardVector().Rotation().Yaw;

				// check whether player is closer to left or right boundary of the cover
				FVector left = _currentCover->GetBoundLeftWorld();
				FVector right = _currentCover->GetBoundRightRearWorld();
				FVector loc = GetActorLocation();
				float lds = FVector::DistSquared(left, loc);
				float rds = FVector::DistSquared(right, loc);

				if ((lds < rds && playerYaw > clampYaw) ||
					(lds > rds && playerYaw < clampYaw))
				{
					emerge.Y = 0.0f;
				}

				// multiply emerge vector by cover's direction vector component-wise - left out will be only one axis
				emerge *= _currentCover->GetCoverEmergeDirection();
				emerge.Y *= _coverSpeedMultipliers.X;
				emerge.Z *= _coverSpeedMultipliers.Y;

				// scale Z-emerge by direction facing only if we EMERGE, not hide
				if (emerge.Z > 0.0f)
				{
					FVector camForwardPlain(FirstPersonCameraComponent->GetForwardVector());
					camForwardPlain.Z = 0.0f;
					camForwardPlain.Normalize();
					FVector covForwardPlain(_currentCover->GetCoverForward(GetActorLocation()));
					covForwardPlain.Z = 0.0f;
					covForwardPlain.Normalize();
					float zCoeff = FMath::Max(FVector::DotProduct(camForwardPlain, covForwardPlain), 0.0f);
					zCoeff *= zCoeff;
					emerge.Z *= zCoeff;
				}

				// scale player's capsule by emerge vector 
				movement->ManualCapsuleSizeAdd(emerge.Z);
				AddMovementInput(_currentCover->GetCoverRight(GetActorLocation()), emerge.Y);

				UE_LOG(LogTemp, Log, TEXT("CoverTick: %f %f"), emerge.Y, emerge.Z);
			}
		}
		else if (_bBeginCover)
		{
			if (!_bEndCover)
			{
				EndCover(dt);
				_bEndCover = true;
			}
		}

		if (_bCanLeaveCoverThisFrame)
		{
			_bCanLeaveCoverThisFrame = false;
		}

	}
}

void ACoverCharacter::TickVaultCover(float dt)
{
	if (_bVaultCover &&
		!((UCoverMovementComponent*)GetMovementComponent())->GetCrouchInProgress() &&
		!((UCoverMovementComponent*)GetMovementComponent())->GetUnCrouchInProgress())
	{
		if (GetActorEnableCollision())
		{
			SetActorEnableCollision(false);
		}

		SetActorLocation(FMath::VInterpTo(GetActorLocation(), _vaultCoverDestination, dt, _coverVaultSpeed), false, nullptr, ETeleportType::TeleportPhysics);
		GetController()->SetControlRotation(FMath::RInterpTo(GetController()->GetControlRotation(), _vaultCoverDestRotation, dt, _coverVaultSpeed));

		if ((GetActorLocation() - _vaultCoverDestination).SizeSquared() < 10.0f)
		{
			SetActorLocation(_vaultCoverDestination, false, nullptr, ETeleportType::TeleportPhysics);
			GetController()->SetControlRotation(_vaultCoverDestRotation);
			EndVaultCover();
		}
	}
}

void ACoverCharacter::BeginVaultCover()
{
	UnCrouch();
	_bVaultCover = true;
	_vaultCoverDestination = _currentCover->GetReflectedPositionAcrossCover(GetActorLocation(), GetActorForwardVector());
	_vaultCoverDestRotation = (_vaultCoverDestination - GetActorLocation()).Rotation();

	UCoverMovementComponent* movement = Cast<UCoverMovementComponent>(GetMovementComponent());
	if (movement->IsCrouching())
	{
		_vaultCoverDestination.Z += movement->GetCrouchHalfHeightDifference();
	}

	if (VaultCoverAnimation != NULL && GetCharacterMesh() != NULL)
	{
		UAnimInstance* AnimInstance = GetCharacterMesh()->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(VaultCoverAnimation, 1.f);
		}
	}
}

void ACoverCharacter::EndVaultCover()
{
	_bVaultCover = false;
	SetActorEnableCollision(true);
}

void ACoverCharacter::OnEnterCoverArea(ACover * cover)
{
	_currentCover = cover;
}

void ACoverCharacter::OnExitCoverArea()
{
	_currentCover = NULL;

	UCoverMovementComponent* movement = Cast<UCoverMovementComponent>(GetMovementComponent());
	_bBeginCover = false;
	_bEndCover = false;
	movement->ManualCapsuleSizeReset();
}

bool ACoverCharacter::CanVaultCover()
{
	if (!_currentCover->GetCanVaultOver())
	{
		return false;
	}

	FVector playerForward = GetActorForwardVector();
	FVector coverForward = _currentCover->GetCoverForward(GetActorLocation());
	playerForward.Z = coverForward.Z = 0.0f;
	playerForward.Normalize();
	coverForward.Normalize();

	return (FVector::DotProduct(playerForward, coverForward) > 0.1f);
}

bool ACoverCharacter::IsCurrentlyInCover()
{
	if(_currentCover != NULL)
	{
		if (_currentCover->GetIfNeedToCrouch())
		{
			return Cast<UCoverMovementComponent>(GetMovementComponent())->IsCrouching();
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}
