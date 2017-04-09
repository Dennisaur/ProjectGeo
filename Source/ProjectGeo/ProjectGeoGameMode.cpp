// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ProjectGeo.h"
#include "ProjectGeoGameMode.h"
#include "ProjectGeoHUD.h"
#include "ProjectGeoCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

AProjectGeoGameMode::AProjectGeoGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AProjectGeoHUD::StaticClass();

	PrimaryActorTick.bCanEverTick = true;

	// Base replenish rate
	ReplenishRate = 1.0f;
}

void AProjectGeoGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

void AProjectGeoGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AProjectGeoCharacter* MyCharacter = Cast<AProjectGeoCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (MyCharacter)
	{
		float currentEnergy = MyCharacter->GetCurrentEnergy();
		bool isReplenishing = MyCharacter->GetIsReplenishing();
		bool isDraining = MyCharacter->GetIsDraining();

		if (isReplenishing && currentEnergy < 100.f)
		{
			// Replenish energy
			MyCharacter->UpdateEnergy(DeltaTime * ReplenishRate);
		}
		else if (isDraining && currentEnergy >= 0)
		{
			// Drain energy
			MyCharacter->DrainEnergy(DeltaTime);
		}
	}
}
