// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectGeo.h"
#include "Interactable.h"


// Sets default values
AInteractable::AInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// All interactables start active
	bIsActive = true;
}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractable::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Returns active state
bool AInteractable::IsActive()
{
	return bIsActive;
}

// Changes active state
void AInteractable::SetActive(bool NewActiveState)
{
	bIsActive = NewActiveState;
}

void AInteractable::Interact()
{

}