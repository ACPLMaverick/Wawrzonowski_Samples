// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "EnemyCharacter.h"
#include "PlayerCharacter.h"

#include "Public/PlayerCharacter.h"
#include "Public/SilentKillInteractionZone.h"
#include "Public/SkillTree.h"
#include "Components/StaticMeshComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

AEnemyCharacter::AEnemyCharacter(const class FObjectInitializer& PCIP) :
	Super(PCIP)
{
	ViewCone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ViewCone"));
	ViewCone->SetCastShadow(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> coneMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cone.Shape_Cone'"));
	ViewCone->SetStaticMesh(coneMesh.Object);

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("EnemyPerception Component"));

	PerceptionComponent->ConfigureSense(*_configSight);
	PerceptionComponent->ConfigureSense(*_configHearing);
	PerceptionComponent->ConfigureSense(*_configDamage);
	PerceptionComponent->SetDominantSense(_configSight->GetSenseImplementation());

	_configHearing->DetectionByAffiliation.bDetectEnemies = true;
	_configHearing->DetectionByAffiliation.bDetectFriendlies = true;
	_configHearing->DetectionByAffiliation.bDetectNeutrals = true;
	//PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AEnemyCharacter::ProcessSense);

	SilentKillZone = CreateDefaultSubobject<USilentKillInteractionZone>(TEXT("SilentKillZone"));
	SilentKillZone->SetupAttachment(Mesh1P, NAME_None);
	SilentKillZone->DelegateExecuteInteraction.AddDynamic(this, &AEnemyCharacter::OnSilentKill);
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	ViewCone->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("CameraPoint"));
	ViewCone->bVisible = false;																							// this is temporary

		//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true), TEXT("GripPoint"));
}

void AEnemyCharacter::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (_bDamageDelayed)
	{
		if (_damageDelayedTimer <= 0.0f)
		{
			_bDamageDelayed = false;
			OnHit(_damageDelayed, _damageDelayedDirection);
		}
		else
		{
			--_damageDelayedTimer;
		}
	}

	if (_bTriggerPressed)
	{
		_releaseFireTimer += deltaTime;
		if (_releaseFireTimer >= 1.0f)
		{
			_bTriggerPressed = false;
			_releaseFireTimer = 0.0f;
			Super::OnReleaseFire();
		}
	}

	//if (BTState == EBehaviourTreeState::Suspicious && _detectedPlayer != NULL)
	//{
	//	// cast rays into player to verify if I actually see him

	//	FHitResult hResult;
	//	FVector eyePoint;
	//	FRotator eyeRot;
	//	GetActorEyesViewPoint(eyePoint, eyeRot);
	//	FVector eyeDir = eyeRot.Vector();
	//	FVector posDiff = _detectedPlayer->GetActorLocation() - GetActorLocation();
	//	posDiff.Normalize();
	//	float dot = FVector::DotProduct(posDiff, eyeDir);

	//	if (dot >= FMath::Cos(FMath::DegreesToRadians(SightHalfAngleDegrees)))
	//	{
	//		FVector seenLocation;
	//		float sightStrength;
	//		int32 losChecks;
	//		if (_detectedPlayer->CanBeSeenFrom(eyePoint, seenLocation, losChecks, sightStrength, this) && sightStrength > 0.0f)
	//		{
	//			// NAPIERDALAMYYYY!!!!
	//			//_detectionState = EPlayerDetectionState::Aware;
	//			BTState = EBehaviourTreeState::Aggresive;
	//			//UE_LOG(LogTemp, Log, TEXT("AWARE"));
	//		}
	//	}
	//}

	//Translate BTState to PlayerDetectionState for Map
	TranslateBTState();
}

void AEnemyCharacter::TranslateBTState()
{
	switch (BTState)
	{
	case EBehaviourTreeState::Idle:
		_detectionState = EPlayerDetectionState::Unaware;
		break;
	case EBehaviourTreeState::Patroling:
		_detectionState = EPlayerDetectionState::Unaware;
		break;
	case EBehaviourTreeState::Suspicious:
		_detectionState = EPlayerDetectionState::Suspicious;
		break;
	case EBehaviourTreeState::Searching:
		_detectionState = EPlayerDetectionState::Suspicious;
		break;
	case EBehaviourTreeState::Aggresive:
		_detectionState = EPlayerDetectionState::Aware;
		break;
	}
}

