// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "EnemyCharacter.h"
#include "MapWidget.h"
#include "MapIconImage.h"
#include "Public/MapIconMenuWidget.h"
#include "Public/MapDrawingArea.h"
#include "Public/ControlsIconWidget.h"
#include "Public/MapFiltersWidget.h"
#include "Public/MapObjectivesWidget.h"
#include "Public/ObjectiveComponent.h"
#include "Public/TWDUtilities.h"
#include "Components/InputComponent.h"

/*
MAP ICON ORDER:
"IconEnemy
"IconDraw
"IconSight
"IconDoors
"IconAmmo
*/

UMapWidget::UMapWidget(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	bIsFocusable = true;
}

bool UMapWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	_widgetRoot = GetRootWidget();
	_widgetMapCanvasPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("CanvasPanelMap")));
	_widgetMapImage = Cast<UImage>(_widgetMapCanvasPanel->GetChildAt(0));
	_widgetIconPlayer = Cast<UImage>(_widgetMapCanvasPanel->GetChildAt(1));

	_widgetsIconPanels.Add(Cast<UCanvasPanel>(_widgetMapCanvasPanel->GetChildAt(2)));
	_widgetsIconPanels.Add(Cast<UCanvasPanel>(_widgetMapCanvasPanel->GetChildAt(3)));
	_widgetsIconPanels.Add(Cast<UCanvasPanel>(_widgetMapCanvasPanel->GetChildAt(4)));
	_widgetsIconPanels.Add(Cast<UCanvasPanel>(_widgetMapCanvasPanel->GetChildAt(5)));
	_widgetsIconPanels.Add(Cast<UCanvasPanel>(_widgetMapCanvasPanel->GetChildAt(6)));
	_widgetsIconPanels.Add(Cast<UCanvasPanel>(_widgetMapCanvasPanel->GetChildAt(7)));

	_templateDrawingArea = Cast<UMapDrawingArea>(_widgetsIconPanels[1]->GetChildAt(0));
	_currentDrawingArea = NULL;

	_widgetIconMenu = Cast<UMapIconMenuWidget>(GetWidgetFromName(TEXT("MapIconMenu")));

	_widgetFilters = Cast<UMapFiltersWidget>(GetWidgetFromName(TEXT("Filters")));
	_widgetFilters->DelegateFilterChanged.RemoveDynamic(this, &UMapWidget::OnFiltersChanged);
	_widgetFilters->DelegateFilterChanged.AddDynamic(this, &UMapWidget::OnFiltersChanged);

	_widgetObjectives = Cast<UMapObjectivesWidget>(GetWidgetFromName(TEXT("Objectives")));

	UCanvasPanel* bottomPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("CanvasPanelBottom")));
	_widgetScale = Cast<UControlsIconWidget>(GetWidgetFromName(TEXT("Scale")));

	UHorizontalBox* horBox = Cast<UHorizontalBox>(bottomPanel->GetChildAt(1));
	int32 horBoxCNum = horBox->GetChildrenCount();
	for (int32 i = 0; i < horBoxCNum; ++i)
	{
		UHorizontalBox* horBoxIn = Cast<UHorizontalBox>(horBox->GetChildAt(i));
		_widgetsInputSpecific.Add(Cast<UControlsIconWidget>(horBoxIn->GetChildAt(0)));
	}

	UCanvasPanel* sideBottomPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("CanvasPanelBottomSide")));
	_widgetInputSpecificToggleFilterMenu = Cast<UControlsIconWidget>(Cast<UHorizontalBox>(Cast<UHorizontalBox>(sideBottomPanel->GetChildAt(1))->GetChildAt(0))->GetChildAt(0));
	_widgetInputSpecificDraw = Cast<UControlsIconWidget>(Cast<UHorizontalBox>(Cast<UHorizontalBox>(sideBottomPanel->GetChildAt(1))->GetChildAt(1))->GetChildAt(0));
	_widgetInputSpecificToggleFilterMenu->GetParent()->SetVisibility(ESlateVisibility::Collapsed);
	_widgetInputSpecificDraw->GetParent()->SetVisibility(ESlateVisibility::Collapsed);
	_widgetInputSpecificSidePanel = Cast<UCanvasPanel>(_widgetInputSpecificToggleFilterMenu->GetParent()->GetParent()->GetParent());
	_widgetInputSpecificSidePanel->SetVisibility(ESlateVisibility::Collapsed);

	// cursor setup
	_widgetMapImage->SetCursor(EMouseCursor::Hand);
	_widgetIconMenu->SetCursor(EMouseCursor::Hand);

	// init existing enemy trackers
	// maybe it will become useful some time of late
	/*
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), foundActors);
	int32 faNum = foundActors.Num();
	for (int32 i = 0; i < faNum; ++i)
	{
		FVector worldPosition = foundActors[i]->GetActorLocation();
		UMapIconImage* iconClone = CloneWidget<UMapIconImage>(_widgetIconMenu->GetIconByID(0), _widgetsIconPanels[0], FVector2D::ZeroVector);
		iconClone->OnMapPlacementAction(_geometry, _lastMouseEvent, this, worldPosition);
	}
	*/
	return true;
}

