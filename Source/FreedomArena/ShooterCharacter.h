// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponEnums.h"
#include "ShooterCharacter.generated.h"

USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()
	// Scene component to use for its location for interping
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	USceneComponent* SceneComponent;

	// Number of items interping to/at this scene component location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;

	FInterpLocation()
	{
		ItemCount = 0;
	}

	FInterpLocation(USceneComponent* sceneComponent, int32 itemCount)
	{
		SceneComponent = sceneComponent;
		ItemCount = itemCount;
	}
};
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

	virtual void Jump() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

private:

	// Camera boom positioning the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// Camera that follows the character
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCam;

	// Is character aiming or not
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	// Is aiming button pressed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bAimingButtonPressed;

	// Default camera field of view value
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraDefaultFOV;

	// Zoomed camera field of view value
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraZoomedFOV;

	// Current camera field of view value for frame
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraCurrentFOV;

	// Camera Zooming Interpolation Speed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	// Distance outward from the camera to item for interpolation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effects, meta = (AllowPrivateAccess = "true"))
	float ItemInterpOutwardDistance;

	// Distance Upward from the camera to item for interpolation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effects, meta = (AllowPrivateAccess = "true"))
	float ItemInterpUpDistance;

	// Randomized gunshot sound cue
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	// Muzzle flash when shot at Muzzle Socket
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	// Montage for firing weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	// Montage for reloading
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* ReloadMontage;

	// Particles spawned when bullet hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;
	
	// Smoke trail for bullets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EquipItem, meta = (AllowPrivateAccess = "true"))
	USceneComponent* WeaponInterpComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EquipItem, meta = (AllowPrivateAccess = "true"))
	USceneComponent* FirstInterpComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EquipItem, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SecondInterpComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EquipItem, meta = (AllowPrivateAccess = "true"))
	USceneComponent* ThirdInterpComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EquipItem, meta = (AllowPrivateAccess = "true"))
	USceneComponent* FourthInterpComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EquipItem, meta = (AllowPrivateAccess = "true"))
	USceneComponent* FifthInterpComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EquipItem, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SixthInterpComp;

	// Array of interp location structs
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EquipItem, meta = (AllowPrivateAccess = "true"))
	TArray<FInterpLocation> InterpLocations;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EquipItem, meta = (AllowPrivateAccess = "true"))
	FTimerHandle PickupSoundTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EquipItem, meta = (AllowPrivateAccess = "true"))
	FTimerHandle EquipSoundTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EquipItem, meta = (AllowPrivateAccess = "true"))
	bool bShouldPlayPickupSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EquipItem, meta = (AllowPrivateAccess = "true"))
	bool bShouldPlayEquipSound;

	// Time to wait before another sound is played
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EquipItem, meta = (AllowPrivateAccess = "true"))
	float SoundResetTime;

	
public:
	// GETTERS
	//Returns the FollowCamera subobject
	FORCEINLINE UCameraComponent* GetFollowCam() const { return FollowCam; };

	// Returns the CameraBoom subobject 
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; };

	FORCEINLINE bool GetIsAiming() const { return bAiming; }

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }
	FORCEINLINE bool GetIsCrouching() const { return bCrouching; }

	FORCEINLINE bool GetShouldPlayEquipSound() const { return bShouldPlayEquipSound; }
	FORCEINLINE bool GetShouldPlayPickupSound() const { return bShouldPlayPickupSound; }
	FORCEINLINE float GetSoundResetTime() const { return SoundResetTime; }

	AWeapon* GetEquippedWeapon();

	// Adds/subtracts to/from OverlappedItemCount and updates bShouldTraceForItems;
	void IncrementOverlappedItemCount(int8 Amount);

	// TODO DELETE No longer needed. AItem has GetInterpLocation
	//FVector GetCameraInterpLocation();

