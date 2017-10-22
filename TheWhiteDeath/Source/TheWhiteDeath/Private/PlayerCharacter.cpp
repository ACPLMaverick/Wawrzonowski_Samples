// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "PlayerCharacter.h"
#include "EnemyCharacter.h"
#include "Engine.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Public/InteractionZone.h"
#include "Public/SilentKillInteractionZone.h"
#include "Animation/AnimInstance.h"
#include "Public/MapComponent.h"
#include "Public/ObjectiveComponent.h"
#include "Public/EnemyHighlightComponent.h"
#include "Components/DecalComponent.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer & PCIP) :
	Super(PCIP)
{
	// Create a CameraComponent	
	Mesh1P->SetupAttachment(GetCapsuleComponent());

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(Mesh1P);
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	GunHideDetector = CreateDefaultSubobject<UBoxComponent>(TEXT("GunHideDetector"));
	GunHideDetector->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	GunHideDetector->SetupAttachment(Mesh1P);

	HighlightComponent = CreateDefaultSubobject<UEnemyHighlightComponent>(TEXT("EnemyHighlight"));
	HighlightComponent->SetupAttachment(FirstPersonCameraComponent);

	SkillTree = CreateDefaultSubobject<USkillTree>(TEXT("SkillTree"));
	InitSkillFunctions();


	Map = CreateDefaultSubobject<UMapComponent>(TEXT("Map"));

	TargetMarker = CreateDefaultSubobject<UDecalComponent>(TEXT("TargetMarker"));
	TargetMarker->SetupAttachment(FirstPersonCameraComponent);
	TargetMarker->SetVisibility(false);

	_canBeSeenChannelsToCheck.Add(ECollisionChannel::ECC_WorldStatic);
	_canBeSeenChannelsToCheck.Add(ECollisionChannel::ECC_WorldDynamic);
	_canBeSeenChannelsToCheck.Add(ECollisionChannel::ECC_Vehicle);
	_canBeSeenChannelsToCheck.Add(ECollisionChannel::ECC_PhysicsBody);
	_canBeSeenChannelsToCheck.Add(ECollisionChannel::ECC_Visibility);

	_cameraDefaultFOV = FirstPersonCameraComponent->FieldOfView;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	_mesh1PBaseRotation = FRotator(Mesh1P->GetRelativeTransform().GetRotation());

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true), TEXT("GripPointL"));

	FirstPersonCameraComponent->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("CameraPoint"));

	GunHideDetector->OnComponentBeginOverlap.Clear();
	GunHideDetector->OnComponentEndOverlap.Clear();
	GunHideDetector->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnGunHideBeginOverlap);
	GunHideDetector->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnGunHideEndOverlap);

	if (SkillTree)
	{
		SkillTree->RegisterComponent();
	}

	_gunRelativeTransform = Gun->GetRelativeTransform();

	ControlScheme = CreateWidget<UUserWidget>(GetWorld(), ControlSchemeClass);
	ControlScheme->AddToViewport();
	_bControlSchemeShown = true;
}

void APlayerCharacter::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (_bAlive)
	{
		TickHealthRegain(deltaTime);
		TickAim(deltaTime);
		TickWeaponSway(deltaTime);
		TickCover(deltaTime);
		TickVaultCover(deltaTime);
		TickViewConesVisibilitySkill(deltaTime);
		TickCheckForObjectives(deltaTime);
		TickMarker(deltaTime);
		TickSilentKill(deltaTime);
		TickCheckGrenade(deltaTime);

		// haxx ftw
		if (_haxxStickEventTimer > 0.0f)
		{
			_haxxStickEventTimer -= deltaTime;
		}

		// haxx ftw even moar
		if (!GetIsGunHidden())
		{
			Gun->SetRelativeLocation(FMath::VInterpTo(Gun->GetRelativeTransform().GetLocation(), _gunRelativeTransform.GetLocation(), deltaTime, 20.0f));
			Gun->SetRelativeRotation(FMath::RInterpTo(FRotator(Gun->GetRelativeTransform().GetRotation()), FRotator(_gunRelativeTransform.GetRotation()), deltaTime, 20.0f));
		}

		FRotator rot = GetControlRotation();
		FVector2D limits = Cast<UCharacterMovementComponent>(GetMovementComponent())->IsCrouching() ? CameraPitchRangeCrouch : CameraPitchRangeStand;
		const float correctionSpeed = 1.0f;
		limits.X += 360.0f;	// as it doesn't account for negative values, assuming minimum value is lesser than zero
		if (rot.Pitch >= 270.0f && rot.Pitch < 360.0f && rot.Pitch < limits.X)
		{
			rot.Pitch = rot.Pitch + ((correctionSpeed * (FMath::Abs(rot.Pitch - limits.X))));
			Cast<APlayerController>(GetController())->SetControlRotation(rot);
		}
		else if (rot.Pitch >= 0.0f && rot.Pitch < 90.0f && rot.Pitch > limits.Y)
		{
			rot.Pitch = rot.Pitch + (-(correctionSpeed * (FMath::Abs(rot.Pitch - limits.X))));
			Cast<APlayerController>(GetController())->SetControlRotation(rot);
		}
	}
	else
	{
		_deathTimer += deltaTime;
		FVector shit(1.0f - _deathTimer / _deathDelay);
		FirstPersonCameraComponent->PostProcessSettings.ColorGamma = shit;
		if (_deathTimer >= _deathDelay)
		{
			GetWorld()->GetAuthGameMode()->RestartGame();
		}
	}
}

