// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/CapsuleComponent.h"
#include "Public/SkillTree.h"
#include "InteractionZone.generated.h"

UENUM(BlueprintType)
enum class EInteractionZoneInputType : uint8
{
	ACTION			UMETA(DisplayName = "Action"),
	SILENTKILL		UMETA(DisplayName = "SilentKill"),
};

class APlayerCharacter;

USTRUCT()
struct FInteractionZoneExecutionCall
{
	GENERATED_BODY()

	APlayerCharacter* Player;
	float Timer;

	FInteractionZoneExecutionCall() :
		Player(NULL),
		Timer(0.0f)
	{

	}

	FInteractionZoneExecutionCall(APlayerCharacter* playa, float tima) :
		Player(playa),
		Timer(tima)
	{

	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionZoneExecInteraction, APlayerCharacter*, PlayerCharacter);

/**
 * 
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class THEWHITEDEATH_API UInteractionZone : public USceneComponent
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(Category = "Capsule", EditAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* CapsuleComponent;

	UPROPERTY(Category = "InteractionZone", EditAnywhere, BlueprintReadWrite)
	EInteractionZoneInputType InputType = EInteractionZoneInputType::ACTION;

	UPROPERTY(Category = "InteractionZone", EditAnywhere, BlueprintReadWrite)
	float RepeatMinDelaySeconds = 1.0f;

	UPROPERTY(Category = "InteractionZone", EditAnywhere, BlueprintReadWrite)
	bool Repeatable = false;

	TArray<FInteractionZoneExecutionCall> _execCalls;
	TArray<APlayerCharacter*> _players;
	TArray<UActorComponent*> _components;

	UFUNCTION()
	void RegisterOnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void UnregisterOnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void RegisterToInputCall(APlayerCharacter* pc);
	void UnregisterFromInputCall(APlayerCharacter* pc);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractionZone")
	void Interaction(APlayerCharacter* pc);
	virtual void Interaction_Implementation(APlayerCharacter* pc);

public:

	UPROPERTY(BlueprintAssignable, Category = "InteractionZone")
	FInteractionZoneExecInteraction DelegateExecuteInteraction;

	UPROPERTY(Category = "InteractionZone", EditAnywhere, BlueprintReadWrite)
	FString ObjectName;

	UPROPERTY(Category = "InteractionZone", EditAnywhere, BlueprintReadWrite)
	bool ShowNotification = true;

	UInteractionZone(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	virtual bool CanExecuteInteraction(APlayerCharacter * pc);

	void ExecuteInteraction(APlayerCharacter* pc, float delay);

	void SetCapsuleSize(float InRadius, float InHalfHeight, bool bUpdateOverlaps);
};
