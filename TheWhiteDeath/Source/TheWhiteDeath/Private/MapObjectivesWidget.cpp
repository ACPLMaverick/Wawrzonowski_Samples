// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "MapObjectivesWidget.h"
#include "Public/ObjectiveComponent.h"
#include "Public/MapWidget.h"
#include "Public/MapIconImage.h"

UMapObjectivesWidget::UMapObjectivesWidget(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

bool UMapObjectivesWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	_widgetVerticalBox = Cast<UVerticalBox>(Cast<UCanvasPanel>(GetRootWidget())->GetChildAt(2));
	_widgetTemplatePrimary = Cast<UHorizontalBox>(_widgetVerticalBox->GetChildAt(0));
	_widgetTemplateSecondary = Cast<UHorizontalBox>(_widgetVerticalBox->GetChildAt(1));
	_widgetTemplatePrimary->SetVisibility(ESlateVisibility::Collapsed);
	_widgetTemplateSecondary->SetVisibility(ESlateVisibility::Collapsed);


	return true;
}

void UMapObjectivesWidget::InitData(class UMapWidget* map, const TArray<class UObjectiveComponent*>& objectives)
{
	_map = map;

	int32 oNum = objectives.Num();
	bool bFirstPrimary, bFirstSecondary;
	bFirstPrimary = bFirstSecondary = false;

	for (int32 i = 0; i < oNum; ++i)
	{
		UHorizontalBox* objWidget = NULL;
		UHorizontalBox* temp = NULL;
		UMapIconImage* mapIcon = NULL;

		if (objectives[i]->GetObjectiveImportance() == EObjectiveImportance::Primary)
		{
			if (!bFirstPrimary)
			{
				bFirstPrimary = true;
				objWidget = _widgetTemplatePrimary;
			}
			else
			{
				temp = _widgetTemplatePrimary;
			}
		}
		else if (objectives[i]->GetObjectiveImportance() == EObjectiveImportance::Secondary)
		{
			if (!bFirstSecondary)
			{
				bFirstSecondary = true;
				objWidget = _widgetTemplateSecondary;
			}
			else
			{
				temp = _widgetTemplateSecondary;
			}
		}

		if (temp != NULL)
		{
			objWidget = DuplicateObject<UHorizontalBox>(temp, this);

			objWidget->Slot = _widgetVerticalBox->AddChild(objWidget);
			Cast<UVerticalBoxSlot>(objWidget->Slot)->SetPadding(Cast<UVerticalBoxSlot>(temp->Slot)->Padding);
			Cast<UVerticalBoxSlot>(objWidget->Slot)->SetSize(Cast<UVerticalBoxSlot>(temp->Slot)->Size);
			Cast<UVerticalBoxSlot>(objWidget->Slot)->SetHorizontalAlignment(Cast<UVerticalBoxSlot>(temp->Slot)->HorizontalAlignment);
			Cast<UVerticalBoxSlot>(objWidget->Slot)->SetVerticalAlignment(Cast<UVerticalBoxSlot>(temp->Slot)->VerticalAlignment);
		}

		objWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		UTextBlock* tb = Cast<UTextBlock>(Cast<UCanvasPanel>(objWidget->GetChildAt(1))->GetChildAt(0));
		tb->SetText(objectives[i]->GetObjectiveName());

		if (objectives[i]->GetInitiallyVisible() && objectives[i]->GetObjectiveType() != EObjectiveType::Find)
		{
			mapIcon = CreateMapIcon(objectives[i], Cast<UImage>(Cast<UCanvasPanel>(objWidget->GetChildAt(0))->GetChildAt(0)));
		}

		_mapComponentToWidget.Add(objectives[i], objWidget);
		_mapComponentToMapImage.Add(objectives[i], mapIcon);

		objectives[i]->DelegateObjectiveCompleted.AddDynamic(this, &UMapObjectivesWidget::OnObjectiveCompleted);
		objectives[i]->DelegateObjectiveMoved.AddDynamic(this, &UMapObjectivesWidget::OnObjectiveMoved);
		objectives[i]->DelegateObjectiveMarkedOnMap.AddDynamic(this, &UMapObjectivesWidget::OnObjectiveMarkedOnMap);
		objectives[i]->DelegateObjectiveMarkedVisible.AddDynamic(this, &UMapObjectivesWidget::OnObjectiveMarkedVisible);
	}
}


