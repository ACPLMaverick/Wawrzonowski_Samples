// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "MapIconMenuWidget.h"
#include "Public/PlayerCharacter.h"
#include "Public/MapWidget.h"
#include "Public/MapIconWidget.h"
#include "Public/MapIconImage.h"

UMapIconMenuWidget::UMapIconMenuWidget(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

bool UMapIconMenuWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	_widgetRoot = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Root")));

	int32 childNum = _widgetRoot->GetChildrenCount();
	for (int32 i = 1; i < childNum; ++i)	// do not take background into account
	{
		_widgetsIcon.Add(Cast<UMapIconWidget>(_widgetRoot->GetChildAt(i)));
	}

	int32 iconCount = _widgetsIcon.Num();
	_anglePerIcon = (360.0f / (float)iconCount);
	FVector2D placementVector(0.0f, -IconPlacementRadius);
	float placementAngle = 0.0f;
	for (int32 i = 0; i < iconCount; ++i)
	{
		if (_widgetsIcon[i] != NULL)
		{
			_widgetsIcon[i]->SetRenderAngle(placementAngle);
			_widgetsIcon[i]->CounterRotateIcon(placementAngle);
			Cast<UCanvasPanelSlot>(_widgetsIcon[i]->Slot)->SetPosition(placementVector);
			placementAngle += _anglePerIcon;
			placementVector = placementVector.GetRotated(_anglePerIcon);
		}
	}

	return true;
}

void UMapIconMenuWidget::InitData(APlayerCharacter * player)
{
	_player = player;

	if (_player != NULL)
	{
		_player->GetMyInputComponent()->BindAxis("MapGpadWorldMarkHor", this, &UMapIconMenuWidget::GpadWorldMarkHor);
		_player->GetMyInputComponent()->BindAxis("MapGpadWorldMarkVert", this, &UMapIconMenuWidget::GpadWorldMarkVert);
	}
}

void UMapIconMenuWidget::NativeTick(const FGeometry & MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UMapIconMenuWidget::Show_Implementation()
{
	for (int32 i = 0; i < _widgetsIcon.Num(); ++i)
	{
		if (_widgetsIcon[i]->IsHighlighted())
		{
			_widgetsIcon[i]->UnHighlightImmediately();
		}
	}
}

void UMapIconMenuWidget::Hide_Implementation()
{
	_slotSelectedId = -1;
}

FReply UMapIconMenuWidget::InternalOnMouseMove(const FGeometry & ExtGeometry, const FPointerEvent & ExtMouseEvent, const UMapWidget * caller)
{
	if (_widgetRoot->GetVisibility() != ESlateVisibility::Collapsed && (_player != NULL ? _player->GetLastInputDeviceType() == EInputDeviceType::KeyboardMouse : true))
	{
		FVector2D mouseDir;
		UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(Slot);

		mouseDir = ExtGeometry.AbsoluteToLocal(ExtMouseEvent.GetScreenSpacePosition());

		if (slot != NULL)
		{
			mouseDir -= slot->GetPosition() + ExtGeometry.Size * 0.5f;
		}
		else
		{
			mouseDir -= ExtGeometry.Size * 0.5f;
		}

		SolveMouseHighlight(mouseDir);

		return FReply::Handled();
	}
	else
	{
		_slotSelectedId = -1;
	}

	return FReply::Unhandled();
}

FReply UMapIconMenuWidget::NativeOnMouseMove(const FGeometry & ExtGeometry, const FPointerEvent & ExtMouseEvent)
{
	Super::NativeOnMouseMove(ExtGeometry, ExtMouseEvent);

	if (!bExternalMouseEvent)
	{
		return InternalOnMouseMove(ExtGeometry, ExtMouseEvent, NULL);
	}
	else
	{
		return FReply::Unhandled();
	}
}

void UMapIconMenuWidget::SolveMouseHighlight(FVector2D mouseDir)
{
	if (mouseDir.Size() >= InternalRadius)
	{
		FVector mouseDir3d = FVector(mouseDir, 0.0f);
		mouseDir3d.Normalize();
		mouseDir3d = mouseDir3d.RotateAngleAxis(-90.0f + _anglePerIcon * 0.5f, FVector::UpVector);
		float angle = FMath::RadiansToDegrees(mouseDir3d.HeadingAngle()) + 180.0f;

		int32 angleInt = (int32)angle;
		angleInt /= (int32)_anglePerIcon;
		angleInt = FMath::Clamp(angleInt, 0, _widgetsIcon.Num() - 1);

		if (_slotSelectedId != -1 && _slotSelectedId != angleInt && _widgetsIcon[_slotSelectedId]->IsHighlighted())
		{
			_widgetsIcon[_slotSelectedId]->UnHighlight();
		}

		if (_slotSelectedId != angleInt)
		{
			_slotSelectedId = angleInt;
			_widgetsIcon[_slotSelectedId]->Highlight();
		}
	}
	else
	{
		if (_slotSelectedId != -1 && _widgetsIcon[_slotSelectedId]->IsHighlighted())
		{
			_widgetsIcon[_slotSelectedId]->UnHighlight();
			_slotSelectedId = -1;
		}
	}
}

FReply UMapIconMenuWidget::ExternalOnMouseMove(const FGeometry & ExtGeometry, const FPointerEvent & ExtMouseEvent, const UMapWidget* caller)
{
	if (bExternalMouseEvent)
	{
		return InternalOnMouseMove(ExtGeometry, ExtMouseEvent, caller);
	}
	else
	{
		return FReply::Unhandled();
	}
}

UMapIconImage* UMapIconMenuWidget::GetIconSelected()
{
	if (_slotSelectedId != -1)
	{
		return _widgetsIcon[_slotSelectedId]->GetIcon();
	}
	else
	{
		return NULL;
	}
}

UMapIconImage * UMapIconMenuWidget::GetIconByID(int32 id)
{
	return _widgetsIcon[id]->GetIcon();
}

void UMapIconMenuWidget::GpadWorldMarkHor(float val)
{
	_lastGamepadInput.X = val;

	if (val != 0.0f)
	{
		SolveMouseHighlight(_lastGamepadInput * IconPlacementRadius);
	}
}

void UMapIconMenuWidget::GpadWorldMarkVert(float val)
{
	_lastGamepadInput.Y = val;

	if (val != 0.0f)
	{
		SolveMouseHighlight(_lastGamepadInput * IconPlacementRadius);
	}
}
