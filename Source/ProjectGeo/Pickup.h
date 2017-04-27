// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Interactable.h"
#include "Pickup.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGEO_API APickup : public AInteractable
{
	GENERATED_BODY()
	
public:
	void Interact() override;
};
