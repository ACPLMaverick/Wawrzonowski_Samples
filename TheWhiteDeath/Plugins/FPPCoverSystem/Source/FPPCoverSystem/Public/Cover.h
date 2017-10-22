// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Cover.generated.h"

class ACoverCharacter;
class UBoxComponent;

UCLASS()
class ACover : public AActor
{
	GENERATED_BODY()

protected:

#pragma region Properties

	//** External collider inside which cover will start to influence player controller. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* ExternalCollider;

	//** Internal collider inside which cover will have 100% influence of controller */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* InternalCollider;

	//** Direction in which the player can emerge from the cover. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover", meta = (AllowPrivateAccess = "true"))
	FVector CoverEmergeDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover", meta = (AllowPrivateAccess = "true"))
	bool NeedToCrouch = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover", meta = (AllowPrivateAccess = "true"))
	bool CanVaultOver = true;

#pragma endregion

#pragma region Protected

	TArray<ACoverCharacter*> _charactersInCoverArea;
	FVector _boundLeftWorld;
	FVector _boundRightWorld;
	FVector _boundRightRearWorld;
	FVector _boundUpWorld;

#pragma endregion

#pragma region Functions Protected

#pragma endregion
	
public:	

#pragma region Functions Public

	// Sets default values for this actor's properties
	ACover();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION()
	virtual void OnInternalBeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult & sweepResult);
	UFUNCTION()
	virtual void OnExternalBeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult & sweepResult);
	UFUNCTION()
	virtual void OnInternalEndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex);
	UFUNCTION()
	virtual void OnExternalEndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex);

	FVector GetReflectedPositionAcrossCover(FVector currentPosition, FVector currentDirection);
	FVector GetCoverForward(FVector currentPosition);
	FVector GetCoverRight(FVector currentPosition);

#pragma region Accessors

	FVector GetCoverEmergeDirection() { return CoverEmergeDirection; }
	bool GetIfNeedToCrouch() { return NeedToCrouch; }
	bool GetCanVaultOver() { return CanVaultOver; }
	FVector GetBoundLeftWorld() { return _boundLeftWorld; }
	FVector GetBoundRightWorld() { return _boundRightWorld; }
	FVector GetBoundRightRearWorld() { return _boundRightRearWorld; }
	FVector GetBoundUpWorld() { return _boundUpWorld; }
	FVector GetBoundExtent() { return ExternalCollider->GetScaledBoxExtent(); }

#pragma endregion

#pragma endregion

};
