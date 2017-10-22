// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "MapComponent.h"
#include "Public/MapWidget.h"
#include "Public/MapIconMenuWidget.h"
#include "Public/PlayerCharacter.h"
#include "Public/GameController.h"
#include "Public/ObjectiveComponent.h"

// Sets default values for this component's properties
UMapComponent::UMapComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UMapComponent::BeginPlay()
{
	Super::BeginPlay();


	int32 oNum = ObjectiveActors.Num();
	for (int32 i = 0; i < oNum; ++i)
	{
		UObjectiveComponent* objective = Cast<UObjectiveComponent>(ObjectiveActors[i]->GetComponentByClass(UObjectiveComponent::StaticClass()));

		if (objective != NULL)
		{
			Objectives.Add(objective);

			objective->DelegateObjectiveCompleted.AddDynamic(this, &UMapComponent::OnObjectiveCompleted);
			objective->DelegateObjectiveMarkedVisible.AddDynamic(this, &UMapComponent::OnObjectiveMarkedVisible);
			objective->DelegateObjectiveMarkedOnMap.AddDynamic(this, &UMapComponent::OnObjectiveMarkedOnMap);
			objective->DelegateObjectiveMoved.AddDynamic(this, &UMapComponent::OnObjectiveMoved);

			if (objective->GetObjectiveType() == EObjectiveType::Destroy)
			{
				++_activeObjectivesDestroyNum;
			}
			else if (objective->GetObjectiveType() == EObjectiveType::Find)
			{
				++_activeObjectivesFindNum;
			}
			else if (objective->GetObjectiveType() == EObjectiveType::Collect)
			{
				++_activeObjectivesCollectNum;
			}
		}
	}

	APlayerCharacter* owner = Cast<APlayerCharacter>(GetOwner());
	if (owner != NULL && MainScreenClass != NULL)
	{
		AController* controller = owner->GetController();
		MainScreen = CreateWidget<UMapWidget>(GetWorld(), MainScreenClass);
		MainScreen->AddToViewport();
		MainScreen->HideImmediately();

		MainScreen->InitData(owner, 
			AGameController::GetInstance()->GetWorldTopLeftCorner(), 
			AGameController::GetInstance()->GetWorldBottomRightCorner(),
			Objectives,
			ZoomRatio,
			MaxZoom,
			SideMoveSpeed,
			SideMoveBorderWidth,
			BaseScale);
	}

	// init input
	owner->GetMyInputComponent()->BindAction("MapGpadReturn", IE_Pressed, this, &UMapComponent::GpadReturn);

	if (owner != NULL && MarkMenuClass != NULL)
	{
		MarkMenuScreen = CreateWidget<UMapIconMenuWidget>(Cast<APlayerController>(owner->GetController()), MarkMenuClass);
		MarkMenuScreen->AddToViewport();
		MarkMenuScreen->SetVisibility(ESlateVisibility::Collapsed);
		MarkMenuScreen->SetIfExternalMouseEvent(false);
		MarkMenuScreen->InitData(owner);
	}

}


// Called every frame
void UMapComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

void UMapComponent::ToggleVisible()
{
	if (MainScreen->GetVisibility() == ESlateVisibility::Visible)
	{
		DisableFocus();

		MainScreen->Hide();
	}
	else
	{
		EnableFocus(MainScreen->GetShowFocusWidget());

		MainScreen->Show();
	}
}

bool UMapComponent::IsVisible()
{
	return MainScreen->GetVisibility() == ESlateVisibility::Visible ? true : false;
}

void UMapComponent::SetMarkMenuOn()
{
	if (MarkMenuScreen->GetVisibility() == ESlateVisibility::Collapsed && MainScreen->GetVisibility() == ESlateVisibility::Collapsed)
	{
		EnableFocus(MarkMenuScreen->GetShowFocusWidget());
		MarkMenuScreen->Show();
	}
}

void UMapComponent::SetMarkMenuOff()
{
	if (MarkMenuScreen->GetVisibility() != ESlateVisibility::Collapsed)
	{
		UMapIconImage* icon = MarkMenuScreen->GetIconSelected();

		if (icon != NULL)
		{
			MarkFromWorld(icon);
		}

		DisableFocus();
		MarkMenuScreen->Hide();
	}
}

void UMapComponent::MarkFromWorld(class UMapIconImage * imageToMark)
{
	APlayerCharacter* owner = Cast<APlayerCharacter>(GetOwner());
	FVector2D worldPosition = FVector2D(owner->GetLocationWantsToHit());
	MainScreen->WorldToMapCoords(worldPosition);
	UMapIconImage* clone = MainScreen->CloneWidgetMapPosition(imageToMark, MainScreen->GetPanelByIconType(imageToMark->IconType), worldPosition);
	clone->OnMapPlacementAction(MainScreen->GetGeometry(), MainScreen->GetLastMouseEvent(), MainScreen, owner->GetLocationWantsToHit());
}

void UMapComponent::EnableFocus(const TSharedPtr<SWidget>& widget)
{
	FInputModeGameAndUI zbychu;
	zbychu.SetWidgetToFocus(widget);
	APlayerCharacter* owner = Cast<APlayerCharacter>(GetOwner());
	APlayerController* controller = Cast<APlayerController>(owner->GetController());
	controller->SetInputMode(zbychu);
	FSlateApplication::Get().SetKeyboardFocus(widget, EKeyboardFocusCause::SetDirectly);
}

void UMapComponent::DisableFocus()
{
	APlayerCharacter* owner = Cast<APlayerCharacter>(GetOwner());
	APlayerController* controller = Cast<APlayerController>(owner->GetController());
	controller->SetInputMode(FInputModeGameOnly::FInputModeGameOnly());
	FSlateApplication::Get().SetUserFocusToGameViewport(0);
}

void UMapComponent::OnObjectiveCompleted(UObjectiveComponent * obj)
{
	if (obj->GetObjectiveType() == EObjectiveType::Destroy)
	{
		--_activeObjectivesDestroyNum;
	}
	else if (obj->GetObjectiveType() == EObjectiveType::Find)
	{
		--_activeObjectivesFindNum;
	}
	else if (obj->GetObjectiveType() == EObjectiveType::Collect)
	{
		--_activeObjectivesCollectNum;
	}
}

void UMapComponent::OnObjectiveMarkedVisible(UObjectiveComponent * obj)
{
}

void UMapComponent::OnObjectiveMarkedOnMap(UObjectiveComponent * obj)
{
}

void UMapComponent::OnObjectiveMoved(UObjectiveComponent * obj, FVector location)
{
}

void UMapComponent::GpadReturn()
{
	if (MainScreen->GetVisibility() != ESlateVisibility::Collapsed)
	{
		ToggleVisible();
	}
}