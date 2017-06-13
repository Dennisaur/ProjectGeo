// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectGeo.h"
#include "Indicator.h"

AIndicator::AIndicator()
{
	UsingEnergyOrb = false;
	Energy = 0;
}

void AIndicator::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

/** Returns true if indicator will continuously use energy */
bool AIndicator::GetIsContinuous()
{
	return IsContinuous;
}

/** Returns energy used per second */
float AIndicator::GetEnergyPerSecond()
{
	return EnergyPerSecond;
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
float AIndicator::GetEnergy()
{
	return Energy;
}

/** Returns true if indicator is currently using the energy orb */
bool AIndicator::GetUsingOrb()
{
	return UsingEnergyOrb;
}

void AIndicator::SetUsingOrb(bool UsingOrb) {
	UsingEnergyOrb = UsingOrb;
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
	// Change energy
	Energy += EnergyChange;

	// Stop filling energy when maxed out
	if (!IsContinuous && Energy >= EnergyCost)
	{
		Energy = EnergyCost;
		CompletedCharge = true;

		UE_LOG(LogClass, Log, TEXT("You have %d energy"), Energy);
	}
}