void UMapWidget::NativeTick(const FGeometry & MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	_geometry = MyGeometry;

	TickData(MyGeometry, InDeltaTime);
	TickPlayerPosition(MyGeometry, InDeltaTime);
	TickMapMoveOnCursorInBorderZone(MyGeometry, InDeltaTime);
	TickZoom(MyGeometry, InDeltaTime);
}

void UMapWidget::InitData(APlayerCharacter * player, FVector topLeftCornerWorldLocation, FVector bottomRightCornerWorldLocation,
	const TArray<class UObjectiveComponent*>& objectives,
	float zoomRatio,
	float maxZoom,
	float sideMoveSpeed,
	float sideMoveBorderWidth,
	int baseScale)
{
	_player = player;

	_player->DelegateInputTypeChanged.AddDynamic(this, &UMapWidget::OnPlayerInputTypeChanged);

	_zoomRatio = zoomRatio;
	_maxZoom = maxZoom;
	_baseScale = baseScale;
	_sideMoveSpeed = sideMoveSpeed;
	_sideMoveBorderWidth = sideMoveBorderWidth;

	FVector2D imgPos = Cast<UCanvasPanelSlot>(_widgetMapCanvasPanel->Slot)->GetPosition();
	FVector2D imgSize = Cast<UCanvasPanelSlot>(_widgetMapCanvasPanel->Slot)->GetSize() * 0.5f;
	FVector2D imgTopLeft = imgPos - imgSize;
	float imgMagnitude = (imgPos - imgTopLeft).Size();

	_mapBaseCorners[0] = imgTopLeft;
	_mapBaseCorners[1] = FVector2D(imgPos.X + imgSize.X, imgPos.Y - imgSize.Y);
	_mapBaseCorners[2] = FVector2D(imgPos.X + imgSize.X, imgPos.Y + imgSize.Y);
	_mapBaseCorners[3] = FVector2D(imgPos.X - imgSize.X, imgPos.Y + imgSize.Y);

	FVector2D worldCenterPoint = FVector2D((topLeftCornerWorldLocation.X + bottomRightCornerWorldLocation.X) * 0.5f,
		(topLeftCornerWorldLocation.Y + bottomRightCornerWorldLocation.Y) * 0.5f);

	float worldMagnitude = (FVector2D(topLeftCornerWorldLocation) - worldCenterPoint).Size();
	_worldToMapScale = imgMagnitude / worldMagnitude;

	_mapBasePosition = imgPos;

	_widgetFilters->InitData(this);
	_widgetObjectives->InitData(this, objectives);

	// initialize gamepad input callbacks

	UInputComponent* ic = _player->GetMyInputComponent();
	ic->BindAxis("MapGpadMoveCursorHor", this, &UMapWidget::GpadMoveCursorHor);
	ic->BindAxis("MapGpadMoveCursorVert", this, &UMapWidget::GpadMoveCursorVert);
	ic->BindAxis("MapGpadMoveMapHor", this, &UMapWidget::GpadMoveHor);
	ic->BindAxis("MapGpadMoveMapVert", this, &UMapWidget::GpadMoveVert);
	ic->BindAxis("MapGpadZoomIn", this, &UMapWidget::GpadZoomIn);
	ic->BindAxis("MapGpadZoomOut", this, &UMapWidget::GpadZoomOut);

	ic->BindAction("MapGpadAccept", IE_Pressed, this, &UMapWidget::GpadAccept);
	ic->BindAction("MapGpadDraw", IE_Pressed, this, &UMapWidget::GpadDraw);
	ic->BindAction("MapGpadDraw", IE_Released, this, &UMapWidget::GpadDrawRelease);
	ic->BindAction("MapGpadAccept", IE_Released, this, &UMapWidget::GpadAcceptRelease);
	ic->BindAction("MapGpadReturn", IE_Pressed, this, &UMapWidget::GpadReturn);
	ic->BindAction("MapGpadRemove", IE_Pressed, this, &UMapWidget::GpadRemove);
	ic->BindAction("MapGpadFiltersToggle", IE_Pressed, this, &UMapWidget::GpadFiltersOn);
	ic->BindAction("MapGpadFiltersToggle", IE_Released, this, &UMapWidget::GpadFiltersOff);
	ic->BindAction("MapGpadFiltersUp", IE_Pressed, this, &UMapWidget::GpadFiltersUp);
	ic->BindAction("MapGpadFiltersDown", IE_Pressed, this, &UMapWidget::GpadFiltersDown);

	// ///
}

void UMapWidget::Show_Implementation()
{
	ShowImmediately();
	if (_player != NULL)
	{
		APlayerController* pc = Cast<APlayerController>(_player->GetController());
		pc->bShowMouseCursor = true;
		pc->bEnableClickEvents = true;
		pc->bEnableMouseOverEvents = true;

		FVector2D playerPos = Cast<UCanvasPanelSlot>(_widgetIconPlayer->Slot)->GetPosition();
		Cast<UCanvasPanelSlot>(_widgetMapCanvasPanel->Slot)->SetPosition(-playerPos);
		FixMapPosition();
	}
}

void UMapWidget::Hide_Implementation()
{
	HideImmediately();

	if (_bDrawMode)
	{
		DisableDrawMode();
	}

	if (_player != NULL)
	{
		APlayerController* pc = Cast<APlayerController>(_player->GetController());
		pc->bShowMouseCursor = false;
		pc->bEnableClickEvents = false;
		pc->bEnableMouseOverEvents = false;
	}
}