void APlayerCharacter::OnHit(float DmgAmount = 15.0f, FVector Direction = FVector(0.0f, -1.0f, 0.0f))
{
	Super::OnHit(DmgAmount, Direction);

	_healthRegainTime += (DmgAmount / 10.0f);
	_bHealtRegainInProgress = false;

	HealthPostProcess();

	float cosAngle = 0.0f;
	FVector hitDirWithoutZ = FVector(-Direction.X, -Direction.Y, 0.0f);

	//FVector baseVector = FVector(1.0f, 0.0f, 0.0f);
	FVector baseVector = GetActorForwardVector();
	hitDirWithoutZ.Normalize();
	baseVector.Normalize();

	cosAngle = (FVector::DotProduct(hitDirWithoutZ, baseVector) / (hitDirWithoutZ.Size() * baseVector.Size()));

	float finalAngle = FMath::RadiansToDegrees(FGenericPlatformMath::Acos(cosAngle));
	FVector crossProduct = FVector::CrossProduct(hitDirWithoutZ, baseVector);

	if (crossProduct.Z > 0.0f)
	{
		float remainder = 0 - finalAngle;
		finalAngle = remainder;
	}

	if (HitMarkerClass) // Check the selected UI class is not NULL
	{
		HitMarker = CreateWidget<UHitMarkerWidget>(GetWorld(), HitMarkerClass); // Create Widget
		if (!HitMarker)
			return;
		HitMarker->AddToViewport(); // Add it to the viewport so the Construct() method in the UUserWidget:: is run.
	}

	/** THIS LINE IS FOR FINAL VERSION */
	HitMarker->PlayAppear(finalAngle);

	/** THIS LINE IS FOR TESTING */
	//HitMarker->PlayAppear(FMath::RandRange(-180.0f, 180.0f));
}

void APlayerCharacter::OnEnterCoverArea(ACover * cover)
{
	Super::OnEnterCoverArea(cover);
}

void APlayerCharacter::OnExitCoverArea()
{
	Super::OnExitCoverArea();
	/*UTWDMovementComponent* movement = Cast<UTWDMovementComponent>(GetMovementComponent());
	_bBeginCover = false;
	_bEndCover = false;*/
	//movement->ManualCapsuleSizeReset();
}

void APlayerCharacter::OnJump()
{
	OnReleaseHoldBreathInternal();

	if (_currentCover != NULL && CanVaultCover())
	{
		BeginVaultCover();
	}
	else
	{
		ACharacter::Jump();
	}
}

void APlayerCharacter::OnStopJumping()
{
	ACharacter::StopJumping();
}

void APlayerCharacter::OpenHelp()
{
	if (!_bControlSchemeShown)
	{
		ControlScheme->SetVisibility(ESlateVisibility::Visible);
		_bControlSchemeShown = true;
	}
}

void APlayerCharacter::OnFire()
{
	if (CanMove() && !GetIsGunHidden())
	{
		OnReleaseHoldBreathInternal();

		Super::OnFire();
	}
}

void APlayerCharacter::OnReleaseFire()
{
	Super::OnReleaseFire();
}

void APlayerCharacter::OnReload()
{
	if (CanMove() && !GetIsGunHidden())
	{
		OnReleaseHoldBreathInternal();

		Super::OnReload();
	}
}

void APlayerCharacter::OnBeginThrowGrenade()
{
	if (CanMove() && GetVelocity().Size() == 0.0f && !GetIsGunHidden() && GrenadeClass != NULL && Equipment->GrenadesCount > 0)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			FRotator SpawnRotation(GetTransform().GetRotation());
			const FVector SpawnLocation = GetActorLocation() + FirstPersonCameraComponent->ComponentToWorld.GetRotation().RotateVector(FVector(-50.f, 35.f, 120.f));

			Grenade = World->SpawnActor<AGrenade>(GrenadeClass, SpawnLocation, SpawnRotation);
			Grenade->OnReady();
			Grenade->AttachToComponent(FirstPersonCameraComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));

			Equipment->GrenadesCount--;

			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "BOOM!");
		}
	}
}

void APlayerCharacter::OnReleaseGrenade()
{
	if (CanMove() && !GetIsGunHidden() && Grenade != NULL)
	{
		Grenade->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
		Grenade->SetVelocity(FirstPersonCameraComponent->GetForwardVector());
		Super::OnReleaseGrenade();
		Grenade = NULL;

		if (GrenadeThrowAnimation != NULL)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(GrenadeThrowAnimation, 1.f);
			}
		}
	}
}

void APlayerCharacter::BeginCover(float dt)
{
	Super::BeginCover(dt);

	_recoilMultiplier = _recoilMultiplierCover;
	_dispersionMultiplier = _dispersionMultiplierCover;
}

