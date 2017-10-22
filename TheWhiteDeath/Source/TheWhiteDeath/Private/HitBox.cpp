// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "HitBox.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Components/MeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TheWhiteDeathProjectile.h"
#include "TheWhiteDeathCharacter.h"
#include "Public/Gun.h"
#include "ParticleDefinitions.h"

// Sets default values
AHitBox::AHitBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	Collider->Bounds.BoxExtent = FVector(32.0f, 32.0f, 32.0f);
	Collider->SetCollisionProfileName(TEXT("OverlapAll"));
	Collider->SetSimulatePhysics(false);
	Collider->OnComponentHit.AddDynamic(this, &AHitBox::OnHit);
	RootComponent = Collider;

	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	Decal->SetupAttachment(RootComponent);
	Decal->DecalSize = FVector(1.0f, 1.0f, 1.0f);

	Particles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particles"));
	Particles->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AHitBox::BeginPlay()
{
	Super::BeginPlay();
	Particles->DeactivateSystem();
	if (GetParentActor() != NULL)
	{
		_mesh = Cast<UMeshComponent>(GetParentActor()->GetComponentByClass(UMeshComponent::StaticClass()));
		_skMesh = Cast<USkeletalMeshComponent>(GetParentActor()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	}
}

// Called every frame
void AHitBox::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	TickComponentsToDestroy(DeltaTime);
}

void AHitBox::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->IsA<ATheWhiteDeathProjectile>())
	{
		OnHitByProjectile(Cast<ATheWhiteDeathProjectile>(OtherActor), OtherComp, NormalImpulse, Hit);
	}
}

void AHitBox::TickComponentsToDestroy(float deltaTime)
{
	for (int i = 0; i < _componentsToDestroy.Num(); ++i)
	{
		_componentsToDestroy[i].Timer += deltaTime;

		if (_componentsToDestroy[i].Timer >= _componentsToDestroy[i].TimeToDestroy)
		{
			if (_componentsToDestroy[i].Component != NULL && !_componentsToDestroy[i].Component->IsBeingDestroyed())
			{
				_componentsToDestroy[i].Component->DestroyComponent();
			}
			_componentsToDestroy.RemoveAt(i, 1, false);
		}
	}
}

void AHitBox::OnHitByProjectile(ATheWhiteDeathProjectile * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	// apply damage if actor is whitedeathcharacter
	AGun* gun = OtherActor->GetOwningGun();
	FVector dir(0.0f, 0.0f, 1.0f);
	ATheWhiteDeathCharacter* owner = NULL;
	if (gun != NULL)
	{
		dir = GetActorLocation() - gun->GetActorLocation();
		dir = dir.GetSafeNormal();
		owner = Cast<ATheWhiteDeathCharacter>(GetParentActor());
		if (owner != NULL)
		{
			// set damage
			float totalDamage = (OtherActor->GetDamage() + gun->GetDamageBase()) * DamageMultiplier;
			owner->OnHit(totalDamage, dir); 
		}
	}
	// calculate hit position on mesh (if available)
	FVector hitPosition = Hit.Location;
	FVector hitOnBody = Hit.Location;

	if (_skMesh != NULL)
	{
		_skMesh->GetClosestPointOnCollision(hitPosition, hitOnBody);
	}
	else if (_mesh != NULL)
	{
		_mesh->GetClosestPointOnCollision(hitPosition, hitOnBody);
	}
	hitPosition = hitOnBody;

	// add hit decal
	if (Decal != NULL)
	{
		AActor* def = GetParentActor() != NULL ? GetParentActor() : this;
		UDecalComponent* SpawnedDecal = NewObject<UDecalComponent>(def->GetRootComponent(), NAME_None, RF_NoFlags, Decal);
		if (SpawnedDecal)
		{
			_componentsToDestroy.Add(FHitBoxDestructionPair(SpawnedDecal, DecalLifeSeconds));
			SpawnedDecal->RegisterComponent();
			SpawnedDecal->SetWorldLocation(hitPosition);
			SpawnedDecal->SetWorldScale3D(FVector(DecalSize));
			SpawnedDecal->SetFadeOut(DecalLifeSeconds * 0.75f, DecalLifeSeconds * 0.25f, false);
			SpawnedDecal->Activate();
		}
	}

	// fire particle
	if (Particles != NULL)
	{
		UParticleSystemComponent* SpawnedParticles = NewObject<UParticleSystemComponent>(RootComponent, NAME_None, RF_NoFlags, Particles);
		if (SpawnedParticles)
		{
			_componentsToDestroy.Add(FHitBoxDestructionPair(SpawnedParticles, 3.0f));
			SpawnedParticles->RegisterComponent();
			SpawnedParticles->SetWorldLocation(hitPosition);
			SpawnedParticles->Activate();
			SpawnedParticles->ActivateSystem();
		}
	}

	// fire sound
	if (Sound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation());
	}
}
