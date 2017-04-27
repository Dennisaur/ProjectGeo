// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "Indicator.h"
#include "ProjectGeoCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class AProjectGeoCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* VR_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* L_MotionController;

public:
	AProjectGeoCharacter();

	virtual void BeginPlay();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AProjectGeoProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint32 bUsingMotionControllers : 1;

protected:

	/** Fires a projectile. */
	void OnFire();

	/** Performs an action. */
	void OnAction();

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }


/** New functions/variables */
public:
	// Sphere component to determine nearby actionable objects
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* ActionSphere;

	UPROPERTY(EditDefaultsOnly, Category = "Energy")
		TSubclassOf<class APickup> OrbPickup;

	/** Accessor function for initial energy */
	UFUNCTION(BlueprintPure, Category = "Energy")
		float GetInitialEnergy();

	/** Accessor function for maximum energy */
	UFUNCTION(BlueprintPure, Category = "Energy")
		float GetMaximumEnergy();

	/** Accessor function for current energy */
	UFUNCTION(BlueprintPure, Category = "Energy")
		float GetCurrentEnergy();

	/** Accessor function for if replenishing energy */
	UFUNCTION(BlueprintPure, Category = "Energy")
		bool GetIsReplenishing();

	/** Accessor function for if replenishing energy */
	UFUNCTION(BlueprintPure, Category = "Energy")
		bool GetIsDraining();

	/** Accessor function for if character has energy orb */
	UFUNCTION(BlueprintPure, Category = "Energy")
		bool GetHasEnergyOrb();

	/** Accessor function for if character has obtained the orb */
	UFUNCTION(BlueprintPure, Category = "Energy")
		bool GetHasObtainedOrb();

	/**
	Function to update the character's energy
	* @param EnergyChange This is the amount to change the energy by. It can be positive or negative.
	*/
	UFUNCTION(BlueprintCallable, Category = "Energy")
		void UpdateEnergy(float EnergyChange);

	UFUNCTION(BlueprintCallable, Category = "Energy")
		void DrainEnergy(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Energy")
		void PickupOrb();

protected:
	/** Starting energy level of the character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
		float InitialEnergy;

	/** Maximum energy level of the character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
		float MaximumEnergy;

private:
	UPROPERTY(VisibleAnywhere, Category = "Energy")
		float CharacterEnergy;

	UPROPERTY(VisibleAnywhere, Category = "Energy")
		bool IsReplenishing;

	UPROPERTY(VisibleAnywhere, Category = "Energy")
		bool IsDraining;

	UPROPERTY(VisibleAnywhere, Category = "Energy")
		bool HasEnergyOrb;

	UPROPERTY(VisibleAnywhere, Category = "Energy")
		bool HasObtainedOrb;

	class AIndicator* CurrentIndicator;
	float DrainRate;

};

