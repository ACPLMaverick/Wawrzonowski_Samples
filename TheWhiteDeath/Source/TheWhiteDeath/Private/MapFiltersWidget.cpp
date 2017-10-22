// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "MapFiltersWidget.h"
#include "Public/MapWidget.h"


bool UMapFiltersWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	_beginPosition = Slot != NULL ? Cast<UCanvasPanelSlot>(Slot)->GetPosition() : FVector2D(0.0f, 0.0f);
	_emergedPosition = _beginPosition + EmergedPositionDelta;

	UVerticalBox* btnBox = Cast<UVerticalBox>(Cast<UCanvasPanel>(GetRootWidget())->GetChildAt(2));
	UVerticalBox* imgBox = Cast<UVerticalBox>(Cast<UCanvasPanel>(GetRootWidget())->GetChildAt(3));
	int32 btnNum = btnBox->GetChildrenCount();
	for (int32 i = 0; i < btnNum; ++i)
	{
		_btns.Add(Cast<UButton>(btnBox->GetChildAt(i)));
		_imgs.Add(Cast<UImage>(imgBox->GetChildAt(i)));
		_btns[i]->OnClicked.Clear();

		_btns[i]->ColorAndOpacity = EnabledButtonColor;
		Cast<UTextBlock>(_btns[i]->GetChildAt(0))->ColorAndOpacity = EnabledTextColor;
		_imgs[i]->ColorAndOpacity = EnabledImageColor;

		_states.Add(false);
	}

	_btns[0]->OnClicked.AddDynamic(this, &UMapFiltersWidget::OnButtonClickEnemies);
	_btns[1]->OnClicked.AddDynamic(this, &UMapFiltersWidget::OnButtonClickDrawings);
	_btns[2]->OnClicked.AddDynamic(this, &UMapFiltersWidget::OnButtonClickSights);
	_btns[3]->OnClicked.AddDynamic(this, &UMapFiltersWidget::OnButtonClickDoors);
	_btns[4]->OnClicked.AddDynamic(this, &UMapFiltersWidget::OnButtonClickAmmo);
	_btns[5]->OnClicked.AddDynamic(this, &UMapFiltersWidget::OnButtonClickObjectives);

	_buttonDefaultTexture = Cast<UTexture2D>(_btns[0]->WidgetStyle.Normal.GetResourceObject());
	_buttonHoveredTexture = Cast<UTexture2D>(_btns[0]->WidgetStyle.Hovered.GetResourceObject());

	return true;
}

void UMapFiltersWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Slot != NULL && Cast<UCanvasPanelSlot>(Slot)->GetPosition() != _targetPosition)
	{
		Cast<UCanvasPanelSlot>(Slot)->SetPosition(FMath::Vector2DInterpTo(Cast<UCanvasPanelSlot>(Slot)->GetPosition(), _targetPosition, InDeltaTime, 7.0f));

		if (FMath::Abs(Cast<UCanvasPanelSlot>(Slot)->GetPosition().SizeSquared() - _targetPosition.SizeSquared()) < 0.1f)
		{
			Cast<UCanvasPanelSlot>(Slot)->SetPosition(_targetPosition);
		}
	}

	if (_map->GetPlayer()->GetLastInputDeviceType() == EInputDeviceType::Gamepad)
	{
		int bNum = _btns.Num();
		for (int i = 0; i < bNum; ++i)
		{
			if (_btns[i]->HasKeyboardFocus() && _btns[i]->WidgetStyle.Normal.GetResourceObject() != _buttonHoveredTexture)
			{
				_btns[i]->WidgetStyle.Normal.SetResourceObject(_buttonHoveredTexture);
			}
			else if (!_btns[i]->HasKeyboardFocus() && _btns[i]->WidgetStyle.Normal.GetResourceObject() == _buttonHoveredTexture)
			{
				if (!_states[i])
				{
					_btns[i]->WidgetStyle.Normal.SetResourceObject(_buttonDefaultTexture);
				}
				else
				{
					_btns[i]->WidgetStyle.Normal.SetResourceObject(ButtonFilterActiveTexture);
				}
			}
		}
	}
}

void UMapFiltersWidget::InitData(class UMapWidget* map)
{
	_map = map;
}

void UMapFiltersWidget::Show()
{
	_bIsShown = true;
	_targetPosition = _emergedPosition;
}

void UMapFiltersWidget::Hide()
{
	_bIsShown = false;
	_targetPosition = _beginPosition;
}


void UMapFiltersWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (_map->GetPlayer()->GetLastInputDeviceType() == EInputDeviceType::KeyboardMouse)
	{
		_bIsMouseInArea = true;
		_bIsShown = true;
		_targetPosition = _emergedPosition;
	}
}

void UMapFiltersWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	_bIsMouseInArea = false;
	_bIsShown = false;
	_targetPosition = _beginPosition;
}

void UMapFiltersWidget::OnButtonClick(int32 index)
{
	// clicked button means filter is DISABLED, means we DONT see given category.
	// default state means all filters are ENABLED, means we se ALL categories
	if (!_states[index])
	{
		// disable
		_btns[index]->WidgetStyle.Normal.SetResourceObject(ButtonFilterActiveTexture);
		_imgs[index]->SetColorAndOpacity(DisabledImageColor);
	}
	else
	{
		// enable
		_btns[index]->WidgetStyle.Normal.SetResourceObject(_buttonDefaultTexture);
		_imgs[index]->SetColorAndOpacity(EnabledImageColor);
	}

	_states[index] = !_states[index];
	DelegateFilterChanged.Broadcast((EMapIconType)index, _states[index]);
}

void UMapFiltersWidget::OnButtonClickEnemies()
{
	OnButtonClick(0);
}

void UMapFiltersWidget::OnButtonClickDrawings()
{
	OnButtonClick(1);
}

void UMapFiltersWidget::OnButtonClickSights()
{
	OnButtonClick(2);
}

void UMapFiltersWidget::OnButtonClickDoors()
{
	OnButtonClick(3);
}

void UMapFiltersWidget::OnButtonClickAmmo()
{
	OnButtonClick(4);
}

void UMapFiltersWidget::OnButtonClickObjectives()
{
	OnButtonClick(5);
}