void APlayerCharacter::EndCover(float dt)
{
	Super::EndCover(dt);

	_recoilMultiplier = _recoilMultiplierBase;
	_dispersionMultiplier = _dispersionMultiplierBase;
}

void APlayerCharacter::OnChangeAimMode()
{
	if (Gun != NULL && Gun->GetChildActor() != NULL)
	{
		Cast<AGun>(Gun->GetChildActor())->ChangeAimMode();
		if (_bIsAiming || (_bAimingInProgress && _aimTargetAlpha > 0.0f))
		{
			OnAiming();
		}
	}
}

void APlayerCharacter::OnAction()
{
	if (CanMove())
	{
		OnReleaseHoldBreathInternal();

		for (int32 i = 0; i < _interZonesAction.Num(); ++i)
		{
			if (_interZonesAction[i]->CanExecuteInteraction(this))
			{
				_interZonesAction[i]->ExecuteInteraction(this, 0.0f);

				// here will be actions taken according to given interaction zone type
			}
		}
	}
}

void APlayerCharacter::OnSilentKill()
{
	if (CanMove() && !_bSilentKillLerpInProgress)
	{
		OnReleaseHoldBreathInternal();

		float distance = FLT_MAX;
		for (int32 i = 0; i < _interZonesSilentKill.Num(); ++i)
		{
			if (_interZonesSilentKill[i]->CanExecuteInteraction(this))
			{
				USilentKillInteractionZone* zone = Cast<USilentKillInteractionZone>(_interZonesSilentKill[i]);
				float currentDistance = FVector::DistSquared(GetActorLocation(), zone->GetKillWorldLocation());
				if (currentDistance < distance)
				{
					_closestInteractionZone = zone;
					distance = currentDistance;
				}
			}
		}

		if (_closestInteractionZone != NULL)
		{
			AEnemyCharacter* enemy = Cast<AEnemyCharacter>(_closestInteractionZone->GetOwner());
			if (enemy != NULL)
			{
				enemy->StopImmediate();
			}
			float time = 1.0f;
			if (SilentKillAnimation != NULL)
			{
				time = SilentKillAnimation->GetSectionLength(0) * 0.65f;
			}
			_bSilentKillLerpInProgress = true;
			_closestInteractionZone->ExecuteInteraction(this, time);
			EndCrouch();
		}
	}
}

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	_myInputComponent = PlayerInputComponent;

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::OnJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::OnStopJumping);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATheWhiteDeathCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ATheWhiteDeathCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATheWhiteDeathCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ATheWhiteDeathCharacter::EndSprint);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::OnFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayerCharacter::OnReleaseFire);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &APlayerCharacter::OnAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &APlayerCharacter::OnReleaseAiming);

	PlayerInputComponent->BindAction("ChangeAimMode", IE_Pressed, this, &APlayerCharacter::OnChangeAimMode);

	PlayerInputComponent->BindAction("HoldBreath", IE_Pressed, this, &APlayerCharacter::OnHoldBreath);
	PlayerInputComponent->BindAction("HoldBreath", IE_Released, this, &APlayerCharacter::OnReleaseHoldBreath);

	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &APlayerCharacter::OnAction);
	
	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &APlayerCharacter::OnBeginThrowGrenade);
	PlayerInputComponent->BindAction("Throw", IE_Released, this, &APlayerCharacter::OnReleaseGrenade);

	PlayerInputComponent->BindAction("SilentKill", IE_Pressed, this, &APlayerCharacter::OnSilentKill);

	PlayerInputComponent->BindAction("SkillListen", IE_Pressed, this, &APlayerCharacter::OnListen);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlayerCharacter::OnReload);
	PlayerInputComponent->BindAction("WeaponChangeUp", IE_Pressed, this, &APlayerCharacter::OnWeaponChange);
	PlayerInputComponent->BindAction("GUISkillTreeShow", IE_Pressed, this, &APlayerCharacter::OnSkillTreeMenuButtonPressed);
	PlayerInputComponent->BindAction("ShowViewCones", IE_Released, this, &APlayerCharacter::OpenHelp);

	PlayerInputComponent->BindAction("GUIMapShow", IE_Pressed, this, &APlayerCharacter::OnMapShowButtonPressed);
	PlayerInputComponent->BindAction("GUIMapMarkInWorld", IE_Pressed, this, &APlayerCharacter::OnMapMarkInWorldButtonPressed);
	PlayerInputComponent->BindAction("GUIMapMarkInWorld", IE_Released, this, &APlayerCharacter::OnMapMarkInWorldButtonReleased);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::TurnAt);
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUpAt);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("DeviceRecognition", IE_Pressed, this, &APlayerCharacter::OnDeviceRecognition);
	PlayerInputComponent->BindAction("DeviceRecognitionDPad", IE_Pressed, this, &APlayerCharacter::OnDeviceRecognitionDPad);
	PlayerInputComponent->BindAxis("DeviceRecognitionAxisMouse", this, &APlayerCharacter::OnDeviceRecognitionAxisMouse);
	PlayerInputComponent->BindAxis("DeviceRecognitionAxisGamepad", this, &APlayerCharacter::OnDeviceRecognitionAxisGamepad);

	//PlayerInputComponent->BindAxis("SkillSelectUp", this, &APlayerCharacter::OnSelectSkillUpPressed);
	//PlayerInputComponent->BindAxis("SkillSelectRight", this, &APlayerCharacter::OnSelectSkillRightPressed);

	PlayerInputComponent->BindAction("SkillSelectUp", IE_Pressed, this, &APlayerCharacter::OnSelectSkillUpPressed);
	PlayerInputComponent->BindAction("SkillSelectRight", IE_Pressed, this, &APlayerCharacter::OnSelectSkillRightPressed);
	PlayerInputComponent->BindAction("SkillSelectDown", IE_Pressed, this, &APlayerCharacter::OnSelectSkillDownPressed);
	PlayerInputComponent->BindAction("SkillSelectLeft", IE_Pressed, this, &APlayerCharacter::OnSelectSkillLeftPressed);

	PlayerInputComponent->BindAction("Test_HitFromFront", IE_Released, this, &APlayerCharacter::OnHitTest);
	PlayerInputComponent->BindAction("Debug_AddThreeTokens", IE_Released, this, &APlayerCharacter::AddThreeTokens);
	PlayerInputComponent->BindAction("Debug_ResetSkillTree", IE_Released, SkillTree, &USkillTree::ResetSkillTree);

	PlayerInputComponent->BindAction("Debug_Die", IE_Released, this, &APlayerCharacter::DebugDie);
}