protected:

	// Movement Speed 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAcces = "true"))
	float MovementSpeed;

	// Movement Speed 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAcces = "true"))
	float WalkSpeed;

	// Crouch Movement Speed 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAcces = "true"))
	float CrouchSpeed;

	// Ground Friction while not crouching
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAcces = "true"))
	float BaseGroundFriction;
	
	// Ground Friction while crouching
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAcces = "true"))
	float CrouchingGroundFriction;

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

	// True while crouching
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAcces = "true"))
	bool bCrouching;
	
	// Current half height of the capsule
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAcces = "true"))
	float CurrentCapsuleHalfHeight;

	// Half height of the capsule while not crouching
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAcces = "true"))
	float StandingCapsuleHalfHeight;

	// Half height of the capsule while crouching
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAcces = "true"))
	float CrouchingCapsuleHalfHeight;

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
	
	// Delay between shots
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAcces = "true"))
	float ShootingDelay;

	// Sets timer between shots
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FTimerHandle ShootingTimerHandle;

	// Sets timer for reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FTimerHandle ReloadingTimerHandle;

	// True if we should trace every frame for items
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Trace, meta = (AllowPrivateAccess = "true"))
	bool bShouldTraceForItems;
	
	// Number of overlapped AItems
	int8 OverlappedItemCount;

	// The AItem that trace hit last frame
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Trace, meta = (AllowPrivateAccess = "true"))
	class AItem* TraceHitItemLastFrame;

	// Range that player can see AItem
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float ItemTraceHitRange;

	// Range that player can see AItem
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float GunTraceHitRange;

	// Currently equipped weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAcces = "true"))
	class AWeapon* EquippedWeapon;


	// Set this in BP for weapon class
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAcces = "true"))
	TSubclassOf<AWeapon> WeaponClass;

	// Mapping Ammo types and number of them
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAcces = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	// Starting amount of Light ammo
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAcces = "true"))
	int32 StartingLightAmmo;

	// Starting amount of Heavy ammo
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAcces = "true"))
	int32 StartingHeavyAmmo;


	// Transform of the clip when magazine is grabbed first time during reloading
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAcces = "true"))
	FTransform MagazineTransform;

	// Scene component to attach to the character's hand during reloading
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAcces = "true"))
	USceneComponent* HandSceneComponent;
	
	// Input mapping
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAcces = "true"))
	class UInputMappingContext* InputMapping;

	// Hold input actions
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

	// Handle pickup an item input
	UFUNCTION()
	void PickupItemInputHandler(const FInputActionValue& value);

	// Handle drop an weapon input
	UFUNCTION()
	void DropItemInputHandler(const FInputActionValue& value);
	/* ---------------------------------------------- */
	//Fire Weapon Funcitons
	UFUNCTION()
	void PlayFireSound();
	UFUNCTION()
	void SendBullet();
	UFUNCTION()
	void PlayHipFireMontage();
	// Getting the beam end point
	UFUNCTION()
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& outBeamLocation);

	// Setting Current Camera FOV 
	UFUNCTION()
	void SetCameraFOV(float DeltaTime);

	// Setting Turn Rate
	UFUNCTION()
	void SetTurnRate();

	// Interps capsule half height while crouching/standing
	UFUNCTION()
	void InterpCapsuleHalfHeight(float DeltaTime);

	// Setting Mouse Sensivity Scale Factor
	UFUNCTION()
	void SetMouseSensScale(FVector2D& LookValue);

	// Set Crosshair spread multiplier
	UFUNCTION()
	void SetCrosshairSpread(float DeltaTime);

	// Set bCrosshairShooting to true
	UFUNCTION()
	void StartCrosshairShooting();

	// Set bCrosshairShooting to false
	UFUNCTION()
	void FinishCrosshairShooting();

	// Set bCanShoot to the opposite of its current value
	UFUNCTION()
	void SetCanShoot();

	// Set combat state after reloading
	UFUNCTION()
	void ReloadFinished();

	// Call from Animation blueprint with grab mag notify
	UFUNCTION(BlueprintCallable)
	void GrabMag();

	// Call from Animation blueprint with replace mag notify
	UFUNCTION(BlueprintCallable)
	void ReplaceMag();
	// Set reload montage section according to equipped weapon type 
	UFUNCTION()
	void SetReloadMontageSection(FName& MontageSection);

	// Line trace for items under the crosshair
	UFUNCTION()
	bool TraceUnderCrosshair(FHitResult& OutHitResult, const float TraceRange);

	// Trace for items if bShouldTraceForItems is true which means OverlappedItemCount > 0
	UFUNCTION()
	void TraceForItems();

	// Spawns a weapon
	UFUNCTION()
	AWeapon* SpawnDefaultWeapon();

	// Takes a weapon, attaches it to mesh and equips it
	UFUNCTION()
	void EquipWeapon(AWeapon* WeaponToEquip);

	// Detach weapon and let it fall to the ground
	UFUNCTION()
	void DropWeapon();

	// Drops currently equipped weapon and equips weapon that trace hit
	UFUNCTION()
	void SwapWeapon(AWeapon* WeaponToSwap);
	
	UFUNCTION()
	void SetAiming(bool AimButonPressed);

	// Adding ammo to character's ammo amount according to its ammo type
	UFUNCTION()
	void TakeAmmo(class AAmmo* Ammo);

	UFUNCTION()
	void InitializeInterpLocations();

	
public:
	UFUNCTION()
	void GetPickupItem(AItem* item);

	UFUNCTION()
	FInterpLocation GetInterpLocation(int32 index);

	// Returns the index in InterpLocations array with the lowest Item Count
	UFUNCTION()
	int32 GetInterpLocationIndex();

	UFUNCTION()
	void HandleInterpLocItemCount(int32 index, int32 itemAmount);

	UFUNCTION()
	void StartSoundTimer(EItemState itemState);

	UFUNCTION()
	void ResetPickupSoundTimer();

	UFUNCTION()
	void ResetEquipSoundTimer();
};
