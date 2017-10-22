// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "TheWhiteDeathProjectile.h"
#include "TheWhiteDeathCharacter.h"
#include "PlayerCharacter.h"
#include "../Public/Gun.h"
#include "Animation/AnimInstance.h"
#include "ParticleDefinitions.h"
#include "Perception/AIPerceptionSystem.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a gun mesh component
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	//Mesh->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	Mesh->bCastDynamicShadow = true;
	Mesh->CastShadow = true;
	RootComponent = Mesh;

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(Mesh);
	//MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	AimCameraLocation = CreateDefaultSubobject<USceneComponent>(TEXT("AimCameraLocation"));
	AimCameraLocation->SetupAttachment(Mesh);

	ScopeAimCameraLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ScopeAimCameraLocation"));
	ScopeAimCameraLocation->SetupAttachment(Mesh);

	FireParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireParticle"));
	FireParticle->SetupAttachment(Mesh);
	FireParticle->Deactivate();
	FireParticle->bAutoActivate = false;
	FireParticle->bAutoDestroy = false;

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	InteractionZone = CreateDefaultSubobject<UInteractionZone>(TEXT("InteractionZone"));
	InteractionZone->SetupAttachment(Mesh);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTrigger"));
	BoxComponent->SetupAttachment(Mesh);

	//DefaultCollisionChannel = ECollisionChannel::ECC_Pawn;
	BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	_secondsPerFire = 1.0f / (MaxFiresPerMinute / 60.0f);

	_currentDispersion = BulletDispersionStationary;

	// spawn weapon with full mag
	BulletsInMagazine = MagazineCapacity;

	if (InteractionZone)
	{
		InteractionZone->RegisterComponent();
	}

	if (!IsPickedUp)
	{
		//BoxComponent->OnComponentBeginOverlap.Clear();
		//BoxComponent->OnComponentEndOverlap.Clear();
		//BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGun::RegisterOnBeginOverlap);
		//BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AGun::UnregisterOnEndOverlap);
	}
}

// Called every frame
void AGun::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	TickDispersion(DeltaTime);
	TickShoot(DeltaTime);
	TickReload(DeltaTime);
}

void AGun::Fire()
{
	_bTriggerPulled = true;
}

void AGun::ReleaseFire()
{
	_bTriggerPulled = false;

	if (MyFireMode == EGunFireMode::BoltAction &&  _bSemiFired && BulletsInMagazine > 0)
	{
		_bBoltNeedRepeat = true;
	}

	_bSemiFired = false;
}

void AGun::Reload(int bulletsCount)
{
	if (!_bReloadInProgress)
	{
		_bReloadInProgress = true;
		_bulletsToReload = bulletsCount;
		_reloadTimer = ReloadTimeSeconds;

		// pick different animation, in regard of ammunition count
		UAnimMontage* reloadAnimation = ReloadAnimation;
		if (BulletsInMagazine == 0)
		{
			reloadAnimation = ReloadAnimationEmptyChamber;
		}

		// try and play a firing animation if specified
		if (reloadAnimation != NULL)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(reloadAnimation, 1.f);
			}
		}
	}
}

bool AGun::CanFire()
{
	return (!_bSemiFired && !_bBoltNeedRepeat && !_bReloadInProgress && _fireTimer <= 0.0f);
}

void AGun::PickUp(ATheWhiteDeathCharacter * character)
{
	_owner = character;

	//BoxComponent->OnComponentBeginOverlap.Clear();
	//BoxComponent->OnComponentEndOverlap.Clear();

	InteractionZone->SetCapsuleSize(0.0f, 0.0f, false);

	//BoxComponent->SetBoxExtent(FVector(0.0f, 0.0f, 0.0f), false);
}

void AGun::Discard()
{
	_owner = NULL;
}

void AGun::TickDispersion(float DeltaTime)
{
	if (_dispersionTimer > 0.0f)
	{
		_dispersionTimer = FMath::Max(_dispersionTimer - DeltaTime, 0.0f);
	}
	
	_currentDispersion = FMath::Lerp<FVector>(BulletDispersionStationary, BulletDispersionBurst, _dispersionTimer / BulletDispersionOverTime);

	if (GetParentActor() != NULL)
	{
		ATheWhiteDeathCharacter* character = (ATheWhiteDeathCharacter*)GetParentActor();
		UCharacterMovementComponent* movComponent = (UCharacterMovementComponent*)character->GetMovementComponent();

		if (movComponent != NULL)
		{
			_currentDispersion *= (1.0f + BulletDispersionCharacterVelocityInfluence * _dispersionVelocityInfluenceMplier * movComponent->Velocity.SizeSquared());
		}
	}
}

