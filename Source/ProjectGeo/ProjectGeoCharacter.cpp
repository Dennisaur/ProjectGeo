// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ProjectGeo.h"
#include "ProjectGeoCharacter.h"
#include "ProjectGeoGameMode.h"
#include "ProjectGeoProjectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "Pickup.h"
#include "EngineUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AProjectGeoCharacter

AProjectGeoCharacter::AProjectGeoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);
	
	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->Hand = EControllerHand::Right;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.
	
	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;

	// Set up character energy
	MaximumEnergy = 100.0f;
	InitialEnergy = 100.0f;
	CharacterEnergy = InitialEnergy;
	HasEnergyOrb = false;
	HasObtainedOrb = false;
	
	// Create collision sphere component to detect objects in range for action
	ActionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ActionSphere"));
	ActionSphere->SetupAttachment(RootComponent);
	ActionSphere->SetSphereRadius(150.0f);
}

void AProjectGeoCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}

	VR_Gun->SetHiddenInGame(true, true);
	Mesh1P->SetHiddenInGame(true, true);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AProjectGeoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AProjectGeoCharacter::TouchStarted);
	if (EnableTouchscreenMovement(PlayerInputComponent) == false)
	{
		PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AProjectGeoCharacter::OnFire);
	}

	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &AProjectGeoCharacter::OnAction);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AProjectGeoCharacter::OnResetVR);

	PlayerInputComponent->BindAxis("MoveForward", this, &AProjectGeoCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AProjectGeoCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AProjectGeoCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AProjectGeoCharacter::LookUpAtRate);

}

void AProjectGeoCharacter::OnFire()
{

	return;

	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<AProjectGeoProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<AProjectGeoProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AProjectGeoCharacter::OnAction()
{
	TArray<AActor*> ActorsInRange;
	ActionSphere->GetOverlappingActors(ActorsInRange);

	for (int32 iActor = 0; iActor < ActorsInRange.Num(); ++iActor)
	{
		// Cast the actor to APickup
		APickup* const TestPickup = Cast<APickup>(ActorsInRange[iActor]);
		if (!HasEnergyOrb && TestPickup)
		{
			// Display the HUD widget with energy bar for the first time picking up orb
			if (!HasObtainedOrb) {
				HasObtainedOrb = true;

				AProjectGeoGameMode* const GameMode = Cast<AProjectGeoGameMode>(GetWorld()->GetAuthGameMode());
				if (GameMode)
				{
					GameMode->ShowHUDWidget();
				}
			}

			// Pick up the energy orb
			HasEnergyOrb = true;

			// Halts indicator charge and stops draining energy
			if (CurrentIndicator)
			{
				DrainRate = 0;
				CurrentIndicator->SetUsingOrb(false);
				CurrentIndicator = NULL;
			}

			// Destroys the pickup
			TestPickup->Interact();

			break;
		}

		// If orb is already being used on an indicator or orb is recharging, skip checking for other indicators
		if (CurrentIndicator || IsSuperCharging)
		{
			continue;
		}
		// Place orb at current indicator
		else
		{
			// Cast the actor to AIndicator
			AIndicator* const TestIndicator = Cast<AIndicator>(ActorsInRange[iActor]);
			if (HasEnergyOrb && TestIndicator && !TestIndicator->GetCompletedCharge())
			{
				// Set this to current indicator
				CurrentIndicator = TestIndicator;
				CurrentIndicator->SetUsingOrb(true);

				// Spawn the orb actor at the indicator
				UWorld* const World = GetWorld();
				if (World)
				{
					FActorSpawnParameters SpawnParams;
					const FTransform IndicatorTransform = ActorsInRange[iActor]->GetTransform();
					APickup* const Orb = World->SpawnActor<APickup>(OrbPickup, IndicatorTransform, SpawnParams);
				}

				// Drop energy orb
				HasEnergyOrb = false;

				// Set drain rate
				if (CurrentIndicator->GetIsContinuous()) {
					DrainRate = CurrentIndicator->GetEnergyPerSecond();
				}
				else {
					float EnergyCost = CurrentIndicator->GetEnergyCost();
					float DrainTime = CurrentIndicator->GetDrainTime();
					DrainRate = EnergyCost / DrainTime;
				}
			}
			break;
		}
	}
}

void AProjectGeoCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AProjectGeoCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AProjectGeoCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AProjectGeoCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void AProjectGeoCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AProjectGeoCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AProjectGeoCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AProjectGeoCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AProjectGeoCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	bool bResult = false;
	if (FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		bResult = true;
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AProjectGeoCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AProjectGeoCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AProjectGeoCharacter::TouchUpdate);
	}
	return bResult;
}

// Returns starting energy
float AProjectGeoCharacter::GetInitialEnergy()
{
	return InitialEnergy;
}

// Returns current energy
float AProjectGeoCharacter::GetCurrentEnergy()
{
	return CharacterEnergy;
}

// Returns maximum energy
float AProjectGeoCharacter::GetMaximumEnergy()
{
	return MaximumEnergy;
}

// Returns true if character is supercharging
bool AProjectGeoCharacter::GetIsSuperCharging()
{
	return IsSuperCharging;
}

// Returns true if character has energy orb
bool AProjectGeoCharacter::GetHasEnergyOrb()
{
	return HasEnergyOrb;
}

// Returns true after character first picks up energy orb
bool AProjectGeoCharacter::GetHasObtainedOrb()
{
	return HasObtainedOrb;
}

// Called whenever energy is increased or decreased
void AProjectGeoCharacter::UpdateEnergy(float EnergyChange)
{
	// Change power
	CharacterEnergy += EnergyChange;

	// Stop replenishing energy when maxed out
	if (CharacterEnergy >= MaximumEnergy)
	{
		CharacterEnergy = MaximumEnergy;
		IsSuperCharging = false;
	}
}

// Called at each tick to drain energy based on drain rate
void AProjectGeoCharacter::DrainEnergy(float DeltaTime)
{
	float DrainAmount = DeltaTime * DrainRate;

	if (CurrentIndicator)
	{
		if (!CurrentIndicator->GetCompletedCharge()) {
			// Drain character energy
			UpdateEnergy(-DrainAmount);

			// Fill indicator charge
			CurrentIndicator->UpdateEnergy(DrainAmount);
		}

	}

	if (CharacterEnergy <= 0)
	{
		// Ran out of energy, start replenishing
		CharacterEnergy = 0;
		IsSuperCharging = true;
	}
	else if (CurrentIndicator && CurrentIndicator->GetCompletedCharge())
	{
		// Indicator fully charge, stop draining energy
	}
}

// Pick up energy orb
void AProjectGeoCharacter::PickupOrb()
{
	HasObtainedOrb = true;
	HasEnergyOrb = true;
}