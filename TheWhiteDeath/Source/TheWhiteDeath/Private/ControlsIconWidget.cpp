// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "ControlsIconWidget.h"

bool UControlsIconWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	UCanvasPanel* canvas = Cast<UCanvasPanel>(GetRootWidget());


	int32 tNum = Textures.Num();
	for (int32 i = 0; i < tNum; ++i)
	{
		UImage* nImage = NewObject<UImage>(UImage::StaticClass());
		_contrImages.Add(nImage);

		nImage->Slot = canvas->AddChild(nImage);
		UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(nImage->Slot);
		slot->SetAnchors(FAnchors(0.5f));
		slot->SetPosition(FVector2D(0.0f, 0.0f));
		slot->SetAlignment(FVector2D(0.5f, 0.5f));
		slot->SetSize(ImageSize);

		nImage->SetBrushFromTexture(Textures[i]);

		nImage->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (_contrImages.Num() > 0)
	{
		_contrImages[0]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	return true;
}

void UControlsIconWidget::ChangeImage(int32 index)
{
	int32 num = _contrImages.Num();
	if (num > 0)
	{
		int32 clampedIndex = FMath::Clamp(index, 0, num - 1);
		for (int32 i = 0; i < num; ++i)
		{
			if (i == clampedIndex)
			{
				_contrImages[i]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			else
			{
				_contrImages[i]->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void UControlsIconWidget::ChangeImage(float index, float max)
{
	float val = index / max * (float)(_contrImages.Num());
	ChangeImage((int32)(val - 1));
}