void APlayerCharacter::Die()
{
	Super::Die();

	OnReleaseHoldBreathInternal();

	if (GEngine != NULL)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "YOU'RE DEAD");
	}
	_healthRegainTime = 0.0f;

	FirstPersonCameraComponent->bUsePawnControlRotation = false;
	FirstPersonCameraComponent->PostProcessSettings.bOverride_ColorGamma = true;

	_myInputComponent->Deactivate();
}

void APlayerCharacter::DebugDie()
{
	Health = -MaxHealth;
}

void APlayerCharacter::HealthPostProcess()
{
	FirstPersonCameraComponent->PostProcessSettings.FilmSaturation = Health / MaxHealth;
	FirstPersonCameraComponent->PostProcessSettings.VignetteIntensity = ((MaxHealth - Health) / MaxHealth) + 0.4f;
	if (Health <= 30.0f)
	{
		float contrast = ((30.0f - Health) / 30.0f) + 0.03f;
		FMath::Clamp(contrast, 0.03f, 0.5f);
		FirstPersonCameraComponent->PostProcessSettings.FilmContrast = contrast;
	}
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	if (CanMove())
	{
		Super::TurnAtRate((_bIsAiming && Gun != NULL && Gun->GetChildActor() != NULL) ? Rate * Cast<AGun>(Gun->GetChildActor())->GetAimSensitivityMultiplier() : Rate);
	}
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	if (CanMove())
	{
		Super::LookUpAtRate((_bIsAiming && Gun != NULL && Gun->GetChildActor() != NULL) ? Rate * Cast<AGun>(Gun->GetChildActor())->GetAimSensitivityMultiplier() : Rate);
	}
}

void APlayerCharacter::TurnAt(float Rate)
{
	if (CanMove())
	{
		AddControllerYawInput((_bIsAiming && Gun != NULL && Gun->GetChildActor() != NULL) ? Rate * Cast<AGun>(Gun->GetChildActor())->GetAimSensitivityMultiplier() : Rate);
	}
}

void APlayerCharacter::LookUpAt(float Rate)
{
	if (CanMove())
	{
		AddControllerPitchInput((_bIsAiming && Gun != NULL && Gun->GetChildActor() != NULL) ? Rate * Cast<AGun>(Gun->GetChildActor())->GetAimSensitivityMultiplier() : Rate);
	}
}

void APlayerCharacter::RegisterInteractionZoneOnInputAction(UInteractionZone * uiz)
{
	_interZonesAction.Add(uiz);
}

void APlayerCharacter::RegisterInteractionZoneOnInputSilentKill(UInteractionZone * uiz)
{
	_interZonesSilentKill.Add(uiz);
}

void APlayerCharacter::UnregisterInteractionZoneOnInputAction(UInteractionZone * uiz)
{
	_interZonesAction.Remove(uiz);
}

void APlayerCharacter::UnregisterInteractionZoneOnInputSilentKill(UInteractionZone * uiz)
{
	_interZonesSilentKill.Remove(uiz);
}

