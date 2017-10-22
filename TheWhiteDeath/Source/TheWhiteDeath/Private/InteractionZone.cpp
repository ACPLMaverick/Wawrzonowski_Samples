// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "Public/PlayerCharacter.h"
#include "Engine.h"
#include "InteractionZone.h"
#include "DrawDebugHelpers.h"

UInteractionZone::UInteractionZone(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer)
{
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleZone"));
	CapsuleComponent->SetupAttachment(this);

	//DefaultCollisionChannel = ECollisionChannel::ECC_Pawn;
	CapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	bWantsInitializeComponent = true;
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionZone::BeginPlay()
{
	CapsuleComponent->OnComponentBeginOverlap.Clear();
	CapsuleComponent->OnComponentEndOverlap.Clear();
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &UInteractionZone::RegisterOnBeginOverlap);
	CapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &UInteractionZone::UnregisterOnEndOverlap);
	_components = GetAttachmentRootActor()->GetComponentsByClass(UActorComponent::StaticClass());
}

void UInteractionZone::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	// process delayed interaction executions
	int32 eCount = _execCalls.Num();
	if (eCount != 0)
	{
		for (int32 i = 0; i < eCount; ++i)
		{
			if (_execCalls[i].Timer <= 0.0f)
			{
				DelegateExecuteInteraction.Broadcast(_execCalls[i].Player);
				Interaction(_execCalls[i].Player);
				_execCalls.RemoveAt(i, 1, false);
			}
			else
			{
				_execCalls[i].Timer -= DeltaTime;
			}
		}

		_execCalls.Shrink();
	}
}

void UInteractionZone::RegisterOnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA<APlayerCharacter>())
	{
		RegisterToInputCall(Cast<APlayerCharacter>(OtherActor));
	}
}

void UInteractionZone::UnregisterOnEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA<APlayerCharacter>())
	{
		UnregisterFromInputCall(Cast<APlayerCharacter>(OtherActor));
	}
}

void UInteractionZone::RegisterToInputCall(APlayerCharacter * pc)
{
	switch (InputType)
	{
	case EInteractionZoneInputType::ACTION:
	{
		pc->RegisterInteractionZoneOnInputAction(this);
	}
	break;
	case EInteractionZoneInputType::SILENTKILL:
	{
		pc->RegisterInteractionZoneOnInputSilentKill(this);
	}
	break;
	default:
		break;
	}

	_players.Add(pc);
}

void UInteractionZone::UnregisterFromInputCall(APlayerCharacter * pc)
{
	switch (InputType)
	{
	case EInteractionZoneInputType::ACTION:
	{
		pc->UnregisterInteractionZoneOnInputAction(this);

	}
	break;
	case EInteractionZoneInputType::SILENTKILL:
	{
		pc->UnregisterInteractionZoneOnInputSilentKill(this);
	}
	break;
	default:
		break;
	}

	_players.Remove(pc);
}

void UInteractionZone::Interaction_Implementation(APlayerCharacter * pc)
{
}

bool UInteractionZone::CanExecuteInteraction(APlayerCharacter * pc)
{
	int32 index;
	if (!_players.Find(pc, index))
	{
		return false;
	}
	if (_components.Num() <= 1)
	{
		return true;
	}

	FHitResult hResult;
	FCollisionObjectQueryParams params;
	params.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	params.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
	params.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	FCollisionQueryParams qParams;
	qParams.AddIgnoredActor(pc);
	TArray<AActor*> cActors;
	pc->GetAllChildActors(cActors);
	for (int32 i = 0; i < cActors.Num(); ++i)
	{
		qParams.AddIgnoredActor(cActors[i]);
	}
	qParams.AddIgnoredComponent(Cast<const UPrimitiveComponent>(this));
	qParams.AddIgnoredComponent(Cast<const UPrimitiveComponent>(CapsuleComponent));

	FVector rayStart, rayEnd;
	FRotator camRot;
	pc->GetActorEyesViewPoint(rayStart, camRot);
	if (CapsuleComponent->GetScaledCapsuleHalfHeight() > 10.0f && CapsuleComponent->GetScaledCapsuleRadius() > 10.0f)
	{
		rayEnd = rayStart + camRot.Vector() * (CapsuleComponent->GetScaledCapsuleHalfHeight() + 2.0f * CapsuleComponent->GetScaledCapsuleRadius());
	}
	else
	{
		rayEnd = rayStart + camRot.Vector() * 500.0f;
	}

	if (GWorld->SweepSingleByObjectType(hResult, rayStart, rayEnd, FQuat(), params, FCollisionShape::MakeSphere(50.0f), qParams) &&
		(hResult.Component->IsA<UBoxComponent>() || hResult.Actor == GetAttachmentRootActor()))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void UInteractionZone::ExecuteInteraction(APlayerCharacter * pc, float delay)
{
	if (pc != NULL)
	{
		if (delay != 0.0f)
		{
			_execCalls.Add(FInteractionZoneExecutionCall(pc, delay));
		}
		else
		{
			DelegateExecuteInteraction.Broadcast(pc);
			Interaction(pc);

			if (ShowNotification)
			{
				USkillTree::AddNotificationToQueue("YOU PICKED UP " + ObjectName);
			}
		}
	}
}

void UInteractionZone::SetCapsuleSize(float InRadius, float InHalfHeight, bool bUpdateOverlaps)
{
	CapsuleComponent->SetCapsuleSize(InRadius, InHalfHeight, bUpdateOverlaps);
}
