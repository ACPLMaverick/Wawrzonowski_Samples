// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "MapIconWidget.h"
#include "Public/MapIconImage.h"

UMapIconWidget::UMapIconWidget(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

bool UMapIconWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (IconClass) // Check the selected UI class is not NULL
	{
		UWorld* world = GetWorld();
		if (world == NULL)
		{
			UPanelWidget* parent = GetParent();
			while (parent != NULL)
			{
				world = parent->GetWorld();

				if (world != NULL)
				{
					break;
				}
				else
				{
					parent = parent->GetParent();
				}
			}
			
		}

		if (world != NULL)
		{
			_icon = CreateWidget<UMapIconImage>(world, IconClass); // Create Widget
			_icon->Slot = Cast<UCanvasPanel>(GetRootWidget())->AddChild(_icon);

			_icon->IconType = IconType;

			Cast<UCanvasPanelSlot>(_icon->Slot)->SetAnchors(FAnchors(0.5f, 0.5f));
			Cast<UCanvasPanelSlot>(_icon->Slot)->SetAlignment(FVector2D(0.5f, 0.5f));
			Cast<UCanvasPanelSlot>(_icon->Slot)->SetPosition(FVector2D(0.0f, 0.0f));

			if (Cast<UCanvasPanelSlot>(Slot) != NULL)
			{
				Cast<UCanvasPanelSlot>(_icon->Slot)->SetSize(Cast<UCanvasPanelSlot>(Slot)->GetSize());
			}
			else
			{
				Cast<UCanvasPanelSlot>(_icon->Slot)->SetSize(FVector2D(50.0f, 50.0f));
			}
		}
	}

	return true;
}

void UMapIconWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UMapIconWidget::CounterRotateIcon(float counterAngle)
{
	if (_icon != NULL)
	{
		_icon->SetRenderAngle(-counterAngle);
	}
}