void UMapWidget::TickData(const FGeometry & myGeometry, float deltaTime)
{
	FVector2D alignment = Cast<UCanvasPanelSlot>(_widgetMapCanvasPanel->Slot)->GetAlignment();
	FVector2D localSize = myGeometry.GetLocalSize();
	FVector2D position = Cast<UCanvasPanelSlot>(_widgetMapCanvasPanel->Slot)->GetPosition();
	_mapFinalCornerTL = _mapBaseCorners[0] * _widgetMapCanvasPanel->RenderTransform.Scale + alignment * localSize + position;
	_mapFinalCornerBR = _mapBaseCorners[2] * _widgetMapCanvasPanel->RenderTransform.Scale + alignment * localSize + position;
}

void UMapWidget::TickPlayerPosition(const FGeometry& myGeometry, float deltaTime)
{
	if (_player != NULL && _widgetIconPlayer != NULL)
	{
		FVector2D playerPos = FVector2D(_player->GetActorLocation());
		WorldToMapCoords(playerPos);
		Cast<UCanvasPanelSlot>(_widgetIconPlayer->Slot)->SetPosition(playerPos);
		_widgetIconPlayer->SetRenderAngle(_player->GetActorRotation().Yaw);
	}
}

void UMapWidget::TickMapMoveOnCursorInBorderZone(const FGeometry& InGeometry, float deltaTime)
{
	if (_bMouseWasInMiddleZone && !_bIsMouseMiddleButtonPressed && !_bIsMouseLeftButtonPressed && !_bIsMouseRightButtonPressed)
	{
		//UE_LOG(LogTemp, Log, TEXT("I AM IN MIDDLE ZONE %f"), GWorld->GetTimeSeconds());
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "MIDDLE ZONE " + FString::SanitizeFloat(GWorld->GetTimeSeconds()));

		float mplier = _sideMoveSpeed / _widgetMapCanvasPanel->RenderTransform.Scale.X;
		//FVector2D cornerPointTL(FMath::Max(_mapFinalCornerTL.X, 0.0f), FMath::Max(_mapFinalCornerTL.Y, 0.0f));
		//FVector2D cornerPointBR(FMath::Min(_mapFinalCornerBR.X, myGeometry.GetLocalSize().X), FMath::Min(_mapFinalCornerBR.Y, myGeometry.GetLocalSize().Y));
		FVector2D tl = Cast<UCanvasPanelSlot>(_widgetMapImage->Slot)->GetPosition() - (-InGeometry.Size + Cast<UCanvasPanelSlot>(_widgetMapImage->Slot)->GetSize()) * 0.5f;
		FVector2D br = Cast<UCanvasPanelSlot>(_widgetMapImage->Slot)->GetPosition() - (-InGeometry.Size - Cast<UCanvasPanelSlot>(_widgetMapImage->Slot)->GetSize()) * 0.5f;
		FVector2D mousePosition = InGeometry.AbsoluteToLocal(_lastMouseEvent.GetScreenSpacePosition());

		FVector2D mapMovement
		(
			(-FMath::Max(mousePosition.X - (br.X - _sideMoveBorderWidth), 0.0f) -FMath::Min(mousePosition.X - (tl.X + _sideMoveBorderWidth), 0.0f)) * mplier,
			(-FMath::Max(mousePosition.Y - (br.Y - _sideMoveBorderWidth), 0.0f) - FMath::Min(mousePosition.Y - (tl.Y + _sideMoveBorderWidth), 0.0f)) * mplier
		);

		FVector2D pos = Cast<UCanvasPanelSlot>(_widgetMapCanvasPanel->Slot)->GetPosition();
		Cast<UCanvasPanelSlot>(_widgetMapCanvasPanel->Slot)->SetPosition(pos + mapMovement);

		FixMapPosition();
	}
}

void UMapWidget::TickZoom(const FGeometry & myGeometry, float deltaTime)
{
	if (_widgetMapCanvasPanel->RenderTransform.Scale.X != _targetZoom)
	{
		FVector2D currentScale = _widgetMapCanvasPanel->RenderTransform.Scale;
		currentScale = FMath::Vector2DInterpTo(currentScale, FVector2D(_targetZoom, _targetZoom), deltaTime, 10.0f);
		_widgetMapCanvasPanel->SetRenderScale(currentScale);

		if (FMath::Abs(_widgetMapCanvasPanel->RenderTransform.Scale.X - _targetZoom) < 0.001f)
		{
			_widgetMapCanvasPanel->SetRenderScale(FVector2D(_targetZoom, _targetZoom));
		}

		FixMapPosition();
	}
}

FVector UMapWidget::MapToWorldCoords(const FVector2D & mapPos, bool calculateZ) const
{
	FVector ret(-mapPos.Y, mapPos.X, 0.0f);
	FVector mPos = FVector(-Cast<UCanvasPanelSlot>(_widgetMapCanvasPanel->Slot)->GetPosition().Y, Cast<UCanvasPanelSlot>(_widgetMapCanvasPanel->Slot)->GetPosition().X, 0.0f);
	ret -= mPos;
	ret /= _widgetMapCanvasPanel->RenderTransform.Scale.X;
	//ret.X -= 16.0f;
	ret /= _worldToMapScale;

	if (calculateZ)
	{
		FHitResult hResult;
		FVector start = FVector(mapPos.X, mapPos.Y, 10000.0f);
		FVector end = FVector(mapPos.X, mapPos.Y, -10000.0f);

		GWorld->LineTraceSingleByChannel(hResult, start, end, ECollisionChannel::ECC_WorldStatic);
		if (hResult.IsValidBlockingHit())
		{
			ret.Z = hResult.Location.Z;
			return ret;
		}
	}

	return ret;
}

