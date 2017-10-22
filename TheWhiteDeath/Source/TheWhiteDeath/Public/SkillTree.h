// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Public/SkillTreeWidget.h"
#include "SkillTree.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEWHITEDEATH_API USkillTree : public UActorComponent
{
	GENERATED_BODY()

protected:

	// The instance of the skill tree notifications
	UPROPERTY(Category = "User Interface", EditDefaultsOnly, BlueprintReadOnly)
	class USkillTreeWidget* Notifications;

	// The instance of the skill tree main screen
	UPROPERTY(Category = "User Interface", EditDefaultsOnly, BlueprintReadOnly)
	class USkillTreeWidget* MainScreen;

	static TMap<FString, int32> ExperienceEventMap;
	static TArray<FString> NotificationsQueue;

	const float _timeBetweenNotifications = 1.0f;
	static const int32 _xpToNewLevelStep = 50;

	float _hideNotificationTime = 0.0f;

#pragma region Protected Functions

	void FillMap();
	void PlayNotificationQueue(float dt);
	static void LevelUp();
	void UpdateMainWidget();
 
#pragma endregion

public:	

	// The class that will be used for skill tree notifications
	UPROPERTY(Category = "User Interface", EditAnywhere, BlueprintReadOnly)
	TSubclassOf<USkillTreeWidget> NotificationsClass;

	// The class that will be used for skill tree main screen
	UPROPERTY(Category = "User Interface", EditAnywhere, BlueprintReadOnly)
	TSubclassOf<USkillTreeWidget> MainScreenClass;

	// Array of skill levels player currently have
	UPROPERTY(Category = "Skill Tree", EditAnywhere, BlueprintReadWrite)
	TArray<int32> CurrentSkillsLevels;

	// Array of maximum skill levels
	UPROPERTY(Category = "Skill Tree", EditDefaultsOnly, BlueprintReadOnly)
	TArray<int32> MaxSkillsLevels;

	// Check if there are some skill choices to accept
	UPROPERTY(Category = "Skill Tree", EditAnywhere, BlueprintReadWrite)
	bool AreThereSkillsToAccept;

	UPROPERTY(Category = "Skill Tree", EditAnywhere, BlueprintReadWrite)
	bool InSkillTreeMenu;
	
	static int32 CurrentLevel;
	static int32 XPCount;
	static int32 XPToNextLevel;
	static int32 TokensCount;

	// Sets default values for this component's properties
	USkillTree();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	// Grant player with experience points - TODO: Add struct with type of granting possibilities
	static void GrantExperiencePoints(FString e);

	static void AddNotificationToQueue(FString e);

	void ToggleSkillTreeMenu();

	void ShowAcceptDialog();

	//UFUNCTION(BlueprintCallable, Category="Skill Tree")
	bool OnBuySkillPress(int32 id);

	void InvokeHoveredSkillMoveLeft();
	void InvokeHoveredSkillMoveUp();
	void InvokeHoveredSkillMoveRight();
	void InvokeHoveredSkillMoveDown();

	void ResetSkillTree();

	bool HasSkill(int32 id);
};
