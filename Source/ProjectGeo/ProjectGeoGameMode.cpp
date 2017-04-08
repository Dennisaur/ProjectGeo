// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ProjectGeo.h"
#include "ProjectGeoGameMode.h"
#include "ProjectGeoHUD.h"
#include "ProjectGeoCharacter.h"

AProjectGeoGameMode::AProjectGeoGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AProjectGeoHUD::StaticClass();
}