void APlayerCharacter::MoveForward(float Value)
{
	if (!SkillTree->InSkillTreeMenu)
	{
		if (CanMove())
		{
			Super::MoveForward(Value);

			if (Value != 0.0f)
			{
				_bCanLeaveCoverThisFrame = true;
			}

			if (FMath::Abs(Value) > 0.1f)
			{
				OnReleaseHoldBreathInternal();
			}
		}
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (CanMove())
	{
		Super::MoveRight(Value);

		if (Value != 0.0f)
		{
			_bCanLeaveCoverThisFrame = true;
		}

		if (FMath::Abs(Value) > 0.1f)
		{
			OnReleaseHoldBreathInternal();
		}
	}
}

void APlayerCharacter::OnAiming()
{
	if (CanMove() && !GetIsGunHidden())
	{
		// temp
		if (Gun != NULL && Gun->GetChildActor() != NULL)
		{
			_bAimingInProgress = true;
			_cameraTargetFOV = Cast<AGun>(Gun->GetChildActor())->GetCameraAimedFOV();
			_aimTargetAlpha = 1.0f;

			FirstPersonCameraComponent->AttachToComponent(Cast<AGun>(Gun->GetChildActor())->GetAimLocationComponent(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
		}
	}
}

void APlayerCharacter::OnReleaseAiming()
{
	if (_bIsAiming || _bAimingInProgress)
	{
		_cameraTargetFOV = _cameraDefaultFOV;
		_bAimingInProgress = true;
	}
	else
	{
		_bAimingInProgress = false;
	}
	_bIsAiming = false;
	_aimTargetAlpha = 0.0f;

	OnReleaseHoldBreathInternal();

	// temp
	FirstPersonCameraComponent->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true), TEXT("CameraPoint"));
}

void APlayerCharacter::OnHoldBreath()
{
	OnHoldBreathInternal(true);
}

void APlayerCharacter::OnReleaseHoldBreath()
{
	OnReleaseHoldBreathInternal(true);
}

void APlayerCharacter::OnHoldBreathInternal(bool sound)
{
	if (!_bIsHoldingBreath && _bIsAiming && GetVelocity().Size() < 0.001f)
	{
		if (sound && SoundHoldBreath != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, SoundHoldBreath, GetActorLocation());
		}

		//Mesh1P->GlobalAnimRateScale *= 0.3f;		// this is done in animation blueprint
		_bIsHoldingBreath = true;
	}
}

void APlayerCharacter::OnReleaseHoldBreathInternal(bool sound)
{
	if (_bIsHoldingBreath)
	{
		if (sound && SoundReleaseBreath != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, SoundReleaseBreath, GetActorLocation());
		}

		//Mesh1P->GlobalAnimRateScale = 1.0f;		// this is done in animation blueprint
		_bIsHoldingBreath = false;
	}
}

void APlayerCharacter::OnHitTest()
{
	OnHit();
}

void APlayerCharacter::OnWeaponChange()
{
	if (CanMove())
	{
		OnReleaseHoldBreathInternal();

		if (Equipment->SwitchGun())
		{
			if (GEngine != NULL)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::FromInt(Equipment->MyCurrentWeapon));
			}
			Gun->SetChildActorClass(Equipment->MyWeapons[Equipment->MyCurrentWeapon]->GetClass());
		}
	}
}

void APlayerCharacter::OnWeaponPickUp(AGun* pickedGun)
{
	pickedGun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true), TEXT("GripPoint"));
	pickedGun->SetActorRelativeTransform(Gun->GetRelativeTransform());

	pickedGun->SetActorHiddenInGame(true);
}

void APlayerCharacter::OnSkillTreeMenuButtonPressed()
{
	if (SkillTree->AreThereSkillsToAccept)
	{
		SkillTree->ShowAcceptDialog();
	}
	else
	{
		SkillTree->ToggleSkillTreeMenu();
	}	
}

void APlayerCharacter::OnSelectSkillRightPressed()
{
	if (SkillTree->InSkillTreeMenu)
	{
		SkillTree->InvokeHoveredSkillMoveRight();
	}
}
void APlayerCharacter::OnSelectSkillUpPressed()
{
	if (SkillTree->InSkillTreeMenu)
	{
		SkillTree->InvokeHoveredSkillMoveUp();
	}
}
void APlayerCharacter::OnSelectSkillLeftPressed()
{
	if (SkillTree->InSkillTreeMenu)
	{
		SkillTree->InvokeHoveredSkillMoveLeft();
	}
}
void APlayerCharacter::OnSelectSkillDownPressed()
{
	if (SkillTree->InSkillTreeMenu)
	{
		SkillTree->InvokeHoveredSkillMoveDown();
	}
}

void APlayerCharacter::AddThreeTokens()
{
	SkillTree->TokensCount = 3;
}

bool APlayerCharacter::HasSkill(int32 id)
{
	return SkillTree->HasSkill(id);
}

void APlayerCharacter::OnListen()
{
	if (CanMove() && _bCanFocusMode)
	{
		HighlightComponent->Highlight();
	}
}

void APlayerCharacter::OnShowingViewCones()
{
	if (_bCanSeeViewCones && !_bAreViewConesVisible)
	{
		for (TActorIterator<AEnemyCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			AEnemyCharacter *Enemy = *ActorItr;
			Cast<AEnemyCharacter>(*ActorItr)->SetViewConeVisible();
		}
		_bAreViewConesVisible = true;
	}
}

void APlayerCharacter::HideViewCones()
{
	for (TActorIterator<AEnemyCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AEnemyCharacter *Enemy = *ActorItr;
		Cast<AEnemyCharacter>(*ActorItr)->SetViewConeHidden();
	}
	_bAreViewConesVisible = false;
}

