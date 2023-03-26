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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAcces = "true"))
	float MovementSpeed = 100.f;

	// Base Turn Rate, in deg/sec.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAcces = "true"))
	float TurnRate = 45.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAcces = "true"))
	class UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAcces = "true"))
	class UShooterCharacterInputConfigData* InputActions;

	// Handle movement input
	UFUNCTION()
	void MoveHandler(const FInputActionValue& value);

	// Handle look input
	UFUNCTION()
	void LookHandler(const FInputActionValue& value);

	// Handle jump input
	UFUNCTION()
	void JumpHandler(const FInputActionValue& value);

	// Handle crouch input
	UFUNCTION()
	void CrouchHandler(const FInputActionValue& value);

	// Handle reload input
	UFUNCTION()
	void ReloadHandler(const FInputActionValue& value);

	// Handle change weapon input
	void ChangeWeaponHandler(const FInputActionValue& value);

	// Handle fire input
	UFUNCTION()
	void FireHandler(const FInputActionValue& value);

	// Getting the beam end point
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& outBeamLocation);
};
