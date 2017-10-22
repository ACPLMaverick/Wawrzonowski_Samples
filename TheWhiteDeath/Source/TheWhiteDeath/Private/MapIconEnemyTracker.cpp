// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "MapIconEnemyTracker.h"
#include "Public/MapEnemyTrackerRecognitionZone.h"
#include "Public/PlayerCharacter.h"
#include "Public/MapWidget.h"
#include "Engine/Texture2D.h"

UMapIconEnemyTracker::UMapIconEnemyTracker(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	IconType = EMapIconType::Enemy;
}

bool UMapIconEnemyTracker::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	_widgetRoot = Cast<UCanvasPanel>(GetRootWidget());
	_widgetDefaultIcon = Cast<UImage>(_widgetRoot->GetChildAt(1));
	_widgetFieldOfView = Cast<UImage>(_widgetRoot->GetChildAt(0));
	_widgetsDetectionLevels.Add(Cast<UImage>(_widgetRoot->GetChildAt(2)));
	_widgetsDetectionLevels.Add(Cast<UImage>(_widgetRoot->GetChildAt(3)));
	_widgetsDetectionLevels.Add(Cast<UImage>(_widgetRoot->GetChildAt(4)));
	_widgetsDetectionLevels.Add(Cast<UImage>(_widgetRoot->GetChildAt(5)));

	_widgetFieldOfView->SetVisibility(ESlateVisibility::Collapsed);
	_widgetsDetectionLevels[0]->SetVisibility(ESlateVisibility::Collapsed);
	_widgetsDetectionLevels[1]->SetVisibility(ESlateVisibility::Collapsed);
	_widgetsDetectionLevels[2]->SetVisibility(ESlateVisibility::Collapsed);
	_widgetsDetectionLevels[3]->SetVisibility(ESlateVisibility::Collapsed);

	for (int32 i = 1; i < 4; ++i)
	{
		_detectionStateTextures.Add(Cast<UTexture2D>(_widgetsDetectionLevels[i]->Brush.GetResourceObject()));
	}

	return true;
}

void UMapIconEnemyTracker::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (_enemy != NULL)
	{
		FVector2D mapPos = FVector2D(_enemy->GetActorLocation());
		_map->WorldToMapCoords(mapPos);
		Cast<UCanvasPanelSlot>(Slot)->SetPosition(mapPos);

		SetRenderAngle(_enemy->GetActorRotation().Yaw);

		if (_map->GetPlayer() != NULL)
		{
			if (_map->GetPlayer()->HasSkill(0) && _widgetFieldOfView->GetVisibility() == ESlateVisibility::Collapsed)
			{
				_widgetFieldOfView->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				// enlarge slot to accomodate field of view
				FVector2D maxSize = Cast<UCanvasPanelSlot>(_widgetFieldOfView->Slot)->GetSize() * 2.0f;
				Cast<UCanvasPanelSlot>(Slot)->SetSize(maxSize);
			}
		}

		if (_enemy->GetDetectionState() != _prevPlayerDetection)
		{
			ChangePlayerDetection();
		}
	}
}


void UMapIconEnemyTracker::OnMapPlacementAction(const FGeometry& ExtGeometry, const FPointerEvent& ExtPointerEvent, class UMapWidget* map, const FVector& worldPosition)
{
	Super::OnMapPlacementAction(ExtGeometry, ExtPointerEvent, map, worldPosition);

	if (_tracker == NULL)
	{
		FActorSpawnParameters params;
		FName name;
		FString stringChujKurwaMarchewka = "EnemyTracker" + FString::SanitizeFloat(GetWorld()->GetTimeSeconds());
		name.AppendString(stringChujKurwaMarchewka);
		params.Name = name;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		params.bNoFail = true;

		if (_map->GetPlayer() != NULL)
		{
			_tracker = Cast<AMapEnemyTrackerRecognitionZone>(_map->GetPlayer()->GetWorld()->SpawnActor<AMapEnemyTrackerRecognitionZone>(AMapEnemyTrackerRecognitionZone::StaticClass(), worldPosition, FRotator::ZeroRotator, params));
		}
		else
		{
			_tracker = Cast<AMapEnemyTrackerRecognitionZone>(GetWorld()->SpawnActor<AMapEnemyTrackerRecognitionZone>(AMapEnemyTrackerRecognitionZone::StaticClass(), worldPosition, FRotator::ZeroRotator, params));
		}
		
		_tracker->DelegateEnemyFound.AddDynamic(this, &UMapIconEnemyTracker::OnEnemyRecognized);
		_tracker->RegisterMapTracker(this);
		_widgetDefaultIcon->SetVisibility(ESlateVisibility::Collapsed);
		_widgetsDetectionLevels[0]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	}
}

void UMapIconEnemyTracker::OnMapEraseAction(const FGeometry& ExtGeometry, const FPointerEvent& ExtPointerEvent)
{
	Super::OnMapEraseAction(ExtGeometry, ExtPointerEvent);
}

void UMapIconEnemyTracker::ChangePlayerDetection()
{
	_prevPlayerDetection = _enemy->GetDetectionState();

	if (_tracker != NULL)
	{
		_tracker->UpdateDetectionState(_prevPlayerDetection);
	}
	switch (_prevPlayerDetection)
	{
	case EPlayerDetectionState::Unaware:
	{
		_widgetsDetectionLevels[1]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		_widgetsDetectionLevels[2]->SetVisibility(ESlateVisibility::Collapsed);
		_widgetsDetectionLevels[3]->SetVisibility(ESlateVisibility::Collapsed);
	}
		break;
	case EPlayerDetectionState::Suspicious:
	{
		_widgetsDetectionLevels[1]->SetVisibility(ESlateVisibility::Collapsed);
		_widgetsDetectionLevels[2]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		_widgetsDetectionLevels[3]->SetVisibility(ESlateVisibility::Collapsed);
	}
		break;
	case EPlayerDetectionState::Aware:
	{
		_widgetsDetectionLevels[1]->SetVisibility(ESlateVisibility::Collapsed);
		_widgetsDetectionLevels[2]->SetVisibility(ESlateVisibility::Collapsed);
		_widgetsDetectionLevels[3]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
		break;
	default:	// should never happen.
	{
		_widgetsDetectionLevels[0]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		_widgetsDetectionLevels[1]->SetVisibility(ESlateVisibility::Collapsed);
		_widgetsDetectionLevels[2]->SetVisibility(ESlateVisibility::Collapsed);
		_widgetsDetectionLevels[3]->SetVisibility(ESlateVisibility::Collapsed);
	}
		break;
	}
}

void UMapIconEnemyTracker::OnEnemyRecognized(class AEnemyCharacter* enemyChar)
{
	_enemy = enemyChar;
	_enemy->OnDestroyed.RemoveDynamic(this, &UMapIconEnemyTracker::OnEnemyDestroyed);
	_enemy->OnDestroyed.AddDynamic(this, &UMapIconEnemyTracker::OnEnemyDestroyed);
	_widgetsDetectionLevels[0]->SetVisibility(ESlateVisibility::Collapsed);
	_widgetsDetectionLevels[1]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	_tracker->UpdateDetectionState(EPlayerDetectionState::Unaware);
}

void UMapIconEnemyTracker::OnEnemyDestroyed(AActor * DestroyedActor)
{
	_enemy = NULL;
	Cast<UCanvasPanel>(GetParent())->RemoveChild(this);
	RemoveFromViewport();
}
