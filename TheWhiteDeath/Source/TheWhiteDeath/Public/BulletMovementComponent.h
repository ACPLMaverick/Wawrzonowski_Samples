// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/ProjectileMovementComponent.h"
#include "BulletMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class THEWHITEDEATH_API UBulletMovementComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()
	
protected:

	/**
	* Given an initial velocity and a time step, compute a new velocity.
	* Default implementation applies the result of ComputeAcceleration() to velocity.
	*
	* @param  InitialVelocity Initial velocity.
	* @param  DeltaTime Time step of the update.
	* @return Velocity after DeltaTime time step.
	*/
	virtual FVector ComputeVelocity(FVector InitialVelocity, float DeltaTime) const;

public:

};
