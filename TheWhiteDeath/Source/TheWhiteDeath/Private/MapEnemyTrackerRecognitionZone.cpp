// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "MapEnemyTrackerRecognitionZone.h"
#include "Public/EnemyCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/BillboardComponent.h"
#include "Engine/Texture2D.h"

// Sets default values
AMapEnemyTrackerRecognitionZone::AMapEnemyTrackerRecognitionZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TrackerWorldIcon = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	TrackerWorldIcon->SetVisibility(false);
	TrackerWorldIcon->SetHiddenInGame(true);
	RootComponent = TrackerWorldIcon;

	RecoSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RecoSphere"));
	RecoSphere->SetSphereRadius(880.0f);
	RecoSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	RecoSphere->SetupAttachment(TrackerWorldIcon);
}

void AMapEnemyTrackerRecognitionZone::RegisterMapTracker(UMapIconEnemyTracker * tracker)
{
	_mapTracker = tracker;
	const TArray<UTexture2D*>& texs = _mapTracker->GetDetectionStateTextures();
	const int32 texsNum = texs.Num();
	for (int32 i = 0; i < texsNum; ++i)
	{
		_detectionStateTextures.Add(texs[i]);
	}
}

void AMapEnemyTrackerRecognitionZone::UpdateDetectionState(EPlayerDetectionState detection)
{
	if (_detectionStateTextures[(int32)detection] != NULL)
	{
		TrackerWorldIcon->SetSprite(_detectionStateTextures[(int32)detection]);
	}
}

// Called when the game starts or when spawned
void AMapEnemyTrackerRecognitionZone::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMapEnemyTrackerRecognitionZone::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (!_firstTick)
	{
		TSet<AActor*> eSet;
		GetOverlappingActors(eSet, AEnemyCharacter::StaticClass());
		const int32 eSetNum = eSet.Num();
		if (eSetNum > 0)
		{
			_enemy = Cast<AEnemyCharacter>(eSet.Array()[0]);
			float closestDist = GetHorizontalDistanceTo(_enemy);
			for (int32 i = 1; i < eSetNum; ++i)
			{
				float dist = GetHorizontalDistanceTo(eSet.Array()[i]);
				if (dist < closestDist)
				{
					_enemy = Cast<AEnemyCharacter>(eSet.Array()[i]);
					closestDist = dist;
				}
			}

			DelegateEnemyFound.Broadcast(_enemy);
			EnableWorldTracker();
		}
		else
		{
			RecoSphere->OnComponentBeginOverlap.AddDynamic(this, &AMapEnemyTrackerRecognitionZone::OnSphereBeginOverlap);
		}

		_firstTick = true;
	}
}

void AMapEnemyTrackerRecognitionZone::EnableWorldTracker()
{
	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, false);
	AttachToActor(_enemy, rules);

	_enemy->OnDestroyed.RemoveDynamic(this, &AMapEnemyTrackerRecognitionZone::OnEnemyDestroyed);
	_enemy->OnDestroyed.AddDynamic(this, &AMapEnemyTrackerRecognitionZone::OnEnemyDestroyed);

	FVector eWorldPos = _enemy->GetActorLocation();
	eWorldPos.Z += _enemy->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 30.0f;
	SetActorLocation(eWorldPos);

	TrackerWorldIcon->SetVisibility(true);
	TrackerWorldIcon->SetHiddenInGame(false);
	RecoSphere->DestroyComponent();
}

void AMapEnemyTrackerRecognitionZone::OnSphereBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if ((_enemy = Cast<AEnemyCharacter>(OtherActor)) != NULL)
	{
		DelegateEnemyFound.Broadcast(_enemy);
		EnableWorldTracker();
	}
}

void AMapEnemyTrackerRecognitionZone::OnEnemyDestroyed(AActor * DestroyedActor)
{
	Destroy();
}
