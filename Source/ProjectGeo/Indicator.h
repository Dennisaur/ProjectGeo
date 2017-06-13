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
		bool GetIsContinuous();

	UFUNCTION(BlueprintPure, Category = "Energy")
		float GetEnergyPerSecond();

	UFUNCTION(BlueprintPure, Category = "Energy")
		float GetEnergyCost();

	UFUNCTION(BlueprintPure, Category = "Energy")
		float GetDrainTime();

	UFUNCTION(BlueprintPure, Category = "Energy")
		float GetEnergy();

	UFUNCTION(BlueprintPure, Category = "Energy")
		bool GetCompletedCharge();

	UFUNCTION(BlueprintPure, Category = "Energy")
		bool GetUsingOrb();


	UFUNCTION(BlueprintCallable, Category = "Energy")
		void SetUsingOrb(bool UsingOrb);

	void Interact() override;

	void UpdateEnergy(float EnergyChange);

protected:
	/** Set to true if indicator will use energy continuously */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
		bool IsContinuous;

	/** Set the amount of energy used per second */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
		float EnergyPerSecond;

	/** Set the amount of energy required to interact with this object */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
		float EnergyCost;

	/** Energy remaining for indicator to fully charge */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
		float Energy;

	/** Set the energy drain time of this object */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
		float DrainTime;

	/** Whether the indicator has been fully charged with energy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
		bool CompletedCharge;

	/** Whether the indicator is currently powering up with the energy orb */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
		bool UsingEnergyOrb;

};
