// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectGeo.h"
#include "Indicator.h"

AIndicator::AIndicator()
{
	UsingEnergyOrb = false;
}

void AIndicator::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	EnergyToCharge = EnergyCost;
}

/** Returns energy cost to interact with indicator */
float AIndicator::GetEnergyCost()
{
	return EnergyCost;
}

/** Returns drain time of indicator */
float AIndicator::GetDrainTime()
{
	return DrainTime;
}

/** Returns remaining energy necessary to fully charge indicator */
float AIndicator::GetEnergyToCharge()
{
	return EnergyToCharge;
}

/** Returns true if indicator has been charged */
bool AIndicator::GetCompletedCharge()
{
	return CompletedCharge;
}

/** Interacts with the indicator */
void AIndicator::Interact()
{
	Super::SetActive(false);

	if (UsingEnergyOrb)
	{
		// Energy orb currently being used to power indicator
		// Interacting will remove the orb and halt indicator charge progress
		UsingEnergyOrb = false;
	}
	else
	{
		// Indicator not currently being charged
		// Interacting will use the orb and begin/continue indicator charge progress
		UsingEnergyOrb = true;
	}
}

// Called when indicator is being charged
void AIndicator::UpdateEnergy(float EnergyChange)
{
	// Change power
	EnergyToCharge += EnergyChange;

	// Stop replenishing energy when maxed out
	if (EnergyToCharge <= 0)
	{
		CompletedCharge = true;
	}
}