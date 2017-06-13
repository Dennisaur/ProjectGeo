// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameModeBase.h"
#include "ProjectGeoGameMode.generated.h"

UCLASS(minimalapi)
class AProjectGeoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AProjectGeoGameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void ShowHUDWidget();


protected:
	/** The rate at which the character replenishes energy */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
		float ReplenishRate;

	/** The rate at which the character replenishes energy after fully depleted */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
		float SuperChargeRate;

	/** The Widget class to use for HUD screen */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
		TSubclassOf<class UUserWidget> HUDWidgetClass;

	/** The instance of the HUD */
	UPROPERTY()
		class UUserWidget* CurrentWidget;

};