void AGun::TickShoot(float DeltaTime)
{
	if (_fireTimer > 0.0f)	// keep fire timer running even when we don't hold the trigger
	{
		_fireTimer = FMath::Max(_fireTimer - DeltaTime, 0.0f);
	}

	if (_bTriggerPulled && !_bReloadInProgress && _fireTimer <= 0.0f)
	{
		switch (MyFireMode)
		{
		case EGunFireMode::SemiAuto:
		{
			if (!_bSemiFired)
			{
				TrySpawnProjectile();
				_bSemiFired = true;
				_fireTimer = _secondsPerFire;
			}
		}
		break;
		case EGunFireMode::Auto:
		{
			TrySpawnProjectile();
			_fireTimer = _secondsPerFire;
		}
		break;
		case EGunFireMode::BoltAction:
		{
			if (!_bSemiFired && !_bBoltNeedRepeat)
			{
				TrySpawnProjectile();
				_bSemiFired = true;
				_fireTimer = _secondsPerFire;
			}
			else if (_bBoltNeedRepeat)
			{
				RepeatBoltAction();
				_bBoltNeedRepeat = false;
				_fireTimer = BoltActionRepeatTimeSeconds;
			}
		}
		break;
		default:
		{
			if (FireEmptyChamberSound != NULL)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireEmptyChamberSound, GetActorLocation());
			}
			_bTriggerPulled = false;
		}
		break;
		}
	}

	//UE_LOG(LogTemp, Log, TEXT("FireTimer: %f BoltReloadTimer: %f"), _fireTimer, _boltReloadTimer);
}

void AGun::TickReload(float DeltaTime)
{
	if (_bReloadInProgress)
	{
		if (_reloadTimer > 0.0f)
		{
			_reloadTimer = FMath::Max(_reloadTimer - DeltaTime, 0.0f);
		}
		else
		{
			BulletsInMagazine = _bulletsToReload;
			_bReloadInProgress = false;
		}
	}
}

void AGun::TrySpawnProjectile()
{
	// check if we have ammunition
	if (BulletsInMagazine > 0)
	{
		// try and fire a projectile
		if (ProjectileClass != NULL)
		{
			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				FRotator SpawnRotation(GetTransform().GetRotation());
				AActor* ParentActor = GetParentActor();
				ATheWhiteDeathCharacter* Char = Cast<ATheWhiteDeathCharacter>(ParentActor);


				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((MuzzleLocation != nullptr) ? MuzzleLocation->GetComponentLocation() : GetActorLocation()) /*+ SpawnRotation.RotateVector(GunOffset)*/;

				APlayerCharacter* CharP = Cast<APlayerCharacter>(ParentActor);

				if (CharP != NULL && CharP->GetIsAiming())
				{
					SpawnRotation = FRotator(Mesh->GetComponentToWorld().GetRotation());
				}
				else if (Char != NULL)
				{
					FVector targetPos = Char->GetLocationWantsToHit();
					targetPos = targetPos - (SpawnLocation);
					targetPos.Normalize();
					SpawnRotation = targetPos.Rotation();
					
					//SpawnRotation = Char->GetControlRotation();
				}

				// take dispersion into account
				SpawnRotation.Yaw += FMath::FRandRange(-1.0f, 1.0f) * _currentDispersion.X * Char->GetDispersionMultiplier();
				SpawnRotation.Pitch += FMath::FRandRange(-1.0f, 1.0f) * _currentDispersion.Y * Char->GetDispersionMultiplier();

				// add to dispersion timer
				_dispersionTimer = FMath::Clamp(_dispersionTimer + (GWorld->GetDeltaSeconds() * 30.0f), 0.0f, BulletDispersionOverTime);

				// spawn the projectile at the muzzle
				ATheWhiteDeathProjectile* proj = World->SpawnActor<ATheWhiteDeathProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
				proj->SetOwningGun(this);

				// apply recoil to owner
				if (GetParentActor() != NULL && GetParentActor()->IsA<ATheWhiteDeathCharacter>())
				{
					ATheWhiteDeathCharacter* character = Cast<ATheWhiteDeathCharacter>(GetParentActor());
					character->ApplyRecoil(Recoil);
				}

				if (FireParticle != NULL)
				{
					FireParticle->ResetParticles();
					FireParticle->Activate();
				}

				// make NOISE
				UAIPerceptionSystem::MakeNoiseImpl(GetParentActor(), FireLoudness, (APawn*)GetParentActor(), GetActorLocation(), FireLoudnessMaxRadius, TEXT("Footsteps"));
			}
		}

		// try and play the sound if specified
		if (FireSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		// pick different animation, in regard of ammunition count
		UAnimMontage* fireAnimation = FireAnimation;
		if (BulletsInMagazine == 1)
		{
			fireAnimation = FireAnimationLastBullet;
		}

		// try and play a firing animation if specified
		if (fireAnimation != NULL)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(fireAnimation, 1.f);
			}
		}

		// lower ammunition count by one
		--BulletsInMagazine;
	}
	else
	{
		if (FireEmptyChamberSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireEmptyChamberSound, GetActorLocation());
		}
	}
}

void AGun::RepeatBoltAction()
{
	if (RepeatAnimation != NULL)
	{
		UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(RepeatAnimation, 1.f);
		}
	}

	if (RepeatSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, RepeatSound, GetActorLocation());
	}
}

void AGun::OnInteraction(APlayerCharacter* pc)
{
	//Cast<APlayerCharacter>(OtherActor)->GetEquipment()->AddAmmo(MyAmmoType, ammoCount);
	if (!IsPickedUp)
	{
		if (pc->GetEquipment()->PickUpGun(this))
		{
			PickUp(pc);
			pc->OnWeaponPickUp(this);
			IsPickedUp = true;
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		else
		{
			int ammoCount = FMath::RandRange(1, 20);
			pc->GetEquipment()->AddAmmo(MyAmmoType, ammoCount);
			InteractionZone->ObjectName = MyAmmoType + "AMMO";
			Destroy();
		}
	}
	else
	{
		Destroy();
	}
}