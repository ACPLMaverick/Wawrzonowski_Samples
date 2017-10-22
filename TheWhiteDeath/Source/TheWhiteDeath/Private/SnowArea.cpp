// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "ParticleDefinitions.h"
#include "Components/BoxComponent.h"
#include "Public/PlayerCharacter.h"
#include "SnowArea.h"


// Sets default values
ASnowArea::ASnowArea(const class FObjectInitializer& PCIP) :
	Super(PCIP)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Extents = PCIP.CreateDefaultSubobject<UBoxComponent>(this, TEXT("Extents"));
	Extents->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = Extents;

	SnowFX = PCIP.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("Snow"));

	//Attach PSC to socket
	SnowFX->SetupAttachment(RootComponent);

	SnowFX->bAutoDestroy = false;
	SnowFX->bAutoActivate = true;
}

// Called when the game starts or when spawned
void ASnowArea::BeginPlay()
{
	Super::BeginPlay();
	_snowBounds = FVector2D(SnowFX->Bounds.BoxExtent.X, SnowFX->Bounds.BoxExtent.Y);

	int32 xAmount, yAmount;
	xAmount = FMath::RoundToInt(Extents->GetScaledBoxExtent().X / _snowBounds.X);
	yAmount = FMath::RoundToInt(Extents->GetScaledBoxExtent().Y / _snowBounds.Y);

	// create particles
	_snowParticleSystems.Add(SnowFX);
	for (int i = 0; i < xAmount * yAmount - 1; ++i)
	{
		FString str("SnowParticles" + FString::FromInt(i));
		FName name;
		name.AppendString(str);
		UParticleSystemComponent* particles = DuplicateObject<UParticleSystemComponent>(SnowFX, this, name);
		particles->AttachToComponent(Extents, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), NAME_None);
		_snowParticleSystems.Add(particles);
	}

	// setup particles
	FVector baseUnit(_snowBounds.X, _snowBounds.Y, 0.0f);
	int32 index = 0;
	for (int32 i = -xAmount / 2; i < (xAmount / 2 + xAmount % 2); ++i)
	{
		for (int32 j = -yAmount / 2; j < (yAmount / 2 + yAmount % 2); ++j, ++index)
		{
			FVector location(baseUnit.X * (float)i, baseUnit.Y * (float)j, 10000.0f);

			FHitResult hitResult;
			GetWorld()->LineTraceSingleByChannel(hitResult, location, FVector(location.X, location.Y, -location.Z), ECollisionChannel::ECC_WorldStatic);
			location.Z = SnowHeight;

			if (hitResult.bBlockingHit)
			{
				location.Z += hitResult.Location.Z;
			}

			_snowParticleSystems[index]->SetWorldLocation(location);
			_snowParticleSystems[index]->SetVisibility(true);
			_snowParticleSystems[index]->Activate();
			_snowParticleSystems[index]->ActivateSystem(true);
		}
	}
}

// Called every frame
void ASnowArea::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (ReferenceCharacter != NULL)
	{
		int32 partNum = _snowParticleSystems.Num();
		for (int32 i = 0; i < partNum; ++i)
		{
			float distance = FMath::Sqrt(FVector::DistSquaredXY(ReferenceCharacter->GetActorLocation(), _snowParticleSystems[i]->GetComponentLocation()));
			if (distance <= SnowViewDistance && !_snowParticleSystems[i]->IsVisible())
			{
				_snowParticleSystems[i]->SetVisibility(true);
			}
			//else if (distance > SnowViewDistance && _snowParticleSystems[i]->IsVisible())
			//{
			//	_snowParticleSystems[i]->SetVisibility(false);
			//}
		}
	}
}

