// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterCharacterAnimInstance.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_Aiming UMETA(DisplayName = "Aiming"),
	EOS_Hip UMETA(DisplayName = "Hip"),
	EOS_Reloading UMETA(DisplayName = "Reloading"),
	EOS_InAir UMETA(DisplayName = "InAir"),

	EOS_MAX UMETA(DisplayName = "DefaultMAX")
};
UCLASS()
class FREEDOMARENA_API UShooterCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UShooterCharacterAnimInstance();

	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	FORCEINLINE bool GetAiming() const { return bAiming; }

protected:
	// Handle turning in place variables
	UFUNCTION()
	void TurnInPlace();

	UFUNCTION()
	void SetOffsetState();

	// Handle calculations for leaning while running
	UFUNCTION()
	void Lean(float DeltaTime);

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* ShooterCharacter;

	// The speed of the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	float Speed;

	// Whether or not the character is in the air
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	// Whether or not the character is accelerating
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	// Offset yaw used for strafing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

	// Offset yaw the frame before stopped moving
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	// Yaw of the Character this frame but only updated when the character stand still and not in air
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float TIPCharacterYaw;

	// Yaw of the Character previous frame but only updated when the character stand still and not in air
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float TIPCharacterYawLastFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float RootYawOffset;

	// The pitch of the aim rotation, used for aim offset.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float Pitch;

	// True when reloading, used to prevent aim offset while reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	bool bReloading;

	// OffsetState used to determine which aim offset to use
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	EOffsetState OffsetState;

	// Rotation curve value this frame
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float RotationCurve;

	// Rotation curve value last frame
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float RotationCurveLastFrame;

	// Character Yaw This Frame
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lean", meta = (AllowPrivateAccess = "true"))
	FRotator CharacterRotation;

	// Character Yaw Last Frame
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lean", meta = (AllowPrivateAccess = "true"))
	FRotator CharacterRotationLastFrame;

	// Yaw delta used for leaning in the running blendspace
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lean", meta = (AllowPrivateAccess = "true"))
	float YawDelta;

	// True while turning
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lean", meta = (AllowPrivateAccess = "true"))
	bool bTurning;
	
	// True while crouching
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crouching", meta = (AllowPrivateAccess = "true"))
	bool bCrouching;

	// Change recoil weight based on offset state
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float RecoilWeight;
};
