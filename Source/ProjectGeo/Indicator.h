// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Interactable.h"
#include "Indicator.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGEO_API AIndicator : public AInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AIndicator();

	virtual void BeginPlay();

	UFUNCTION(BlueprintPure, Category = "Energy")
		float GetEnergyCost();

	UFUNCTION(BlueprintPure, Category = "Energy")
		float GetDrainTime();

	UFUNCTION(BlueprintPure, Category = "Energy")
		float GetEnergyToCharge();

	UFUNCTION(BlueprintPure, Category = "Energy")
		bool GetCompletedCharge();


	void Interact() override;

	void UpdateEnergy(float EnergyChange);

protected:
	/** Set the amount of energy required to interact with this object */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
		float EnergyCost;

	/** Energy remaining for indicator to fully charge */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
		float EnergyToCharge;

	/** Set the energy drain time of this object */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
		float DrainTime;

	/** Whether the indicator has been fully charged with energy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
		bool CompletedCharge;

private:

	/** Whether the indicator is currently powering up with the energy orb */
	bool UsingEnergyOrb;

};
