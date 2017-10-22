// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "MapCursorWidget.h"

bool UMapCursorWidget::Initialize()
{
	if (CursorTextures.Num() < 3)
	{
		UE_LOG(LogTemp, Log, TEXT("CURSOR: Texture count is smaller than cursor type count."));
		return false;
	}

	_cursorImage = Cast<UImage>(Cast<UCanvasPanel>(GetRootWidget())->GetChildAt(0));
	return Super::Initialize();
}

void UMapCursorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UMapCursorWidget::SetCursorMode(EMapCursorMode mode)
{
}