void UMapWidget::WorldToMapCoords(FVector2D& in) const
{
	float tmp = in.X;
	in.X = in.Y;
	in.Y = -tmp;
	in *= _worldToMapScale;
	//in.Y -= 16.0f;
}

UMapIconImage * UMapWidget::PlaceIconWidgetOnMap(EMapIconType type, UTexture2D* sprite, FVector2D initialLocationAbsolute, FVector2D size)
{
	//FGeometry geom;
	//Cast<UCanvasPanel>(_widgetRoot)->GetGeometryForSlot(Cast<UCanvasPanelSlot>(_widgetMapCanvasPanel->Slot), geom);

	FString name = "IconPlacedOnMap" + FString::SanitizeFloat(GetWorld()->GetTimeSeconds());
	FName nameN;
	nameN.AppendString(name);
	UMapIconImage* newImage = CreateWidget<UMapIconImage>(Cast<APlayerController>(_player->GetController()), IconClasses[(int32)type]);

	newImage->Slot = _widgetsIconPanels[(int32)type]->AddChild(newImage);

	newImage->IconType = type;

	UImage* img = Cast<UImage>(newImage->GetWidgetFromName("Icon"));
	img->SetBrushFromTexture(sprite);
	Cast<UCanvasPanelSlot>(img->Slot)->SetSize(size);

	Cast<UCanvasPanelSlot>(newImage->Slot)->SetAnchors(FAnchors(0.5f, 0.5f));
	Cast<UCanvasPanelSlot>(newImage->Slot)->SetPosition(/*geom.AbsoluteToLocal(*/initialLocationAbsolute/*)*/);
	Cast<UCanvasPanelSlot>(newImage->Slot)->SetSize(size);
	Cast<UCanvasPanelSlot>(newImage->Slot)->SetAlignment(FVector2D(0.5f, 0.5f));

	return newImage;
}

template <typename T> T* UMapWidget::CloneWidget(T* src, UCanvasPanel* parent, FVector2D placePosition, FVector2D desiredSize)
{
	UCanvasPanelSlot* mapSlot = Cast<UCanvasPanelSlot>(_widgetMapCanvasPanel->Slot);
	FVector2D inPlacePosition = ((placePosition - mapSlot->GetPosition()) / _widgetMapCanvasPanel->RenderTransform.Scale);
	return CloneWidgetMapPosition(src, parent, inPlacePosition, desiredSize);
}

template<typename T>
T * UMapWidget::CloneWidgetMapPosition(T * src, UCanvasPanel * parent, FVector2D placePosition, FVector2D desiredSize)
{
	FName name;
	FString sName = (src->GetName() + FString::SanitizeFloat(GWorld->GetTimeSeconds()));
	name.AppendString(sName);
	T* clone = DuplicateObject<T>(src, parent, name);

	clone->Slot = parent->AddChild(clone);
	Cast<UCanvasPanelSlot>(clone->Slot)->SetAnchors(Cast<UCanvasPanelSlot>(src->Slot)->GetAnchors());

	if (desiredSize.X != 0.0f || desiredSize.Y != 0.0f)
	{
		Cast<UCanvasPanelSlot>(clone->Slot)->SetSize(desiredSize);
	}
	else
	{
		Cast<UCanvasPanelSlot>(clone->Slot)->SetSize(FVector2D(50.0f, 50.0f));						// hard-coded icon map size
	}

	Cast<UCanvasPanelSlot>(clone->Slot)->SetAlignment(Cast<UCanvasPanelSlot>(src->Slot)->GetAlignment());
	clone->SetRenderAngle(0.0f);

	Cast<UCanvasPanelSlot>(clone->Slot)->SetPosition(placePosition);

	return clone;
}

FVector2D UMapWidget::GetCanvasMousePosition(const FGeometry & geometry)
{
	FVector2D alignment = Cast<UCanvasPanelSlot>(_widgetMapCanvasPanel->Slot)->GetAlignment();
	FVector2D localSize = geometry.GetLocalSize();
	FVector2D mousePosition(0.0f, 0.0f);

	if (_player != NULL)
	{
		int vx, vy;
		Cast<APlayerController>(_player->GetController())->GetViewportSize(vx, vy);
		Cast<APlayerController>(_player->GetController())->GetMousePosition(mousePosition.X, mousePosition.Y);
		mousePosition.X /= (float)vx;
		mousePosition.Y /= (float)vy;
		mousePosition.X *= localSize.X;
		mousePosition.Y *= localSize.Y;
	}

	return mousePosition;
}

