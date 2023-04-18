// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterCharacterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon.h"

UShooterCharacterAnimInstance::UShooterCharacterAnimInstance() :
	Speed(0.f),
	bIsInAir(false),
	bIsAccelerating(false),
	MovementOffsetYaw(0.f),
	LastMovementOffsetYaw(0.f),
	bAiming(false),
	CharacterYaw(0.f),
	CharacterYawLastFrame(0.f),
	RootYawOffset(0.f),
	RotationCurve(0.f),
	RotationCurveLastFrame(0.f),
	Pitch(0.f),
	bReloading(false),
	OffsetState(EOffsetState::EOS_Hip)
{
}

void UShooterCharacterAnimInstance::NativeInitializeAnimation()
{
	// Get shooter character which own this anim instance
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());

}

void UShooterCharacterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		// Get shooter character which own this anim instance
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	if(ShooterCharacter != nullptr )
	{
		// Get the lateral speed of the character from velocity
		FVector Velocity = ShooterCharacter->GetVelocity();
		Velocity.Z = 0;
		Speed = Velocity.Size();

		// Is the character in the air
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		// Is the character accelerating
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Velocity);
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		if (!Velocity.IsZero())
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}
		/* TODO DELETE
		FString RotationMessage = FString::Printf(TEXT("BASE AIM ROTATION: %f"), AimRotation.Yaw);
		FString MovementMessage = FString::Printf(TEXT("BASE Movement ROTATION: %f"), MovementRotation.Yaw);
		FString MovementMessage2 = FString::Printf(TEXT("BASE Movement OFFSET: %f"), MovementOffsetYaw);
		if (GEngine != nullptr)
		{
			//GEngine->AddOnScreenDebugMessage(1, 0, FColor::White, RotationMessage);
			//GEngine->AddOnScreenDebugMessage(1, 0, FColor::White, MovementMessage);
			//GEngine->AddOnScreenDebugMessage(1, 0, FColor::White, MovementMessage2);
		}
		*/

		bAiming = ShooterCharacter->GetIsAiming();
		SetOffsetState();
		
	}
	TurnInPlace();
}

void UShooterCharacterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter != nullptr)
	{
		
		Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;
		// Dont turn in place while character is moving;
		if (Speed <= 0 && !bIsInAir)
		{
			CharacterYawLastFrame = CharacterYaw;
			CharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
			const float YawDelta = CharacterYaw - CharacterYawLastFrame;

			// Root yaw offset, updated and clamped [-180, 180] 
			RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - YawDelta);

			// 1.0 if turning, 0.0 if not
			const float Turning = GetCurveValue(TEXT("Turning"));
			if (Turning > 0)
			{
				RotationCurveLastFrame = RotationCurve;
				RotationCurve = GetCurveValue(TEXT("Rotation"));
				const float DeltaRotation = RotationCurve - RotationCurveLastFrame;

				// if RootYawOffset > 0, this means turning left. Otherwise, turning right.
				if (RootYawOffset > 0)
				{
					RootYawOffset -= DeltaRotation;
				}
				else if (RootYawOffset < 0)
				{
					RootYawOffset += DeltaRotation;
				}
				
				
				const float ABSRootYawOffset = FMath::Abs(RootYawOffset);
				if (ABSRootYawOffset > 90)
				{
					const float YawAxes = ABSRootYawOffset - 90.f;
					RootYawOffset > 0 ? RootYawOffset -= YawAxes : RootYawOffset += YawAxes;
				}
			}
			
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(1, -1, FColor::Blue, FString::Printf(TEXT("CharacterYawLastFrame: %f"), CharacterYawLastFrame));
				GEngine->AddOnScreenDebugMessage(2, -1, FColor::Red, FString::Printf(TEXT("Character Yaw: %f"), CharacterYaw));
				GEngine->AddOnScreenDebugMessage(3, -1, FColor::Green, FString::Printf(TEXT("DELTA: %f"), YawDelta));
				GEngine->AddOnScreenDebugMessage(4, -1, FColor::Purple, FString::Printf(TEXT("RootYawOffset: %f"), RootYawOffset));
			}
			
		}
		else
		{
			RootYawOffset = 0.f;
			CharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
			RotationCurveLastFrame = CharacterYaw;
			RotationCurveLastFrame = 0.f;
			RotationCurve = 0.f;
		}
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, -1, FColor::Blue, FString::Printf(TEXT("CharacterYawLastFrame: %f"), CharacterYawLastFrame));
			GEngine->AddOnScreenDebugMessage(2, -1, FColor::Red, FString::Printf(TEXT("Character Yaw: %f"), CharacterYaw));
			GEngine->AddOnScreenDebugMessage(4, -1, FColor::Purple, FString::Printf(TEXT("RootYawOffset: %f"), RootYawOffset));
		}
	}
}

void UShooterCharacterAnimInstance::SetOffsetState()
{
	AWeapon* CurrentWeapon = ShooterCharacter->GetEquippedWeapon();
	if (CurrentWeapon != nullptr)
	{
		bReloading = CurrentWeapon->GetCombatState() == ECombatState::ECS_Reloading;
	}
	if (bReloading)
	{
		OffsetState = EOffsetState::EOS_Reloading;
	}
	else if (bIsInAir)
	{
		OffsetState = EOffsetState::EOS_InAir;
	}
	else if (bAiming)
	{
		OffsetState = EOffsetState::EOS_Aiming;
	}
	else
	{
		OffsetState = EOffsetState::EOS_Hip;
	}
}