UMapIconImage * UMapObjectivesWidget::CreateMapIcon(const UObjectiveComponent* obj, const UImage* refImage)
{
	UTexture2D* texture = Cast<UTexture2D>(refImage->Brush.GetResourceObject());
	FVector2D pos = FVector2D(obj->GetOwner()->GetActorLocation());
	_map->WorldToMapCoords(pos);
	return _map->PlaceIconWidgetOnMap(EMapIconType::Objective, texture, pos, obj->GetObjectiveImportance() == EObjectiveImportance::Primary ? FVector2D(75.0f, 75.0f) : FVector2D(60.0f, 60.0f));
}

void UMapObjectivesWidget::OnObjectiveCompleted(UObjectiveComponent * obj)
{
	UHorizontalBox* aBox = _mapComponentToWidget[obj];
	UMapIconImage* img = _mapComponentToMapImage[obj];
	if (aBox != NULL)
	{
		Cast<UImage>(Cast<UCanvasPanel>(aBox->GetChildAt(0))->GetChildAt(0))->SetColorAndOpacity(ImageColorCompleted);
		Cast<UImage>(Cast<UCanvasPanel>(aBox->GetChildAt(0))->GetChildAt(1))->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		UTextBlock* text = Cast<UTextBlock>(Cast<UCanvasPanel>(aBox->GetChildAt(1))->GetChildAt(0));
		UImage* crossImage = Cast<UImage>(Cast<UCanvasPanel>(aBox->GetChildAt(1))->GetChildAt(1));
		crossImage->SetVisibility(ESlateVisibility::Visible);
		Cast<UCanvasPanelSlot>(crossImage->Slot)->SetAnchors(Cast<UCanvasPanelSlot>(text->Slot)->GetAnchors());
		FVector2D size = Cast<UCanvasPanelSlot>(text->Slot)->GetSize();
		size.Y = Cast<UCanvasPanelSlot>(crossImage->Slot)->GetSize().Y;
		Cast<UCanvasPanelSlot>(crossImage->Slot)->SetSize(size);
		Cast<UCanvasPanelSlot>(crossImage->Slot)->SetAlignment(Cast<UCanvasPanelSlot>(text->Slot)->GetAlignment());
		Cast<UCanvasPanelSlot>(crossImage->Slot)->SetPosition(Cast<UCanvasPanelSlot>(text->Slot)->GetPosition());
	}

	if (img != NULL && img->Slot != NULL && img->Slot->Parent != NULL)
	{
		img->Slot->Parent->RemoveChild(img);
	}
}

void UMapObjectivesWidget::OnObjectiveMarkedVisible(UObjectiveComponent * obj)
{
	if (obj->GetObjectiveType() == EObjectiveType::Find && _mapComponentToMapImage[obj] == NULL)
	{
		_mapComponentToMapImage[obj] = CreateMapIcon(obj, Cast<UImage>(Cast<UCanvasPanel>(_mapComponentToWidget[obj]->GetChildAt(0))->GetChildAt(0)));
	}
}

void UMapObjectivesWidget::OnObjectiveMarkedOnMap(UObjectiveComponent * obj)
{
}

void UMapObjectivesWidget::OnObjectiveMoved(UObjectiveComponent * obj, FVector location)
{
	UMapIconImage* img = _mapComponentToMapImage[obj];
	if (img != NULL)
	{
		FVector2D location2d(location);
		_map->WorldToMapCoords(location2d);
		Cast<UCanvasPanelSlot>(img->Slot)->SetPosition(location2d);
	}
}