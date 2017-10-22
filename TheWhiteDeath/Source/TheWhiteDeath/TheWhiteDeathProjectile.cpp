// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TheWhiteDeath.h"
#include "TheWhiteDeathProjectile.h"
#include "TheWhiteDeathCharacter.h"
#include "Public/Gun.h"
#include "Public/HitBox.h"
#include "Public/BulletMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ATheWhiteDeathProjectile::ATheWhiteDeathProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ATheWhiteDeathProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UBulletMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	PrimaryActorTick.bCanEverTick = true;
}

void ATheWhiteDeathProjectile::Tick(float deltaTime)
{

}

void ATheWhiteDeathProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Destroy on hit
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		bIsInfluencedByWind = false;

		if (OtherComp->IsA<UHitBoxComponent>())
		{
			Destroy();
		}
	}

	//DrawDebugSphere(GetWorld(), GetActorLocation(), 10.0f, 16, FColor::Red, true, 60.0f, 0x00, 10.0f);
}