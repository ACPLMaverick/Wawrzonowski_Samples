// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "BulletMovementComponent.h"
#include "TheWhiteDeathProjectile.h"
#include "Public/GameController.h"

FVector UBulletMovementComponent::ComputeVelocity(FVector InitialVelocity, float DeltaTime) const
{
	FVector velocity = Super::ComputeVelocity(InitialVelocity, DeltaTime);

	if (GetOwner() != NULL && AGameController::GetInstance() != NULL && GetOwner()->IsA<ATheWhiteDeathProjectile>())
	{
		ATheWhiteDeathProjectile* projectile = Cast<ATheWhiteDeathProjectile>(GetOwner());
		if (projectile->GetInfluencedByWind())
		{
			velocity += AGameController::GetInstance()->GetWindVector() * projectile->GetWindInfluenceMultiplier() * DeltaTime;
		}
	}

	return velocity;
}