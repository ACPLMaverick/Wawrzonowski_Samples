// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "TheWhiteDeathHUD.generated.h"

UCLASS()
class ATheWhiteDeathHUD : public AHUD
{
	GENERATED_BODY()

public:
	ATheWhiteDeathHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

