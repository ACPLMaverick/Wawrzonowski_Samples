// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "MapComponent.generated.h"

class UMapWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEWHITEDEATH_API UMapComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(Category = "Map|Objectives", EditAnywhere, BlueprintReadWrite)
	TArray<class AActor*> ObjectiveActors;

	TArray<class UObjectiveComponent*> Objectives;

	// The instance of the skill tree main screen
	class UMapWidget* MainScreen;

	// Marking menu screen
	class UMapIconMenuWidget* MarkMenuScreen;

	// The class that will be used for main map screen
	UPROPERTY(Category = "Map|User Interface", EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMapWidget> MainScreenClass;

	// The class that will be used for world marking menu
	UPROPERTY(Category = "Map|User Interface", EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMapIconMenuWidget> MarkMenuClass;

	UPROPERTY(Category = "Map|Properties", EditAnywhere, BlueprintReadWrite)
	float SideMoveSpeed = 0.5f;

	UPROPERTY(Category = "Map|Properties", EditAnywhere, BlueprintReadWrite)
	float SideMoveBorderWidth = 50.0f;

	UPROPERTY(Category = "Map|Properties", EditAnywhere, BlueprintReadWrite)
	float ZoomRatio = 0.5f;

	UPROPERTY(Category = "Map|Properties", EditAnywhere, BlueprintReadWrite)
	float MaxZoom = 2.0f;

	UPROPERTY(Category = "Map|Properties", EditAnywhere, BlueprintReadWrite)
	int BaseScale = 10;

	int32 _activeObjectivesFindNum = 0;
	int32 _activeObjectivesDestroyNum = 0;
	int32 _activeObjectivesCollectNum = 0;

	void MarkFromWorld(class UMapIconImage* imageToMark);

	void EnableFocus(const TSharedPtr<SWidget>& widget);
	void DisableFocus();

	UFUNCTION()
	void OnObjectiveCompleted(class UObjectiveComponent* obj);
	UFUNCTION()
	void OnObjectiveMarkedVisible(class UObjectiveComponent* obj);
	UFUNCTION()
	void OnObjectiveMarkedOnMap(class UObjectiveComponent* obj);
	UFUNCTION()
	void OnObjectiveMoved(class UObjectiveComponent* obj, FVector location);

	void GpadReturn();

public:	
	// Sets default values for this component's properties
	UMapComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	const TArray<class UObjectiveComponent*>& GetObjectives() { return Objectives; }
	int32 GetActiveObjectivesFindNum() { return _activeObjectivesFindNum; }
	int32 GetActiveObjectivesDestroyNum() { return _activeObjectivesDestroyNum; }
	int32 GetActiveObjectivesCollectNum() { return _activeObjectivesCollectNum; }

	void ToggleVisible();
	bool IsVisible();

	void SetMarkMenuOn();
	void SetMarkMenuOff();
};
