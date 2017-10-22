// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"
#include "ObjectiveComponent.generated.h"

UENUM(BlueprintType)
enum class EObjectiveType : uint8
{
	Destroy			UMETA(DisplayName = "Destroy"),

	Collect			UMETA(DisplayName = "Collect"),

	//* Must be visually seen and marked on map. */
	Find			UMETA(DisplayName = "Find")
};

UENUM(BlueprintType)
enum class EObjectiveImportance : uint8
{
	Primary			UMETA(DisplayName = "Primary"),
	Secondary		UMETA(DisplayName = "Secondary")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FObjectiveCompleted, UObjectiveComponent*, ComponentFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FObjectiveMoved, UObjectiveComponent*, ComponentFinished, FVector, NewLocation);

UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEWHITEDEATH_API UObjectiveComponent : public USceneComponent
{
	GENERATED_BODY()

protected:

	FVector _lastActorLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	FText ObjectiveName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	EObjectiveType ObjectiveType;

	/* Beware! If objective type is FIND, this parameter will be ignored and objective will always be invisible until found. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	bool bInitiallyVisibleOnMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	EObjectiveImportance ObjectiveImportance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	class UBillboardComponent* Billboard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	class UTexture2D* TexturePrimary;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	class UTexture2D* TextureSecondary;

	bool _bMarkedVisible = false;
	bool _bMarkedOnMap = false;
	bool _bIsFound = false;

	virtual void OnCompleted();

public:

	UPROPERTY(BlueprintAssignable, Category = "Objective")
	FObjectiveCompleted DelegateObjectiveCompleted;
	UPROPERTY(BlueprintAssignable, Category = "Objective")
	FObjectiveMoved DelegateObjectiveMoved;
	UPROPERTY(BlueprintAssignable, Category = "Objective")
	FObjectiveCompleted DelegateObjectiveMarkedOnMap;
	UPROPERTY(BlueprintAssignable, Category = "Objective")
	FObjectiveCompleted DelegateObjectiveMarkedVisible;

	// Sets default values for this component's properties
	UObjectiveComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	EObjectiveType GetObjectiveType() const { return ObjectiveType; }
	EObjectiveImportance GetObjectiveImportance() const { return ObjectiveImportance; }
	const FText& GetObjectiveName() const { return ObjectiveName; }
	bool GetMarkedVisible() const { return _bMarkedVisible; }
	bool GetMarkedOnMap() const { return _bMarkedOnMap; }
	bool GetInitiallyVisible() const { return bInitiallyVisibleOnMap; }

	virtual void SetMarkedVisible();
	virtual void SetMarkedOnMap();
};