void APlayerCharacter::OnMapShowButtonPressed()
{
	//Added in case of skill tree turned on
	if (SkillTree->InSkillTreeMenu)
	{
		SkillTree->ToggleSkillTreeMenu();
	}

	Map->ToggleVisible();
}

void APlayerCharacter::OnMapMarkInWorldButtonPressed()
{
	if (CanMove())
	{
		_bMarkInWorldEnabled = true;
		Map->SetMarkMenuOn();
		TargetMarker->SetVisibility(true);
	}
}

void APlayerCharacter::OnMapMarkInWorldButtonReleased()
{
	_bMarkInWorldEnabled = false;
	Map->SetMarkMenuOff();
	TargetMarker->SetVisibility(false);
}

void APlayerCharacter::OnDeviceRecognition(FKey key)
{
	EInputDeviceType type;
	if (key.IsGamepadKey())
	{
		type = EInputDeviceType::Gamepad;
	}
	else
	{
		type = EInputDeviceType::KeyboardMouse;
	}

	if (type != _lastInputType)
	{
		_lastInputType = type;
		DelegateInputTypeChanged.Broadcast(_lastInputType);
	}

	if (_bControlSchemeShown)
	{
		ControlScheme->SetVisibility(ESlateVisibility::Collapsed);
		_bControlSchemeShown = false;
	}
}

void APlayerCharacter::OnDeviceRecognitionDPad()
{
	if (_lastInputType != EInputDeviceType::Gamepad)
	{
		_lastInputType = EInputDeviceType::Gamepad;
		DelegateInputTypeChanged.Broadcast(_lastInputType);
	}
}

void APlayerCharacter::OnDeviceRecognitionAxisMouse(float value)
{
	if (_haxxStickEventTimer <= 0.0f)
	{
		if (value != 0.0f)
		{
			if (_lastInputType != EInputDeviceType::KeyboardMouse)
			{
				_lastInputType = EInputDeviceType::KeyboardMouse;
				DelegateInputTypeChanged.Broadcast(_lastInputType);
			}
		}

		FVector newMousePosition;
		Cast<APlayerController>(GetController())->GetMousePosition(newMousePosition.X, newMousePosition.Y);

		if (newMousePosition.X != _lastCursorPosition.X || newMousePosition.Y != _lastCursorPosition.Y)
		{
			_lastCursorPosition = newMousePosition;
			if (_lastInputType != EInputDeviceType::KeyboardMouse)
			{
				_lastInputType = EInputDeviceType::KeyboardMouse;
				DelegateInputTypeChanged.Broadcast(_lastInputType);
			}
		}
	}
	else
	{
		Cast<APlayerController>(GetController())->GetMousePosition(_lastCursorPosition.X, _lastCursorPosition.Y);
	}
}

void APlayerCharacter::OnDeviceRecognitionAxisGamepad(float value)
{
	if (value != 0.0f)
	{
		if (_lastInputType != EInputDeviceType::Gamepad)
		{
			_haxxStickEventTimer = 0.1f;
			_lastInputType = EInputDeviceType::Gamepad;
			DelegateInputTypeChanged.Broadcast(_lastInputType);
		}
	}
}

void APlayerCharacter::GetLocationWantsToHitData(FVector& outOrigin, FVector& outDirection)
{
	outOrigin = FirstPersonCameraComponent->GetComponentLocation();
	outDirection = FirstPersonCameraComponent->GetForwardVector();
}

void APlayerCharacter::TickHealthRegain(float dt)
{
	/* Timer to start regain health */
	if (_healthRegainTime > 0.0f)
	{
		_healthRegainTime -= dt;
	}
	else
	{
		_bHealtRegainInProgress = true;
	}

	/* Regaining health */
	if (_bHealtRegainInProgress)
	{
		if (Health < MaxHealth)
		{
			if (_currentCover != NULL)
			{
				Health = FMath::Min(Health + _healthUpPerSecond * dt * 2.0f, MaxHealth);
			}
			else
			{
				Health = FMath::Min(Health + _healthUpPerSecond * dt, MaxHealth);
			}
			HealthPostProcess();
		}
		else
		{
			_healthRegainTime = 0.0f;
			_bHealtRegainInProgress = false;
		}
	}
}

void APlayerCharacter::TickAim(float dt)
{
	if (_bAimingInProgress)
	{
		const float speed = 8.0f;
		const float thres = 0.05f;

		float fov = FMath::FInterpTo(FirstPersonCameraComponent->FieldOfView, _cameraTargetFOV, dt, speed);

		_aimAlpha = FMath::FInterpTo(_aimAlpha, _aimTargetAlpha, dt, speed);

		FirstPersonCameraComponent->SetFieldOfView(fov);

		FirstPersonCameraComponent->SetRelativeLocation(FMath::VInterpTo(FirstPersonCameraComponent->GetRelativeTransform().GetLocation(), FVector::ZeroVector, dt, speed));

		if (FirstPersonCameraComponent->GetRelativeTransform().GetLocation().Size() < thres)
		{
			FirstPersonCameraComponent->SetFieldOfView(_cameraTargetFOV);
			_bIsAiming = _cameraTargetFOV == Cast<AGun>(Gun->GetChildActor())->GetCameraAimedFOV();
			_bAimingInProgress = false;
			_aimAlpha = _aimTargetAlpha;
			FirstPersonCameraComponent->SetRelativeLocation(FVector::ZeroVector);
		}
	}
}

