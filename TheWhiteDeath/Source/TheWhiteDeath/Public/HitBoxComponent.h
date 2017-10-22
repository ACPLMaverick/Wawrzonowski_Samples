// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HitBoxComponent.generated.h"

class UBoxComponent;
class UDecalComponent;
class USoundBase;
class UParticleSystemComponent;
class ATheWhiteDeathProjectile;
class UMeshComponent;
class USkeletalMeshComponent;

USTRUCT()
struct THEWHITEDEATH_API FHitBoxDestructionPair
{
	GENERATED_BODY()

		UActorComponent* Component;
	float TimeToDestroy;
	float Timer;

	FHitBoxDestructionPair() :
		Component(NULL),
		TimeToDestroy(0.0f),
		Timer(0.0f)
	{
	}

	FHitBoxDestructionPair(UActorComponent* comp, float timeToDestroy) :
		Component(comp),
		TimeToDestroy(timeToDestroy),
		Timer(0.0f)
	{
	}
};

UCLASS(BlueprintType, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class THEWHITEDEATH_API UHitBoxComponent : public UBoxComponent
{
	GENERATED_BODY()

public:

#pragma region Properties

	//** Main collider for the hit zone. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HitBox", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* Collider;

	//** Decal to show on hit surface. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HitBox", meta = (AllowPrivateAccess = "true"))
		class UDecalComponent* Decal;

	//** Sound to play when hit. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox", meta = (AllowPrivateAccess = "true"))
		class USoundBase* Sound;

	//** Particles to show in hit position. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HitBox", meta = (AllowPrivateAccess = "true"))
		class UParticleSystemComponent* Particles;

	//** Describes how much this hitbox influences the target's health. Should be a value equal or less than one. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox", meta = (AllowPrivateAccess = "true"))
		float DamageMultiplier;

	//** Describes long it takes for the hit decal to disappear. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox", meta = (AllowPrivateAccess = "true"))
		float DecalLifeSeconds;

	//** Describes long it takes for the hit decal to disappear. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox", meta = (AllowPrivateAccess = "true"))
		float DecalSize = 1.0f;

#pragma endregion

protected:

#pragma region Protected

	UMeshComponent* _mesh;
	USkeletalMeshComponent* _skMesh;

	TArray<FHitBoxDestructionPair> _componentsToDestroy;

#pragma endregion

#pragma region Functions Protected

	void TickComponentsToDestroy(float deltaTime);

	virtual void OnHitByProjectile(ATheWhiteDeathProjectile* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

#pragma endregion

public:
	// Sets default values for this actor's properties
	UHitBoxComponent();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