void UMapWidget::ProcessCircleMark(const FGeometry& myGeometry)
{
	if (_currentDrawingArea->CheckIfDrawnCircle())
	{
		FVector2D center2D(_currentDrawingArea->GetMiddlePoint());
		FGeometry geom;
		Cast<UCanvasPanel>(_widgetMapCanvasPanel->Slot->Parent)->GetGeometryForSlot(Cast<UCanvasPanelSlot>(_widgetMapCanvasPanel->Slot), geom);
		center2D = geom.AbsoluteToLocal(center2D);
		center2D -= geom.Size * 0.5f;
		center2D /= _widgetMapCanvasPanel->RenderTransform.Scale.X;

		FVector center = MapToWorldCoords(center2D, false);
		float radius = _currentDrawingArea->GetRadius() / _worldToMapScale;
		FVector start(center.X, center.Y, 10000.0f);
		FVector end(center.X, center.Y, -10000.0f);

		ECollisionChannel channels[3] = { ECollisionChannel::ECC_WorldStatic, ECollisionChannel::ECC_WorldDynamic, ECollisionChannel::ECC_Pawn };

		TArray<FHitResult> hResults;
		UObjectiveComponent* objective;
		for (int32 i = 0; i < 3; ++i)
		{
			TWDUtilities::VTraceSphereMulti(_player, start, end, radius, hResults, channels[i]);
			int32 rNum = hResults.Num();
			for (int32 j = 0; j < rNum; ++j)
			{
				if ((objective = Cast<UObjectiveComponent>(hResults[j].Actor->GetComponentByClass(UObjectiveComponent::StaticClass()))) != NULL)
				{
					objective->SetMarkedOnMap();
					_currentDrawingArea->SetAsMarkingObjective();
				}
			}
		}
	}
}

void UMapWidget::ProcessWidgetRemoval(FVector2D cursorPositionAbsolute, FVector2D cursorPositionMapSpace)
{
	const int32 iconPanelsNum = _widgetsIconPanels.Num() - 1;	// to prevent taking objectives into account
	const float deletionMaxRadius = 50.0f;
	for (int32 i = 0; i < iconPanelsNum; ++i)
	{
		if (_widgetsIconPanels[i]->GetVisibility() != ESlateVisibility::Collapsed)
		{
			if (i == 1)	// lines are different
			{
				const int32 cNum = _widgetsIconPanels[i]->GetChildrenCount();
				for (int32 j = 1; j < cNum; ++j)
				{
					UMapDrawingArea* area = Cast<UMapDrawingArea>(_widgetsIconPanels[i]->GetChildAt(j));
					if (area != NULL)
					{
						FVector2D middlePoint = area->GetMiddlePoint();

						if (!area->IsMarkingObjective() &&
							(FVector2D::Distance(middlePoint, cursorPositionAbsolute) <= deletionMaxRadius * 0.5f + area->GetRadius()))
						{
							_widgetsIconPanels[i]->RemoveChild(area);
							return;
						}
					}
				}
			}
			else
			{
				const int32 cNum = _widgetsIconPanels[i]->GetChildrenCount();
				for (int32 j = 0; j < cNum; ++j)
				{
					UWidget* widget = _widgetsIconPanels[i]->GetChildAt(j);
					FVector2D wPosition = Cast<UCanvasPanelSlot>(widget->Slot)->GetPosition();

					if (FVector2D::Distance(wPosition, cursorPositionMapSpace) <= deletionMaxRadius)
					{
						_widgetsIconPanels[i]->RemoveChild(widget);

						if (widget->IsA<UMapIconImage>())
						{
							Cast<UMapIconImage>(widget)->OnMapEraseAction(_geometry, _lastMouseEvent);
						}

						return;
					}
				}
			}
		}
	}
}

void UMapWidget::FixMapPosition()
{
	FVector2D points[4];
	FVector2D imgPos = Cast<UCanvasPanelSlot>(_widgetMapCanvasPanel->Slot)->GetPosition();
	FVector2D imgSize = Cast<UCanvasPanelSlot>(_widgetMapCanvasPanel->Slot)->GetSize() * 0.5f * _widgetMapCanvasPanel->RenderTransform.Scale;

	points[0] = imgPos - imgSize;
	points[1] = FVector2D(imgPos.X + imgSize.X, imgPos.Y - imgSize.Y);
	points[2] = imgPos + imgSize;
	points[3] = FVector2D(imgPos.X - imgSize.X, imgPos.Y + imgSize.Y);

	if (
		(points[0].X > _mapBaseCorners[0].X) ||
		(points[2].Y < _mapBaseCorners[2].Y) ||
		(points[2].X < _mapBaseCorners[2].X) ||
		(points[0].Y > _mapBaseCorners[0].Y) ||
		(points[3].X > _mapBaseCorners[3].X) ||
		(points[1].Y > _mapBaseCorners[1].Y) ||
		(points[1].X < _mapBaseCorners[1].X) ||
		(points[3].Y < _mapBaseCorners[3].Y)
		)
	{
		FVector2D adjustment = FVector2D
		(
			-(FMath::Max(points[0].X - _mapBaseCorners[0].X, 0.0f)) + 
			FMath::Max(_mapBaseCorners[1].X - points[1].X, 0.0f),
			-(FMath::Max(points[0].Y - _mapBaseCorners[0].Y, 0.0f)) +
			FMath::Max(_mapBaseCorners[3].Y - points[3].Y, 0.0f)
		);

		Cast<UCanvasPanelSlot>(_widgetMapCanvasPanel->Slot)->SetPosition(imgPos + adjustment);
	}
}

