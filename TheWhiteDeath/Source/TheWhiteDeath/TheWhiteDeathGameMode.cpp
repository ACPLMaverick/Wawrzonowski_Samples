// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TheWhiteDeath.h"
#include "TheWhiteDeathGameMode.h"
#include "TheWhiteDeathHUD.h"
#include "TheWhiteDeathCharacter.h"

ATheWhiteDeathGameMode::ATheWhiteDeathGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/Characters/Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ATheWhiteDeathHUD::StaticClass();
}