void APlayerCharacter::TickWeaponSway(float dt)
{
	if (CanMove())
	{
		_swayVectorLast = _swayVector;
		_swayVector.X = _myInputComponent->GetAxisValue(TEXT("Turn"));
		if (_swayVector.X == 0.0f)
		{
			_swayVector.X = _myInputComponent->GetAxisValue(TEXT("TurnRate")) * BaseTurnRate * dt;
		}

		_swayVector.Y = -_myInputComponent->GetAxisValue(TEXT("LookUp"));
		if (_swayVector.Y == 0.0f)
		{
			_swayVector.Y = -_myInputComponent->GetAxisValue(TEXT("LookUpRate")) * BaseLookUpRate * dt;
		}

		_swayVector.X *= _swayMultiplierX;
		_swayVector.Y *= _swayMultiplierY;
		_swayVector = FMath::Vector2DInterpTo(_swayVectorLast, _swayVector, dt, _swaySmooth);

		// rotating Mesh1P is temporary. In a final version, one will probably have to adjust arm IK animation by these values
		FRotator swayRotation(_mesh1PBaseRotation.Pitch + _swayVector.Y,
			_mesh1PBaseRotation.Yaw + _swayVector.X, _mesh1PBaseRotation.Roll);
		Mesh1P->SetRelativeRotation(swayRotation.Quaternion());
	}
}

void APlayerCharacter::TickViewConesVisibilitySkill(float dt)
{
	if (_bAreViewConesVisible)
	{
		_viewConesVisibleTimer += dt;
		if (_viewConesVisibleTimer >= 15.0f)
		{
			HideViewCones();
			_viewConesVisibleTimer = 0.0f;
		}
	}
}

void APlayerCharacter::TickCheckForObjectives(float dt)
{
	if (Map != NULL && Map->GetActiveObjectivesFindNum() > 0)
	{
		FVector start, end;
		FRotator rot;
		GetActorEyesViewPoint(start, rot);
		end = rot.Vector();
		end *= 10000.0f;

		FHitResult outHit;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		GetWorld()->LineTraceSingleByObjectType(outHit, start, end, FCollisionObjectQueryParams::AllObjects, params);

		UObjectiveComponent* objective = NULL;
		if (outHit.Actor != NULL)
		{
			if ((objective = Cast<UObjectiveComponent>(outHit.GetActor()->GetComponentByClass(UObjectiveComponent::StaticClass()))) != NULL)
			{
				objective->SetMarkedVisible();
			}
		}
	}
}

void APlayerCharacter::TickMarker(float dt)
{
	TargetMarker->SetWorldLocation(_locationWantsToHit);
	float scaleMplier = FMath::Clamp(FMath::Loge(FVector::Dist(_locationWantsToHit, GetActorLocation()) * 0.01f), 1.0f, 100.0f);
	TargetMarker->SetRelativeScale3D(FVector(scaleMplier, scaleMplier, scaleMplier));
}

void APlayerCharacter::TickSilentKill(float dt)
{
	if (_bSilentKillLerpInProgress)
	{
		if (_closestInteractionZone != NULL)
		{
			const FVector loc(GetActorLocation());
			const FVector dest(_closestInteractionZone->GetKillWorldLocation().X, _closestInteractionZone->GetKillWorldLocation().Y, loc.Z);
			APlayerController* controller(Cast<APlayerController>(GetController()));
			if (FVector::Dist(loc, dest) < 10.0f)
			{
				SetActorLocation(dest);
				Cast<APlayerController>(GetController())->SetControlRotation(_closestInteractionZone->GetOwner()->GetActorRotation());

				if (SilentKillAnimation != NULL)
				{
					// Get the animation object for the arms mesh
					UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
					if (AnimInstance != NULL)
					{
						AnimInstance->Montage_Play(SilentKillAnimation, 1.f);
					}
				}

				_bSilentKillLerpInProgress = false;
				_closestInteractionZone = NULL;
			}
			else
			{
				SetActorLocation(FMath::VInterpTo(loc, dest, dt, 1.0f));
				controller->SetControlRotation(FMath::RInterpTo(controller->GetControlRotation(), _closestInteractionZone->GetOwner()->GetActorRotation(), dt, 5.0f));
			}
		}
	}
}

void APlayerCharacter::TickCheckGrenade(float dt)
{
	if (Grenade != NULL)
	{
		if (!Grenade->GetIsGrenadeReady())
		{
			//USUNIÊCIE FLAGI ¯E KORZYSTAMY Z GRANATU
		}
	}
}

bool APlayerCharacter::CanMove()
{
	return _bAlive && !_bCoverBeginRotationLerp && !_bVaultCover && !SkillTree->InSkillTreeMenu && !Map->IsVisible() && !_bSilentKillLerpInProgress && !_bMarkInWorldEnabled && !_bControlSchemeShown;
}