void UMapWidget::ShowIconMenu(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent)
{
	UCanvasPanelSlot* menuCanvasSlot = Cast<UCanvasPanelSlot>(_widgetIconMenu->Slot);
	FVector2D size = menuCanvasSlot->GetSize() * 0.5f;
	FVector2D cursorPos = GetCanvasMousePosition(InGeometry);
	cursorPos.X -= InGeometry.Size.X * 0.5f;
	cursorPos.Y -= InGeometry.Size.Y * 0.5f;
	cursorPos.X = FMath::Clamp(cursorPos.X, (_mapFinalCornerTL.X - InGeometry.Size.X * 0.5f) + size.X, (_mapFinalCornerBR.X - InGeometry.Size.X * 0.5f)- size.X);
	cursorPos.Y = FMath::Clamp(cursorPos.Y, (_mapFinalCornerTL.Y - InGeometry.Size.Y * 0.5f) + size.Y, (_mapFinalCornerBR.Y - InGeometry.Size.Y * 0.5f) - size.Y);
	menuCanvasSlot->SetPosition(cursorPos);

	_widgetIconMenu->Show();

	Cast<APlayerController>(_player->GetController())->bShowMouseCursor = false;
}

void UMapWidget::HideIconMenuAndPickIcon(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent)
{
	Cast<APlayerController>(_player->GetController())->bShowMouseCursor = true;

	UMapIconImage* icon = _widgetIconMenu->GetIconSelected();

	if (icon != NULL)
	{
		FVector2D placePosition = Cast<UCanvasPanelSlot>(_widgetIconMenu->Slot)->GetPosition();
		if (icon->CanBeClonedAndPlaced())
		{
			UCanvasPanel* mPanel = _widgetsIconPanels[icon->GetIconTypeAsInt()];
			UMapIconImage* iconClone = CloneWidget<UMapIconImage>(icon, mPanel, placePosition);

			iconClone->OnMapPlacementAction(_geometry, _lastMouseEvent, this, MapToWorldCoords(placePosition));
		}
		else
		{
			icon->OnMapPlacementAction(InGeometry, InMouseEvent, this, MapToWorldCoords(placePosition));
		}
	}
	else if (_bDrawMode)
	{
		DisableDrawMode();
	}

	_widgetIconMenu->Hide();
}

void UMapWidget::ShowImmediately()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UMapWidget::HideImmediately()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UMapWidget::EnableDrawMode()
{
	_bDrawMode = true;

	if (_currentDrawingArea == NULL)
	{
		_currentDrawingArea = CloneWidget<UMapDrawingArea>(_templateDrawingArea, _widgetsIconPanels[1], FVector2D(0.0f, 0.0f), 
			Cast<UCanvasPanelSlot>(_templateDrawingArea->Slot)->GetSize());
	}

	_widgetMapImage->SetCursor(EMouseCursor::Crosshairs);

	_widgetInputSpecificDraw->GetParent()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (_widgetInputSpecificSidePanel->GetVisibility() == ESlateVisibility::Collapsed)
	{
		_widgetInputSpecificSidePanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UMapWidget::DisableDrawMode()
{
	_widgetMapImage->SetCursor(EMouseCursor::Hand);
	_bDrawMode = false;
	_currentDrawingArea = NULL;

	_widgetInputSpecificDraw->GetParent()->SetVisibility(ESlateVisibility::Collapsed);

	if (_widgetInputSpecificToggleFilterMenu->GetParent()->GetVisibility() == ESlateVisibility::Collapsed)
	{
		_widgetInputSpecificSidePanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

FReply UMapWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == "LeftMouseButton")
	{
		if (_bDrawMode)
		{
			EnableDrawMode();
		}

		_bIsMouseLeftButtonPressed = true;
	}
	if (InMouseEvent.GetEffectingButton() == "RightMouseButton")
	{
		if (_bDrawMode)
		{
			DisableDrawMode();
		}

		ShowIconMenu(InGeometry, InMouseEvent);

		_bIsMouseRightButtonPressed = true;
	}
	if (InMouseEvent.GetEffectingButton() == "MiddleMouseButton")
	{
		_bIsMouseMiddleButtonPressed = true;
	}

	return FReply::Handled();
}

FReply UMapWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == "LeftMouseButton")
	{
		if (_bDrawMode)
		{
			ProcessCircleMark(InGeometry);

			_currentDrawingArea = NULL;
		}

		_bIsMouseLeftButtonPressed = false;
	}
	if (InMouseEvent.GetEffectingButton() == "RightMouseButton")
	{
		HideIconMenuAndPickIcon(InGeometry, InMouseEvent);

		_bIsMouseRightButtonPressed = false;
	}
	if (InMouseEvent.GetEffectingButton() == "MiddleMouseButton")
	{
		_bIsMouseMiddleButtonPressed = false;
	}

	return FReply::Handled();
}

