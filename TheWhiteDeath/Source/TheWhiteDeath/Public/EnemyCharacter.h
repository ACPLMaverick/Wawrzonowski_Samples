// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TheWhiteDeathCharacter.h"
#include "GenericTeamAgentInterface.h"
#include "EnemyCharacter.generated.h"

class UStaticMeshComponent;
class UAIPerceptionComponent;
class APlayerCharacter;
class USilentKillInteractionZone;

UENUM(BlueprintType)
enum class EPlayerDetectionState : uint8
{
	Unaware			UMETA(DisplayName = "Unaware"),
	Suspicious		UMETA(DisplayName = "Suspicious"),
	Aware			UMETA(DisplayName = "Aware")
};

UENUM(BlueprintType)
enum class EBehaviourTreeState : uint8
{
	Idle			UMETA(DisplayName = "Idle"),
	Patroling		UMETA(DisplayName = "Patroling"),
	Suspicious		UMETA(DisplayName = "Suspicious"),
	Searching		UMETA(DisplayName = "Searching"),
	Aggresive		UMETA(DisplayName = "Aggresive"),
};

/**
 * 
 */
UCLASS()
class THEWHITEDEATH_API AEnemyCharacter : public ATheWhiteDeathCharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Perception")
	class UAIPerceptionComponent* PerceptionComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Perception")
	class UStaticMeshComponent* ViewCone;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "SilentKill")
	class USilentKillInteractionZone* SilentKillZone;

	/** AnimMontage to play when enemy is silently killed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* SilentKillDieAnimation;

	/** AnimMontages to play when we take damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TArray<class UAnimMontage*> DamageTakingAnimations;


	FVector _damageDelayedDirection;
	EPlayerDetectionState _detectionState;
	APlayerCharacter* _detectedPlayer = NULL;
	float _damageDelayed = 0.0f;
	float _damageDelayedTimer = 0.0f;
	float _releaseFireTimer = 0.0f;
	bool _bDamageDelayed = false;
	bool _bTriggerPressed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	bool IsPlayerDetected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviourTree")
	bool IsBTStopped;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviourTree")
	EBehaviourTreeState BTState;

	virtual void Die() override;

	UFUNCTION()
	virtual void ProcessSense(TArray<AActor*> sensedActors);

	UFUNCTION()
	virtual void OnSilentKill(APlayerCharacter* pc);

public:

	AEnemyCharacter(const class FObjectInitializer& PCIP);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Character")
	bool HasAIController = true;

	virtual void BeginPlay() override;

	virtual void Tick(float deltaTime) override;

	void TranslateBTState();

	EPlayerDetectionState GetDetectionState() { return _detectionState; }

	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const { return 2; }

	virtual void SetViewConeVisible();
	virtual void SetViewConeHidden();

	UFUNCTION(Category = "Enemy Character", BlueprintCallable)
	virtual void AttackEnemy();

	UFUNCTION(Category = "Enemy Character", BlueprintCallable)
	virtual void OnReload() override;

	UFUNCTION(Category = "Enemy Character", BlueprintCallable)
	void StopImmediate();

	UFUNCTION(Category = "Enemy Character", BlueprintCallable)
	void ReleaseStopImmediate();

	UFUNCTION(Category = "Enemy Character", BlueprintCallable)
	void DealDamageFromGrenade(float dmgAmount);

	virtual void OnHit(float DmgAmount, FVector Direction) override;

	UFUNCTION(Category = "Enemy Character", BlueprintCallable)
	bool GetIsPlayerDetected() { return IsPlayerDetected; }

	UFUNCTION(Category = "Enemy Character", BlueprintCallable)
	void SetIsPlayerDetected(bool isDetected) { IsPlayerDetected = isDetected; }
};
