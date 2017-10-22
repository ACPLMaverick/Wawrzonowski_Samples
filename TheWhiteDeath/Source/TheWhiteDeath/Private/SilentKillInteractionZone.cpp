// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "SilentKillInteractionZone.h"
#include "Public/PlayerCharacter.h"
#include "Public/EnemyCharacter.h"

USilentKillInteractionZone::USilentKillInteractionZone(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	InputType = EInteractionZoneInputType::SILENTKILL;
	KillPosition = CreateDefaultSubobject<USceneComponent>(TEXT("KillPlayerPosition"));
	KillPosition->SetupAttachment(this);
}

void USilentKillInteractionZone::BeginPlay()
{
	Super::BeginPlay();

	FVector worldLocation = KillPosition->GetComponentLocation();
	worldLocation -= GetAttachmentRootActor()->GetActorForwardVector() * 100.0f;
	KillPosition->SetWorldLocation(worldLocation);
}

void USilentKillInteractionZone::Interaction_Implementation(APlayerCharacter * pc)
{
	// and again nothing is done here because I totally can't design my code
}

bool USilentKillInteractionZone::CanExecuteInteraction(APlayerCharacter * pc)
{
	if (!Super::CanExecuteInteraction(pc))
		return false;

	FVector kpDiff = GetAttachmentRootActor()->GetActorLocation() - KillPosition->GetComponentLocation();
	kpDiff.Z = 0.0f;
	kpDiff.Normalize();
	FVector pForward = pc->GetActorForwardVector();
	pForward.Z = 0.0f;
	pForward.Normalize();
	float dot = FVector::DotProduct(kpDiff, pForward);
	if (dot >= FMath::Cos(FMath::DegreesToRadians(ActionHalfAngle * 2.0f)))
	{
		return true;
	}
	else
	{
		return false;
	}
}