FReply UMapWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (_bIsMouseMiddleButtonPressed)
	{
		MoveMap(InGeometry, InMouseEvent.GetCursorDelta());
	}

	_lastMouseEvent = InMouseEvent;
	_widgetIconMenu->ExternalOnMouseMove(InGeometry, InMouseEvent, this);

	FVector2D tl = Cast<UCanvasPanelSlot>(_widgetMapImage->Slot)->GetPosition() - (-InGeometry.Size + Cast<UCanvasPanelSlot>(_widgetMapImage->Slot)->GetSize()) * 0.5f;
	FVector2D br = Cast<UCanvasPanelSlot>(_widgetMapImage->Slot)->GetPosition() - (-InGeometry.Size - Cast<UCanvasPanelSlot>(_widgetMapImage->Slot)->GetSize()) * 0.5f;
	FVector2D mousePosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	if ((mousePosition.X >= tl.X + _sideMoveBorderWidth) &&
		(mousePosition.X <= br.X - _sideMoveBorderWidth) &&
		(mousePosition.Y >= tl.Y + _sideMoveBorderWidth) &&
		(mousePosition.Y <= br.X - _sideMoveBorderWidth) &&
		!_bMouseWasInMiddleZone)
	{
		_bMouseWasInMiddleZone = true;
	}

	if ((
		(mousePosition.X < tl.X) ||
		(mousePosition.X > br.X) ||
		(mousePosition.Y < tl.Y) ||
		(mousePosition.Y > br.X)) &&
		_bMouseWasInMiddleZone)
	{
		_bMouseWasInMiddleZone = false;
	}

	OnMouseMoveDrawMode(InGeometry, InMouseEvent);

	return FReply::Unhandled();
}

FReply UMapWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	_targetZoom = FMath::Clamp(_widgetMapCanvasPanel->RenderTransform.Scale.X + InMouseEvent.GetWheelDelta() * _zoomRatio, 1.0f, _maxZoom);
	_widgetScale->ChangeImage(_targetZoom, _maxZoom);
	return FReply::Handled();
}

FReply UMapWidget::NativeOnKeyUp(const FGeometry & InGeometry, const FKeyEvent & InKeyEvent)
{
	if (InKeyEvent.GetKey() == "Delete")
	{
		DeleteIconFromMap(InGeometry);
	}

	return FReply::Unhandled();
}

void UMapWidget::OnMouseMoveDrawMode(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent)
{
	if (_bDrawMode && _currentDrawingArea != NULL && _bIsMouseLeftButtonPressed &&
		(InMouseEvent.GetCursorDelta().X != 0.0f || InMouseEvent.GetCursorDelta().Y != 0.0f))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::SanitizeFloat(InMouseEvent.GetScreenSpacePosition().X) + " | " + FString::SanitizeFloat(InMouseEvent.GetScreenSpacePosition().Y));
		_currentDrawingArea->AddLinePointToDraw(InMouseEvent.GetScreenSpacePosition());
	}
}

