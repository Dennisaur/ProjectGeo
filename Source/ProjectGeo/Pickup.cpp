// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectGeo.h"
#include "Pickup.h"

void APickup::Interact()
{
	Super::SetActive(false);

	// Destroy self
	Destroy();
}