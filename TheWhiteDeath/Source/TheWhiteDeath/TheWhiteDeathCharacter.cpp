// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TheWhiteDeath.h"
#include "TheWhiteDeathCharacter.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "CoverMovementComponent.h"
#include "Public/Gun.h"
#include "Public/HitBox.h"
#include "Components/ChildActorComponent.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Engine.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ATheWhiteDeathCharacter

ATheWhiteDeathCharacter::ATheWhiteDeathCharacter(const class FObjectInitializer& PCIP) : 
	Super(PCIP)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(45.f, 96.0f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	RootComponent = GetCapsuleComponent();

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetupAttachment(RootComponent);
	Mesh1P->bCastDynamicShadow = true;
	Mesh1P->CastShadow = true;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	Gun = CreateDefaultSubobject<UChildActorComponent>(TEXT("Gun"));
	Gun->SetChildActorClass(AGun::StaticClass());
	Gun->SetupAttachment(Mesh1P);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	PrimaryActorTick.bCanEverTick = true;

	Equipment = CreateDefaultSubobject<UEquipment>(TEXT("Equipment"));

	_collisionChannelsToCheck.Add(ECollisionChannel::ECC_WorldStatic);
	_collisionChannelsToCheck.Add(ECollisionChannel::ECC_WorldDynamic);
	_collisionChannelsToCheck.Add(ECollisionChannel::ECC_Vehicle);
	_collisionChannelsToCheck.Add(ECollisionChannel::ECC_Pawn);
	_collisionChannelsToCheck.Add(ECollisionChannel::ECC_PhysicsBody);
	_collisionChannelsToCheck.Add(ECollisionChannel::ECC_Destructible);

	// creation of sense configs
	_configSight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("EnemyPerceptionConfig_Sight"));
	_configHearing = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("EnemyPerceptionConfig_Hearing"));
	_configDamage = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("EnemyPerceptionConfig_Damage"));

	_configSight->SightRadius = SightRadius;
	_configSight->LoseSightRadius = LoseSightRadius;
	_configSight->PeripheralVisionAngleDegrees = SightHalfAngleDegrees;
	_configSight->DetectionByAffiliation.bDetectNeutrals = true;
	_configSight->DetectionByAffiliation.bDetectEnemies = true;
	_configSight->DetectionByAffiliation.bDetectFriendlies = CanDetectDeadBodies;
	_configSight->AutoSuccessRangeFromLastSeenLocation = 500.0f;
	_configSight->SetMaxAge(StimuliMaxAge);

	_configHearing->HearingRange = HearingRadius;
	_configHearing->DetectionByAffiliation.bDetectNeutrals = true;
	_configHearing->DetectionByAffiliation.bDetectEnemies = true;
	_configHearing->DetectionByAffiliation.bDetectFriendlies = true;
	_configHearing->SetMaxAge(StimuliMaxAge);

	// register for senses
	UAIPerceptionSystem::RegisterPerceptionStimuliSource(this, _configDamage->GetSenseImplementation(), this);
	UAIPerceptionSystem::RegisterPerceptionStimuliSource(this, _configSight->GetSenseImplementation(), this);
	UAIPerceptionSystem::RegisterPerceptionStimuliSource(this, _configHearing->GetSenseImplementation(), this);
}

void ATheWhiteDeathCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (Equipment)
	{
		Equipment->RegisterComponent();
	}

	Gun->CreateChildActor();
	Cast<AGun>(Gun->GetChildActor())->PickUp(this);

	Equipment->PickUpGun((AGun*)Gun->GetChildActor());

	Cast<AGun>(Gun->GetChildActor())->IsPickedUp = true;

	// set stamina and health to max values
	Stamina = MaxStamina;
	Health = MaxHealth;


	//if (wHitMarker)
	//{
	//	// Create the widget and store it.
	//	MyHitMarker = CreateWidget<UUserWidget>(GetWorld(), wHitMarker);

	//	// now you can use the widget directly since you have a referance for it.
	//	// Extra check to  make sure the pointer holds the widget.
	//	if (MyHitMarker)
	//	{
	//		//let add it to the view port
	//		MyHitMarker->AddToViewport();
	//	}
	//}
}

void ATheWhiteDeathCharacter::Tick(float deltaTime)
{
	TickDeath(deltaTime);
	
	if (_bAlive)
	{
		TickRecoil(deltaTime);
		TickSprint(deltaTime);
		TickLocationWantsToHit(deltaTime);

		if (_bHasTakenDamageThisFrame)
			_bHasTakenDamageThisFrame = false;
	}
}

void ATheWhiteDeathCharacter::ApplyRecoil(float recoil)
{
	_bRecoilInProgress = true;
	_recoilTarget = recoil * _recoilMultiplier;
	_recoilCurrent = 0.0f;
}