void AEnemyCharacter::Die()
{
	Super::Die();
	USkillTree::GrantExperiencePoints("Kill Enemy");

	IsBTStopped = true;
	PerceptionComponent->Deactivate();
	if (HasAIController)
	{
		this->GetCharacterMovement()->bOrientRotationToMovement = true;
		Cast<AAIController>(GetController())->ClearFocus(EAIFocusPriority::Default);
		Cast<AAIController>(GetController())->GetBrainComponent()->StopLogic("Dead");
		Cast<AAIController>(GetController())->StopMovement();
	}
	SetActorEnableCollision(false);
	if (Gun != NULL)
	{
		FActorSpawnParameters Parameters;
		Gun->GetChildActor()->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));

		AGun* gun = GetWorld()->SpawnActor<AGun>(Gun->GetChildActor()->GetClass(), Parameters);
		gun->SetActorLocation(Gun->GetChildActor()->GetActorLocation());
		if (gun != NULL)
		{
			gun->SetActorEnableCollision(true);
			gun->GetInteractionZone()->SetCapsuleSize(94.0f, 22.0f, false);
			gun->SetMyWeaponType(EWeaponType::SecondaryWeapon);
			gun->GetMesh()->SetCollisionProfileName("BlockAllDynamic");
			gun->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
			gun->GetMesh()->SetSimulatePhysics(true);
			gun->GetMesh()->SetMassOverrideInKg(NAME_None, 10.0f);
		}
		Gun->DestroyComponent();
	}
}

void AEnemyCharacter::ProcessSense(TArray<AActor*> sensedActors)
{

	//for (int32 i = 0; i < sensedActors.Num(); ++i)
	//{
	//	if ((_detectedPlayer = Cast<APlayerCharacter>(sensedActors[i])) != NULL && !IsPlayerDetected)
	//	{
	//		IsPlayerDetected = true;
	//		//_detectionState = EPlayerDetectionState::Suspicious;
	//		BTState = EBehaviourTreeState::Suspicious;
	//		//UE_LOG(LogTemp, Log, TEXT("SUSPICIOUS"));
	//		return;
	//	}
	//}

	////UE_LOG(LogTemp, Log, TEXT("UNAWARE"));
	////_detectionState = EPlayerDetectionState::Unaware;
	//BTState = EBehaviourTreeState::Patroling;
	//IsPlayerDetected = false;
	//_detectedPlayer = NULL;

}

void AEnemyCharacter::OnSilentKill(APlayerCharacter* pc)
{
	if (SilentKillDieAnimation != NULL)
	{
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(SilentKillDieAnimation, 1.f);
			_damageDelayedTimer = SilentKillDieAnimation->GetSectionLength(0) * 0.5f;
			_damageDelayed = SilentKillZone->GetTotalDamageTakes(MaxHealth);
			_bDamageDelayed = true;
			_damageDelayedDirection = GetActorLocation() - pc->GetActorLocation();
			_damageDelayedDirection.Normalize();
		}
	}
	else
	{
		FVector damageDelayedDirection = GetActorLocation() - pc->GetActorLocation();
		damageDelayedDirection.Normalize();
		OnHit(SilentKillZone->GetTotalDamageTakes(MaxHealth), damageDelayedDirection);
		//USkillTree::GrantExperiencePoints("Silent Kill");
	}
}

void AEnemyCharacter::DealDamageFromGrenade(float dmgAmount)
{
	Health -= dmgAmount;
}

void AEnemyCharacter::OnHit(float DmgAmount, FVector Direction)
{
	Super::OnHit(DmgAmount, Direction);

	if (!Cast<UCharacterMovementComponent>(GetMovementComponent())->IsCrouching() && Health > 0.0f)
	{
		int32 animNum = DamageTakingAnimations.Num();
		if (animNum != 0)
		{
			int32 id = FMath::RandRange(0, animNum - 1);
			if (DamageTakingAnimations[id] != NULL)
			{
				UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
				if (AnimInstance != NULL)
				{
					AnimInstance->Montage_Play(DamageTakingAnimations[id], 1.f);
				}
			}
		}
	}
}

void AEnemyCharacter::SetViewConeVisible()
{
	ViewCone->AddLocalRotation(FRotator(0.0f, 0.0f, -90.0f));
	ViewCone->SetVisibility(true);
}

void AEnemyCharacter::SetViewConeHidden()
{
	ViewCone->SetVisibility(false);
	ViewCone->AddLocalRotation(FRotator(0.0f, 0.0f, 90.0f));
}

void AEnemyCharacter::AttackEnemy()
{
	Super::OnFire();
	_bTriggerPressed = true;
	//Super::OnReleaseFire();
}

void AEnemyCharacter::OnReload()
{
	Super::OnReload();
}

void AEnemyCharacter::StopImmediate()
{
	IsBTStopped = true;
}

void AEnemyCharacter::ReleaseStopImmediate()
{
	IsBTStopped = false;
}