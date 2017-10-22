// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "TheWhiteDeathCharacter.h"
#include "ObjectiveComponent.h"
#include "Components/BillboardComponent.h"
#include "Public/SkillTree.h"

// Sets default values for this component's properties
UObjectiveComponent::UObjectiveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetHiddenInGame(false);
	Billboard->SetupAttachment(this);
}


// Called when the game starts
void UObjectiveComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ObjectiveImportance == EObjectiveImportance::Primary)
	{
		Billboard->SetSprite(TexturePrimary);
	}
	else if (ObjectiveImportance == EObjectiveImportance::Secondary)
	{
		Billboard->SetSprite(TextureSecondary);
	}

	_lastActorLocation = GetOwner()->GetActorLocation();
	Billboard->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void UObjectiveComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	if (ObjectiveType == EObjectiveType::Destroy && GetWorld() != NULL && GetWorld()->GetGameInstance() != NULL)
	{
		OnCompleted();
	}
}

// Called every frame
void UObjectiveComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (!_bIsFound)
	{
		if (ObjectiveType == EObjectiveType::Find)
		{
			if (_bMarkedOnMap && _bMarkedVisible)
			{
				OnCompleted();
			}
		}
		else if (ObjectiveType == EObjectiveType::Collect)
		{
			// TBA
		}
		else if (ObjectiveType == EObjectiveType::Destroy)
		{
			ATheWhiteDeathCharacter* character = NULL;
			if ((character = Cast<ATheWhiteDeathCharacter>(GetOwner())) != NULL)
			{
				if (character->Health <= 0.0f)
				{
					OnCompleted();
				}
			}
		}

		if (_lastActorLocation != GetOwner()->GetActorLocation())
		{
			_lastActorLocation = GetOwner()->GetActorLocation();
			DelegateObjectiveMoved.Broadcast(this, _lastActorLocation);
		}
	}
}

void UObjectiveComponent::SetMarkedVisible()
{
	if (!_bMarkedVisible)
	{
		_bMarkedVisible = true;
		DelegateObjectiveMarkedVisible.Broadcast(this);

		if (ObjectiveType == EObjectiveType::Find)
		{
			FString notification = "Objective spotted: " + ObjectiveName.ToString();
			USkillTree::AddNotificationToQueue(notification);
		}
	}
}

void UObjectiveComponent::SetMarkedOnMap()
{
	if (!_bMarkedOnMap)
	{
		_bMarkedOnMap = true;
		DelegateObjectiveMarkedOnMap.Broadcast(this);

		if (ObjectiveType == EObjectiveType::Find)
		{
			FString notification = "Objective marked on map: " + ObjectiveName.ToString();
			USkillTree::AddNotificationToQueue(notification);
		}
	}
}

void UObjectiveComponent::OnCompleted()
{
	DelegateObjectiveCompleted.Broadcast(this);
	Billboard->SetVisibility(false);
	_bIsFound = true;

	FString notification = "Objective completed: " + ObjectiveName.ToString();
	USkillTree::AddNotificationToQueue(notification);
}