void UMapWidget::OnPlayerInputTypeChanged(EInputDeviceType newInputType)
{
	int32 wNum = _widgetsInputSpecific.Num();
	for (int32 i = 0; i < wNum; ++i)
	{
		_widgetsInputSpecific[i]->ChangeImage((int32)newInputType);
	}

	_widgetInputSpecificToggleFilterMenu->ChangeImage((int32)newInputType);
	_widgetInputSpecificDraw->ChangeImage((int32)newInputType);

	if (newInputType == EInputDeviceType::Gamepad)
	{
		_widgetInputSpecificToggleFilterMenu->GetParent()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (_widgetInputSpecificSidePanel->GetVisibility() == ESlateVisibility::Collapsed)
		{
			_widgetInputSpecificSidePanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
	else
	{
		_widgetInputSpecificToggleFilterMenu->GetParent()->SetVisibility(ESlateVisibility::Collapsed);

		if (_widgetInputSpecificDraw->GetParent()->GetVisibility() == ESlateVisibility::Collapsed)
		{
			_widgetInputSpecificSidePanel->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UMapWidget::OnFiltersChanged(EMapIconType typeChanged, bool newState)
{
	_widgetsIconPanels[(int32)typeChanged]->SetVisibility(newState ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
}

void UMapWidget::MoveCursor(FVector2D delta, bool limitToMapArea)
{
	APlayerController* pc = Cast<APlayerController>(_player->GetController());
	FIntPoint mousePos;
	if (pc != NULL && pc->GetLocalPlayer() != NULL && pc->GetLocalPlayer()->ViewportClient != NULL && pc->GetLocalPlayer()->ViewportClient->Viewport != NULL)
	{
		FViewport* vp = pc->GetLocalPlayer()->ViewportClient->Viewport;
		vp->GetMousePos(mousePos);

		FIntPoint min(0, 0);
		FIntPoint max(vp->GetSizeXY());

		if (limitToMapArea)
		{
			FVector2D fmin, fmax;
			UCanvasPanelSlot* mSlot = Cast<UCanvasPanelSlot>(_widgetMapImage->Slot);
			fmin.X = (mSlot->GetPosition().X - mSlot->GetSize().X * 0.5f + _geometry.Size.X * 0.5f) / _geometry.Size.X * (float)vp->GetSizeXY().X;
			fmin.Y = (mSlot->GetPosition().Y - mSlot->GetSize().Y * 0.5f + _geometry.Size.Y * 0.5f) / _geometry.Size.Y * (float)vp->GetSizeXY().Y;
			fmax.X = (mSlot->GetPosition().X + mSlot->GetSize().X * 0.5f + _geometry.Size.X * 0.5f) / _geometry.Size.X * (float)vp->GetSizeXY().X;
			fmax.Y = (mSlot->GetPosition().Y + mSlot->GetSize().Y * 0.5f + _geometry.Size.Y * 0.5f) / _geometry.Size.Y * (float)vp->GetSizeXY().Y;

			int totalOffset = 3;
			int yOffset = 20;
			min.X = fmin.X + totalOffset;
			min.Y = fmin.Y + totalOffset - yOffset;
			max.X = fmax.X - totalOffset;
			max.Y = fmax.Y - totalOffset - yOffset;
		}

		mousePos.X = FMath::Clamp((int32)(((float)mousePos.X) + delta.X * (float)vp->GetSizeXY().X * _gpadCursorMoveSpeed), min.X, max.X);
		mousePos.Y = FMath::Clamp((int32)(((float)mousePos.Y) + delta.Y * (float)vp->GetSizeXY().Y * _gpadCursorMoveSpeed), min.Y, max.Y);
		vp->SetMouse(mousePos.X, mousePos.Y);
	}
}

void UMapWidget::MoveMap(const FGeometry & InGeometry, FVector2D cursorDelta)
{
	Cast<UCanvasPanelSlot>(_widgetMapCanvasPanel->Slot)->SetPosition(Cast<UCanvasPanelSlot>(_widgetMapCanvasPanel->Slot)->GetPosition() + cursorDelta);
	FixMapPosition();
}

void UMapWidget::DeleteIconFromMap(const FGeometry & InGeometry)
{
	FVector2D cursorPos = GetCanvasMousePosition(InGeometry);
	cursorPos -= InGeometry.Size * 0.5f + Cast<UCanvasPanelSlot>(_widgetMapCanvasPanel->Slot)->GetPosition();
	cursorPos /= _widgetMapCanvasPanel->RenderTransform.Scale.X;
	ProcessWidgetRemoval(_lastMouseEvent.GetScreenSpacePosition(), cursorPos);
}

void UMapWidget::GpadMoveCursorHor(float val)
{
	if (val != 0.0f && !_widgetFilters->GetIsShown())
	{
		MoveCursor(FVector2D(val, 0.0f), true);
	}
}

void UMapWidget::GpadMoveCursorVert(float val)
{
	if (val != 0.0f && !_widgetFilters->GetIsShown())
	{
		MoveCursor(FVector2D(0.0f, -val), true);
	}
}

void UMapWidget::GpadMoveHor(float val)
{
	if (val != 0.0f)
	{
		MoveMap(_geometry, FVector2D(-val * _gpadMapMoveSpeed, 0.0f));
	}
}

void UMapWidget::GpadMoveVert(float val)
{
	if (val != 0.0f)
	{
		MoveMap(_geometry, FVector2D(0.0f, -val * _gpadMapMoveSpeed));
	}
}

void UMapWidget::GpadZoomIn(float val)
{
	if (val != 0.0f)
	{
		_targetZoom = FMath::Clamp(_widgetMapCanvasPanel->RenderTransform.Scale.X + val * _gpadZoomSpeed * _zoomRatio, 1.0f, _maxZoom);
		_widgetScale->ChangeImage(_targetZoom, _maxZoom);
	}
}

void UMapWidget::GpadZoomOut(float val)
{
	if (val != 0.0f)
	{
		_targetZoom = FMath::Clamp(_widgetMapCanvasPanel->RenderTransform.Scale.X - val * _gpadZoomSpeed * _zoomRatio, 1.0f, _maxZoom);
		_widgetScale->ChangeImage(_targetZoom, _maxZoom);
	}
}

void UMapWidget::GpadAccept()
{
	ShowIconMenu(_geometry, _lastMouseEvent);
}

void UMapWidget::GpadDraw()
{
	if (_bDrawMode)
	{
		EnableDrawMode();
	}

	_gpadCursorMoveSpeed = _gpadCursorMoveDrawSpeed;

	_bIsMouseLeftButtonPressed = true;
}

void UMapWidget::GpadDrawRelease()
{
	if (_bDrawMode)
	{
		ProcessCircleMark(_geometry);

		_currentDrawingArea = NULL;
	}

	_gpadCursorMoveSpeed = _gpadCursorMoveDefaultSpeed;

	_bIsMouseLeftButtonPressed = false;
}

void UMapWidget::GpadAcceptRelease()
{
	HideIconMenuAndPickIcon(_geometry, _lastMouseEvent);
}

void UMapWidget::GpadReturn()
{
	// do nth;
}

void UMapWidget::GpadRemove()
{
	DeleteIconFromMap(_geometry);
}

void UMapWidget::GpadFiltersOn()
{
	_widgetFilters->Show();
	FInputModeGameAndUI zbychu;
	TSharedPtr<SWidget> w = _widgetFilters->GetShowFocusWidget();
	zbychu.SetWidgetToFocus(w);
	Cast<APlayerController>(_player->GetController())->SetInputMode(zbychu);
	FSlateApplication::Get().SetKeyboardFocus(w, EKeyboardFocusCause::SetDirectly);
}

void UMapWidget::GpadFiltersOff()
{
	_widgetFilters->Hide();
	FInputModeGameAndUI zbychu;
	TSharedPtr<SWidget> w = GetShowFocusWidget();
	zbychu.SetWidgetToFocus(w);
	Cast<APlayerController>(_player->GetController())->SetInputMode(zbychu);
	FSlateApplication::Get().SetKeyboardFocus(w, EKeyboardFocusCause::SetDirectly);
}

void UMapWidget::GpadFiltersUp()
{
}

void UMapWidget::GpadFiltersDown()
{
}