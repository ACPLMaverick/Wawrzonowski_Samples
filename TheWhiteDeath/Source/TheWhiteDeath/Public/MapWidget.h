// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Public/PlayerCharacter.h"
#include "Public/MapIconImage.h"
#include "MapWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEWHITEDEATH_API UMapWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	TArray<UCanvasPanel*> _widgetsIconPanels;
	TArray<class UControlsIconWidget*> _widgetsInputSpecific;
	class UControlsIconWidget* _widgetInputSpecificDraw;
	class UControlsIconWidget* _widgetInputSpecificToggleFilterMenu;
	class UCanvasPanel* _widgetInputSpecificSidePanel;

	FGeometry _geometry;
	FPointerEvent _lastMouseEvent;
	FVector2D _mapBaseCorners[4];
	FVector2D _mapBasePosition;
	FVector2D _mapFinalCornerTL;
	FVector2D _mapFinalCornerBR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MapWidget|Classes")
	TArray<TSubclassOf<UMapIconImage>> IconClasses;

	class UWidget* _widgetRoot;
	class UCanvasPanel* _widgetMapCanvasPanel;
	class UImage* _widgetMapImage;
	class UImage* _widgetIconPlayer;
	class UMapIconMenuWidget* _widgetIconMenu;
	class UControlsIconWidget* _widgetScale;
	class UMapFiltersWidget* _widgetFilters;
	class UMapObjectivesWidget* _widgetObjectives;
	
	class UMapDrawingArea* _templateDrawingArea;
	class UMapDrawingArea* _currentDrawingArea;
	
	class APlayerCharacter* _player = NULL;

	const float _gpadCursorMoveDefaultSpeed = 0.01f;
	const float _gpadCursorMoveDrawSpeed = 0.005f;
	float _gpadCursorMoveSpeed = _gpadCursorMoveDefaultSpeed;
	const float _gpadMapMoveSpeed = 5.0f;
	const float _gpadZoomSpeed = 1.0f;

	float _worldToMapScale = 1.0f;

	float _zoomRatio;
	float _maxZoom;
	float _sideMoveSpeed;
	float _sideMoveBorderWidth;
	int _baseScale;

	float _targetZoom = 1.0f;

	bool _bIsMouseLeftButtonPressed = false;
	bool _bIsMouseRightButtonPressed = false;
	bool _bIsMouseMiddleButtonPressed = false;
	bool _bMouseWasInMiddleZone = false;
	bool _bDrawMode = false;
	
	virtual void Show_Implementation();
	virtual void Hide_Implementation();

	void TickData(const FGeometry& myGeometry, float deltaTime);
	void TickPlayerPosition(const FGeometry& myGeometry, float deltaTime);
	void TickMapMoveOnCursorInBorderZone(const FGeometry& myGeometry, float deltaTime);
	void TickZoom(const FGeometry& myGeometry, float deltaTime);

	FVector2D GetCanvasMousePosition(const FGeometry& geometry);

	void ProcessCircleMark(const FGeometry& myGeometry);
	void ProcessWidgetRemoval(FVector2D cursorPositionAbsolute, FVector2D cursorPositionMapSpace);

	void FixMapPosition();

	void ShowIconMenu(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	void HideIconMenuAndPickIcon(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry & InGeometry, const FKeyEvent & InKeyEvent) override;


	void OnMouseMoveDrawMode(const FGeometry& myGeometry, const FPointerEvent& InMouseEvent);

	UFUNCTION()
	void OnPlayerInputTypeChanged(EInputDeviceType newInputType);
	
	UFUNCTION()
	void OnFiltersChanged(EMapIconType typeChanged, bool newState);

	void MoveCursor(FVector2D delta, bool limitToMapArea = true);
	void MoveMap(const FGeometry& InGeometry, FVector2D cursorDelta);
	void DeleteIconFromMap(const FGeometry & InGeometry);

#pragma region GamepadInputEventCallbacks

	void GpadMoveCursorHor(float val);
	void GpadMoveCursorVert(float val);
	void GpadMoveHor(float val);
	void GpadMoveVert(float val);
	void GpadZoomIn(float val);
	void GpadZoomOut(float val);
	void GpadAccept();
	void GpadDraw();
	void GpadDrawRelease();
	void GpadAcceptRelease();
	void GpadReturn();
	void GpadRemove();
	void GpadFiltersOn();
	void GpadFiltersOff();
	void GpadFiltersUp();
	void GpadFiltersDown();

#pragma endregion

public:

	UMapWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void InitData(APlayerCharacter* player, 
		FVector topLeftCornerWorldLocation,
		FVector bottomRightCornerWorldLocation,
		const TArray<class UObjectiveComponent*>& objectives,
		float zoomRatio,
		float maxZoom,
		float sideMoveSpeed,
		float sideMoveBorderWidth,
		int baseScale);

	
	/// <summary>
	/// Z component of world position is usually Z height of the level's terrain in (X, Y) position. If no terrain is found there,
	/// then world static objects are searched. If none are found, Z = 0.0f is set.
	/// </summary>
	/// <param name="mapPos"></param>
	/// <returns></returns>
	FVector MapToWorldCoords(const FVector2D& mapPos, bool calculateZ = true) const;
	void WorldToMapCoords(FVector2D& in) const;
	template <typename T> T* CloneWidget(T* src, UCanvasPanel* parent, FVector2D placePosition, FVector2D desiredSize = FVector2D::ZeroVector);
	template <typename T> T* CloneWidgetMapPosition(T* src, UCanvasPanel* parent, FVector2D placePosition, FVector2D desiredSize = FVector2D::ZeroVector);
	UMapIconImage* PlaceIconWidgetOnMap(EMapIconType type, UTexture2D* sprite, FVector2D initialLocationAbsolute, FVector2D size);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MapWidget")
	void Show();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MapWidget")
	void Hide();

	void ShowImmediately();
	void HideImmediately();

	void EnableDrawMode();
	void DisableDrawMode();

	TSharedPtr<SWidget> GetShowFocusWidget() const { return _widgetMapCanvasPanel->GetCanvasWidget(); }
	UCanvasPanel* GetPanelByIconType(EMapIconType type) const { return _widgetsIconPanels[(int32)type]; }
	APlayerCharacter* GetPlayer() const { return _player; }
	const FGeometry& GetGeometry() const { return _geometry; }
	const FPointerEvent& GetLastMouseEvent() const { return _lastMouseEvent; }
};
