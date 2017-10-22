// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Actor.h"
#include "TheWhiteDeathProjectile.generated.h"

class AGun;
class UBulletMovementComponent;

UCLASS(config=Game)
class ATheWhiteDeathProjectile : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(Category = "Projectile", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Damage;

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	class USphereComponent* CollisionComp;

	UPROPERTY(Category = "Projectile", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsInfluencedByWind = true;

	UPROPERTY(Category = "Projectile", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float WindInfluenceMultiplier = 1.0f;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UBulletMovementComponent* ProjectileMovement;

	AGun* _gun;

public:
	ATheWhiteDeathProjectile();

	virtual void Tick(float deltaTime);

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UBulletMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	float GetDamage() { return Damage; }
	void SetOwningGun(AGun* gun) { _gun = gun; }
	AGun* GetOwningGun() { return _gun; }
	bool GetInfluencedByWind() { return bIsInfluencedByWind; }
	float GetWindInfluenceMultiplier() { return WindInfluenceMultiplier; }
};

