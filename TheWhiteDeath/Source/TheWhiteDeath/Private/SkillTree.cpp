// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "SkillTree.h"
#include "Engine.h"

TMap<FString, int32> USkillTree::ExperienceEventMap;
TArray<FString> USkillTree::NotificationsQueue;

int32 USkillTree::CurrentLevel;
int32 USkillTree::XPCount;
int32 USkillTree::XPToNextLevel;
int32 USkillTree::TokensCount;

void USkillTree::FillMap()
{
	ExperienceEventMap.Add("", 0);
	ExperienceEventMap.Add("Aim Taken", 50);
	ExperienceEventMap.Add("Kill Enemy", 100);
	ExperienceEventMap.Add("Silent Kill", 50);
}

void USkillTree::PlayNotificationQueue(float dt)
{
	if (NotificationsQueue.Num() > 0)
	{
		if (!Notifications->IsNotificationAnimationPlaying)
		{
			Notifications->PlayShowNotification(FName(*NotificationsQueue.HeapTop()));
			Notifications->IsNotificationAnimationPlaying = true;
		}

		if (_hideNotificationTime < _timeBetweenNotifications)
		{
			_hideNotificationTime += dt;
		}
		else
		{
			NotificationsQueue.HeapPopDiscard();
			_hideNotificationTime = 0.0f;
			Notifications->IsNotificationAnimationPlaying = false;
		}
	}
}

void USkillTree::LevelUp()
{
	XPToNextLevel += _xpToNewLevelStep * CurrentLevel;
	XPCount = 0;
	CurrentLevel++;
	TokensCount++;

	NotificationsQueue.HeapPush("LEVEL UP!");
}

void USkillTree::UpdateMainWidget()
{
	MainScreen->CurrentLevel = CurrentLevel;
	MainScreen->XPCount = XPCount;
	MainScreen->XPToNextLevel = XPToNextLevel;
	MainScreen->TokenCount = TokensCount;
}

// Sets default values for this component's properties
USkillTree::USkillTree()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	CurrentLevel = 1;
	XPCount = 0;
	XPToNextLevel = 100;
	TokensCount = 0;

	FillMap();
}


// Called when the game starts
void USkillTree::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	if (NotificationsClass) // Check the selected UI class is not NULL
	{
		Notifications = CreateWidget<USkillTreeWidget>(GetWorld(), NotificationsClass); // Create Widget
		if (!Notifications)
			return;
		Notifications->AddToViewport(); // Add it to the viewport so the Construct() method in the UUserWidget:: is run.
	}

	if (MainScreenClass) // Check the selected UI class is not NULL
	{
		MainScreen = CreateWidget<USkillTreeWidget>(GetWorld(), MainScreenClass); // Create Widget
		if (!MainScreen)
			return;
		MainScreen->AddToViewport(); // Add it to the viewport so the Construct() method in the UUserWidget:: is run.
	}
}


// Called every frame
void USkillTree::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...

	UpdateMainWidget();
	PlayNotificationQueue(DeltaTime);
}


void USkillTree::GrantExperiencePoints(FString e)
{
	NotificationsQueue.HeapPush(e + ": " + FString::FromInt(ExperienceEventMap[e]) + " XP");

	XPCount += ExperienceEventMap[e];

	if (XPCount >= XPToNextLevel)
	{
		LevelUp();
	}
	//Notifications->PlayShowNotification();
}

void USkillTree::AddNotificationToQueue(FString e)
{
	NotificationsQueue.HeapPush(e);
}

void USkillTree::ToggleSkillTreeMenu()
{
	if (InSkillTreeMenu)
	{
		MainScreen->PlayHideMenu();
		InSkillTreeMenu = false;
	}
	else
	{
		MainScreen->PlayShowMenu();
		InSkillTreeMenu = true;
	}
}

void USkillTree::ShowAcceptDialog()
{
	MainScreen->PlayShowAcceptDialog();
}

void USkillTree::ResetSkillTree()
{
	CurrentLevel = 1;
	XPCount = 0;
	XPToNextLevel = 100;
	TokensCount = 0;

	AreThereSkillsToAccept = false;

	for (int32 i = 1; i < CurrentSkillsLevels.Num(); ++i)
	{
		CurrentSkillsLevels[i] = 0;
	}

	MainScreen->ResetSkillTree();
}

bool USkillTree::HasSkill(int32 id)
{
	if (CurrentSkillsLevels.Num() > id && CurrentSkillsLevels[id] > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool USkillTree::OnBuySkillPress(int id)
{
	TokensCount--;
	CurrentSkillsLevels[id]++;
	if (CurrentSkillsLevels[id] == MaxSkillsLevels[id])
	{
		return true;
	}
	return false;
}

void USkillTree::InvokeHoveredSkillMoveLeft() { MainScreen->HoveredSkillMoveLeft(); }
void USkillTree::InvokeHoveredSkillMoveUp() { MainScreen->HoveredSkillMoveUp(); }
void USkillTree::InvokeHoveredSkillMoveRight() { MainScreen->HoveredSkillMoveRight(); }
void USkillTree::InvokeHoveredSkillMoveDown() { MainScreen->HoveredSkillMoveDown(); }