FVector ATheWhiteDeathCharacter::GetLocationWantsToHit()
{
	return _locationWantsToHit;
}

void ATheWhiteDeathCharacter::OnFire()
{
	// player cannot shoot when sprinting.
	if (Gun != NULL && Gun->GetChildActor() != NULL && !_bSprintInProgress)
	{
		AGun* gun = Cast<AGun>(Gun->GetChildActor());
		if (gun->CanFire() && !gun->GetBoltActionWaitsForRepeat())
		{
			gun->Fire();

			UAnimMontage* fireAnim = (gun->GetMyFireMode() == EGunFireMode::Auto ? FireAutoAnimation : FireAnimation);

			if (fireAnim != NULL)
			{
				// Get the animation object for the arms mesh
				UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
				if (AnimInstance != NULL)
				{
					AnimInstance->Montage_Play(fireAnim, 1.f);
				}
			}
		}
		else if (gun->GetMyFireMode() == EGunFireMode::BoltAction && gun->GetBoltActionWaitsForRepeat())
		{
			gun->Fire();

			if (BoltActionRepeatAnimation != NULL)
			{
				// Get the animation object for the arms mesh
				UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
				if (AnimInstance != NULL)
				{
					AnimInstance->Montage_Play(BoltActionRepeatAnimation, 1.f);
				}
			}
		}
	}
}

void ATheWhiteDeathCharacter::OnReleaseFire()
{
	if (Gun != NULL && Gun->GetChildActor() != NULL && !_bSprintInProgress)
	{
		AGun* gun = Cast<AGun>(Gun->GetChildActor());
		gun->ReleaseFire();

		UAnimMontage* releaseAnim = (gun->GetMyFireMode() == EGunFireMode::Auto ? ReleaseFireAutoAnimation : ReleaseFireAnimation);

		if (releaseAnim != NULL)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(releaseAnim, 1.f);
			}
		}
	}
}

void ATheWhiteDeathCharacter::OnReload()
{
	if (Gun != NULL && Gun->GetChildActor() != NULL)
	{
		AGun* gun = Cast<AGun>(Gun->GetChildActor());

		if (!gun->GetReloadInProgress() && gun->GetBulletCount() < gun->GetMagazineCapacity())
		{
			gun->Reload(Equipment->SendAmmo());
			if (GEngine != NULL)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::FromInt(Equipment->GetCurrentWeaponAmmoCount())); //((AGun*)Gun->GetChildActor())->GetMyAmmoType()
			}

			if (gun->GetMyFireMode() == EGunFireMode::BoltAction)
			{
				if (BoltActionReloadAnimation != NULL)
				{
					UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
					if (AnimInstance != NULL)
					{
						AnimInstance->Montage_Play(BoltActionReloadAnimation, 1.f);
					}
				}
			}
			else if (gun->GetMyFireMode() == EGunFireMode::SemiAuto)
			{
				//TODO: Implement
			}
			else
			{
				//TODO: Implement
			}
		}
	}
}

void ATheWhiteDeathCharacter::OnBeginThrowGrenade()
{
	return;
}

void ATheWhiteDeathCharacter::OnReleaseGrenade()
{
	if (Grenade != NULL)
	{
		Grenade->OnRelease();
	}
}

void ATheWhiteDeathCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);

		UPawnMovementComponent* pmc = GetMovementComponent();
		// add loudness peak
		if (!((UCoverMovementComponent*)GetMovementComponent())->IsCrouching())
		{
			UAIPerceptionSystem::MakeNoiseImpl(this, StepLoudness, this, GetActorLocation(), StepLoudnessMaxRadius, TEXT("Footsteps"));
		}
	}
}

void ATheWhiteDeathCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);

		// add loudness peak
		if (!((UCoverMovementComponent*)GetMovementComponent())->IsCrouching())
		{
			UAIPerceptionSystem::MakeNoiseImpl(this, StepLoudness, this, GetActorLocation(), StepLoudnessMaxRadius, TEXT("Footsteps"));
		}
	}
}

void ATheWhiteDeathCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATheWhiteDeathCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ATheWhiteDeathCharacter::CanCrouch()
{
	return true;
}

bool ATheWhiteDeathCharacter::CanSprint()
{
	return true;
}

void ATheWhiteDeathCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	/*
	RecalculateBaseEyeHeight();
	if (!_bCrouchInProgress)
	{
		_crouchInterp = 0.0f;
		_bCrouchInProgress = true;
	}
	_crouchBaseZ = BaseTranslationOffset.Z;
	_crouchedZ = BaseTranslationOffset.Z + HalfHeightAdjust;
	_crouchSpeed = _crouchSpeedBase;
	*/
}

void ATheWhiteDeathCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	/*
	RecalculateBaseEyeHeight();
	if (!_bCrouchInProgress)
	{
		_crouchInterp = 0.0f;
		_bCrouchInProgress = true;
	}
	_crouchBaseZ = BaseTranslationOffset.Z;
	_crouchedZ = BaseTranslationOffset.Z + HalfHeightAdjust;
	_crouchSpeed = -_crouchSpeedBase;
	*/
}

void ATheWhiteDeathCharacter::OnHit(float DmgAmount = 15.0f, FVector Direction = FVector(0, 0, 1))
{
	// does nothing as hitboxes are responsible for damage taking.
	Health -= DmgAmount;
	_bHasTakenDamageThisFrame = true;
}

void ATheWhiteDeathCharacter::StartCrouch()
{
	if (CanCrouch())
	{
		ACharacter::Crouch();
	}
}

void ATheWhiteDeathCharacter::EndCrouch()
{
	//if (CanCrouch())
	//{
		ACharacter::UnCrouch();
	//}
}

void ATheWhiteDeathCharacter::StartSprint()
{
	if (CanSprint())
	{
		_bSprintInProgress = true;
		((UCoverMovementComponent*)GetMovementComponent())->Sprint();
	}
}

void ATheWhiteDeathCharacter::EndSprint()
{
	if (CanSprint())
	{
		_bSprintInProgress = false;
		((UCoverMovementComponent*)GetMovementComponent())->UnSprint();
	}
}

void ATheWhiteDeathCharacter::TickDeath(float dt)
{
	if (Health <= 0.0f && _bAlive)
	{
		Die();
		_deathPosition = GetActorLocation();
		_deathPosition.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 27.0f;
	}
	else if(!_bAlive)
	{
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), _deathPosition, dt, 1.5f));
	}
}

void ATheWhiteDeathCharacter::TickRecoil(float dt)
{
	if (_bRecoilInProgress)
	{
		_recoilCurrent = FMath::FInterpTo(_recoilCurrent, _recoilTarget, dt, _recoilSpeed);

		if (_recoilTarget - _recoilCurrent < 0.01f)
		{
			_recoilCurrent = _recoilTarget;
			_bRecoilInProgress = false;
		}

		AddControllerPitchInput(-_recoilCurrent);
	}
}

void ATheWhiteDeathCharacter::TickCrouch(float dt)
{
	/*BaseTranslationOffset.Z = FMath::FInterpTo(BaseTranslationOffset.Z, _crouchedZ, GWorld->GetDeltaSeconds(), _crouchSpeed);
	
	if ((_crouchSpeed <= 0.0f && BaseTranslationOffset.Z <= _crouchedZ) ||
		(_crouchSpeed > 0.0f && BaseTranslationOffset.Z >= _crouchedZ))
	{
		_bCrouchInProgress = false;
	}*/
}

void ATheWhiteDeathCharacter::TickSprint(float dt)
{
	if (_bSprintInProgress)
	{
		// stamina goes down while we sprint.

		Stamina -= _staminaDownPerSecond * dt;

		if (Stamina < 0.0f)
		{
			Stamina = 0.0f;
			EndSprint();
		}
	}
	else if (Stamina < MaxStamina)
	{
		// we don't sprint, so we regenerate stamina
		if (((UCoverMovementComponent*)GetMovementComponent())->Velocity.SizeSquared() <= 0.00001f)
		{
			Stamina = FMath::Min(Stamina + _staminaUpPerSecondStand * dt, MaxStamina);
		}
		else
		{
			Stamina = FMath::Min(Stamina + _staminaUpPerSecondWalk * dt, MaxStamina);
		}
	}
}

void ATheWhiteDeathCharacter::TickLocationWantsToHit(float dt)
{
	if (UseTickLocationWantsToHit)
	{
		FVector origin, direction, location;
		FHitResult result;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		GetLocationWantsToHitData(origin, direction);

		for (int i = 0; i < _collisionChannelsToCheck.Num(); ++i)
		{
			if (GWorld->LineTraceSingleByChannel(result, origin, origin + direction * 10000.0f, _collisionChannelsToCheck[i], params))
			{
				_locationWantsToHit = result.Location;
				return;
			}
		}

		_locationWantsToHit = origin + direction * 100.0f;
	}
}

void ATheWhiteDeathCharacter::Die()
{
	_bAlive = false;

	if (DieAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(DieAnimation, 1.f);
		}
	}

	if (SoundDeath != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SoundDeath, GetActorLocation());
	}
}

void ATheWhiteDeathCharacter::GetLocationWantsToHitData(FVector & outOrigin, FVector & outDirection)
{
	FRotator camRot;
	GetActorEyesViewPoint(outOrigin, camRot);

	outOrigin = GetActorLocation();
	outDirection = camRot.Vector();
}
