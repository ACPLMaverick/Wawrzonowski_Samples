// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Grenade.generated.h"

UCLASS()
class THEWHITEDEATH_API AGrenade : public AActor
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class UStaticMeshComponent* Mesh;

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Movement)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleDefaultsOnly, Category = Movement)
	class URadialForceComponent* RadialForce;

	bool _bIsGrenadeReady = false;
	bool _bStartTimer = false;
	float _pinTimer = 0.0f;
	float _timeToExplode = 5.0f;

public:	
	// Sets default values for this actor's properties
	AGrenade();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void TickPin(float DeltaSeconds);

	virtual void OnReady();
	virtual void OnRelease();

	// Explosion
	UFUNCTION(BlueprintNativeEvent)
	void Explode();
	virtual void Explode_Implementation();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void SetVelocity(FVector newVelocity) { ProjectileMovement->Velocity = ProjectileMovement->InitialSpeed * newVelocity; }

	bool GetIsGrenadeReady() { return _bIsGrenadeReady; }
};
