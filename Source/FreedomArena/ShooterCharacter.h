// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class FREEDOMARENA_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE bool GetIsAiming() const { return bAiming; }
	
	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;
private:

	// Camera boom positioning the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// Camera that follows the character
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCam;

	// Randomized gunshot sound cue
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	// Is character aiming or not
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	// Default camera field of view value
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float CameraDefaultFOV;

	// Zoomed camera field of view value
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float CameraZoomedFOV;

	// Current camera field of view value for frame
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float CameraCurrentFOV;

	// Camera Zooming Interpolation Speed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	// Muzzle flash when shot at Muzzle Socket
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	// Montage for firing weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	// Particles spawned when bullet hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;
	
	// Smoke trail for bullets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	
public:

	//Returns the FollowCamera subobject
	FORCEINLINE UCameraComponent* GetFollowCam() const { return FollowCam; };

	// Returns the CameraBoom subobject 
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; };

protected:

	// Movement Speed 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAcces = "true"))
	float MovementSpeed = 100.f;

	// Scale factor for mouse turn sensivity while aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAcces = "true"), meta = (ClampMin = "0.0"), meta = (ClampMax = "1.0"))
	float MouseAimingSensScale;

	// Scale factor for mouse turn sensivity while not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAcces = "true"), meta = (ClampMin = "0.0"), meta = (ClampMax = "1.0"))
	float MouseHipSensScale;

	// Base Turn Rate, in deg/sec.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAcces = "true"))
	float TurnRate;

	// Turn rate while aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAcces = "true"))
	float AimingTurnRate;

	// Turn rate while not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAcces = "true"))
	float HipTurnRate;

	// Determines the spread of the crosshair
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAcces = "true"))
	float CrosshairSpreadMultiplier;
	
	// Velocity component for crosshair spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAcces = "true"))
	float CrosshairVelocityFactor;

	// In air component for crosshair spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAcces = "true"))
	float CrosshairInAirFactor;

	// Aim component for crosshair spread 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAcces = "true"))
	float CrosshairAimFactor;

	// SHooting component for crosshair spread 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAcces = "true"))
	float CrosshairShootingFactor;

	// Crosshair bullet fire factor timer variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAcces = "true"))
	float CrosshairShootTimeDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAcces = "true"))
	bool bCrosshairShooting;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAcces = "true"))
	FTimerHandle CrosshairShootTimer;


	// True when we can fire. False when waiting for shooting delay
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAcces = "true"))
	bool bCanShoot;
	
	// Delay between shots
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAcces = "true"))
	float ShootingDelay;

	// Sets timer between shots
	FTimerHandle ShootingTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAcces = "true"))
	class UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAcces = "true"))
	class UShooterCharacterInputConfigData* InputActions;

	/* INPUT HANDLER FUNCTIONS */
	// Handle movement input
	UFUNCTION()
	void MoveInputHandler(const FInputActionValue& value);

	// Handle look input
	UFUNCTION()
	void LookInputHandler(const FInputActionValue& value);

	// Handle jump input
	UFUNCTION()
	void JumpInputHandler(const FInputActionValue& value);

	// Handle crouch input
	UFUNCTION()
	void CrouchInputHandler(const FInputActionValue& value);

	// Handle reload input
	UFUNCTION()
	void ReloadInputHandler(const FInputActionValue& value);

	// Handle change weapon input
	void ChangeWeaponInputHandler(const FInputActionValue& value);

	// Handle fire input
	UFUNCTION()
	void FireInputHandler(const FInputActionValue& value);

	// Handle aim input to set aiming
	UFUNCTION()
	void AimingInputHandler(const FInputActionValue& value);
	/* ---------------------------------------------- */

	// Getting the beam end point
	UFUNCTION()
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& outBeamLocation);

	// Setting Current Camera FOV 
	UFUNCTION()
	void SetCameraFOV(float DeltaTime);

	// Setting Turn Rate
	UFUNCTION()
	void SetTurnRate();

	// Setting Mouse Sensivity Scale Factor
	UFUNCTION()
	void SetMouseSensScale(FVector2D& LookValue);

	// Set Crosshair spread multiplier
	UFUNCTION()
	void SetCrosshairSpread(float DeltaTime);

	// 
	UFUNCTION()
	void StartCrosshairShooting();
	// 
	UFUNCTION()
	void FinishCrosshairShooting();

	UFUNCTION()
	void SetCanShoot();
};
