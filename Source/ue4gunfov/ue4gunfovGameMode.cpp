// Copyright Epic Games, Inc. All Rights Reserved.

#include "ue4gunfovGameMode.h"

#include "DebugHud.h"
#include "ue4gunfovCharacter.h"
#include "UObject/ConstructorHelpers.h"

Aue4gunfovGameMode::Aue4gunfovGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ADebugHud::StaticClass();
}
