// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

UCLASS()
class PROJECTGEO_API AInteractable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractable();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** Return whether or not the interactable object is active */
	UFUNCTION(BlueprintPure, Category = "Interactable")
		bool IsActive();

	/** Allows other classes to safely change whether or not the interactable is active */
	UFUNCTION(BlueprintCallable, Category = "Interactable")
		void SetActive(bool NewActiveState);

	/** Function to designate behavior when player interacts with this object */
	UFUNCTION(BlueprintCallable, Category = "Interactable")
		virtual void Interact();

protected:
	/** True when the interactable object can be used; false when deactivated */
	bool bIsActive;
};