bool APlayerCharacter::CanCrouch()
{
	return Super::CanCrouch() && !_bVaultCover && !_bSilentKillLerpInProgress;
}

bool APlayerCharacter::CanSprint()
{
	return Super::CanSprint() && GetVelocity().Size() > 0.1f && !_bIsAiming;
}

void APlayerCharacter::InitSkillFunctions()
{
	//COMBAT SKILLS
	SkillFunctions[0] = &APlayerCharacter::EmptyFunction;	//Deep breath
	SkillFunctions[1] = &APlayerCharacter::EmptyFunction;	//Improved aim
	SkillFunctions[2] = &APlayerCharacter::EmptyFunction;	//Improved damage
	SkillFunctions[3] = &APlayerCharacter::EmptyFunction;	//Improved recoil
	SkillFunctions[4] = &APlayerCharacter::EmptyFunction;	//HP Regeneration
	SkillFunctions[5] = &APlayerCharacter::EmptyFunction;	//Improved defense

	//STEALTH SKILLS
	SkillFunctions[6] = &APlayerCharacter::BuyFocusMode;
	SkillFunctions[7] = &APlayerCharacter::EmptyFunction;	//Show enemy state
	SkillFunctions[8] = &APlayerCharacter::BuyViewConeVisibility;
	SkillFunctions[9] = &APlayerCharacter::EmptyFunction;	//Lower bullet volume
	SkillFunctions[10] = &APlayerCharacter::EmptyFunction;	//Quiet shot
	SkillFunctions[11] = &APlayerCharacter::EmptyFunction;	//Quiet walking
}

void APlayerCharacter::PlaySkillFunction(int index, int level)
{
	if (index >= 12 || index < 0) return;

	(this->*(SkillFunctions[index]))(level);
}

bool APlayerCharacter::CanBeSeenFrom(const FVector & ObserverLocation, FVector & OutSeenLocation, int32 & NumberOfLoSChecksPerformed, float & OutSightStrength, const AActor * IgnoreActor) const
{
	// will have to take light and weather into account in OutSightStrength

	//FVector HeadLocation = Mesh1P->GetBoneLocation("Head", EBoneSpaces::WorldSpace);
	FVector HeadLocation = FirstPersonCameraComponent->GetComponentLocation();	// just for now
	FHitResult hResult;
	FCollisionQueryParams params;
	params.AddIgnoredActor(IgnoreActor);
	params.AddIgnoredActor(this);
	bool blocked = false;
	for (int32 i = 0; i < _canBeSeenChannelsToCheck.Num(); ++i)
	{
		if (GWorld->LineTraceSingleByChannel(hResult, ObserverLocation, HeadLocation, _canBeSeenChannelsToCheck[i], params))
		{
			OutSeenLocation = hResult.Location;
			OutSightStrength = 0.0f;
			blocked = true;
		}
	}

	if (!blocked)
	{
		OutSightStrength = 1.0f;
		OutSeenLocation = HeadLocation;
	}

	return !blocked;
}

bool APlayerCharacter::BuySkill(int id)
{
	bool output = SkillTree->OnBuySkillPress(id);

	//FunctionPtr Array invoke
	PlaySkillFunction(id, SkillTree->CurrentSkillsLevels[id]);

	return output;
}

//SKILL FUNCTIONS

void APlayerCharacter::EmptyFunction(int level)
{
	return;
}

void APlayerCharacter::BuyViewConeVisibility(int level)
{
	if (GEngine != NULL)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "PRESS B TO SEE VIEWCONES");
	}
	_bCanSeeViewCones = true;
}

void APlayerCharacter::BuyFocusMode(int level)
{
	_bCanFocusMode = true;
}

void APlayerCharacter::OnGunHideBeginOverlap(UPrimitiveComponent * overlappedComponent, AActor * otherActor, UPrimitiveComponent * otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult & sweepResult)
{
	if (otherActor != this && otherActor != Gun->GetChildActor() && (otherComp->IsA<UStaticMeshComponent>() || otherComp->IsA<USkeletalMeshComponent>()))
	{
		++_gunHideDetectorOverlapCount;
		Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true), TEXT("GripPoint"));
		//UE_LOG(LogTemp, Log, TEXT("Overlaps up: %d %s"), _gunHideDetectorOverlapCount, &otherActor->GetName().GetCharArray()[0]);
	}
}

void APlayerCharacter::OnGunHideEndOverlap(UPrimitiveComponent * overlappedComponent, AActor * otherActor, UPrimitiveComponent * otherComp, int32 otherBodyIndex)
{
	if (otherActor != this && otherActor != Gun->GetChildActor() && (otherComp->IsA<UStaticMeshComponent>() || otherComp->IsA<USkeletalMeshComponent>()))
	{
		--_gunHideDetectorOverlapCount;
		Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true), TEXT("GripPointL"));
		//UE_LOG(LogTemp, Log, TEXT("Overlaps down: %d %s"), _gunHideDetectorOverlapCount, &otherActor->GetName().GetCharArray()[0]);
	}
}

/////////////////
