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
	TIPCharacterYaw(0.f),
	TIPCharacterYawLastFrame(0.f),
	RootYawOffset(0.f),
	RotationCurve(0.f),
	RotationCurveLastFrame(0.f),
	Pitch(0.f),
	bReloading(false),
	OffsetState(EOffsetState::EOS_Hip),
	CharacterRotation(FRotator(0.f)),
	CharacterRotationLastFrame(FRotator(0.f)),
	RecoilWeight(1.f),
	EquippedWeaponType(EWeaponType::EWT_MAX),
	EquippedWeaponCombatState(ECombatState::ECS_MAX)
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

		bAiming = ShooterCharacter->GetIsAiming();
		SetOffsetState();
		bChangingWeapon = ShooterCharacter->GetIsChangingWeapon();
		bCrouching = ShooterCharacter->GetIsCrouching();
		

		// Check if ShooterCharacter has a valid equipped weapon
		if (ShooterCharacter->GetEquippedWeapon() != nullptr)
		{
			EquippedWeaponType = ShooterCharacter->GetEquippedWeapon()->GetWeaponType();
			EquippedWeaponCombatState = ShooterCharacter->GetEquippedWeapon()->GetCombatState();
		}
	}
	TurnInPlace();
	Lean(DeltaTime);
}

void UShooterCharacterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter != nullptr)
	{
		
		Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;
		// Dont turn in place while character is moving;
		if (Speed <= 0 && !bIsInAir)
		{
			TIPCharacterYawLastFrame = TIPCharacterYaw;
			TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
			const float TIPYawDelta = TIPCharacterYaw - TIPCharacterYawLastFrame;
			/*UE_LOG(LogTemp, Warning, TEXT("TIPCharacterYawLastFrame %f"), TIPCharacterYawLastFrame);
			UE_LOG(LogTemp, Warning, TEXT("TIPCharacterYaw %f"), TIPCharacterYaw);
			UE_LOG(LogTemp, Warning, TEXT("TIPYawDelta %f"), TIPYawDelta);
			UE_LOG(LogTemp, Warning, TEXT("RootYawOffset %f"), RootYawOffset);*/
			// Root yaw offset, updated and clamped [-180, 180] 
			RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TIPYawDelta);
			//UE_LOG(LogTemp, Warning, TEXT("RootYawOffset %f"), RootYawOffset);
			// 1.0 if turning, 0.0 if not
			const float Turning = GetCurveValue(TEXT("Turning"));
			if (Turning > 0)
			{
				bTurning = true;
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
			else // not turning in place
			{
				bTurning = false;
				
					
			}
			/* TODO DELETE
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(1, -1, FColor::Blue, FString::Printf(TEXT("CharacterYawLastFrame: %f"), TIPCharacterYawLastFrame));
				GEngine->AddOnScreenDebugMessage(2, -1, FColor::Red, FString::Printf(TEXT("Character Yaw: %f"), TIPCharacterYaw));
				GEngine->AddOnScreenDebugMessage(3, -1, FColor::Green, FString::Printf(TEXT("DELTA: %f"), TIPCharacterYaw));
				GEngine->AddOnScreenDebugMessage(4, -1, FColor::Purple, FString::Printf(TEXT("RootYawOffset: %f"), RootYawOffset));
			}*/
			
		}
		else
		{
			RootYawOffset = 0.f;
			TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
			RotationCurveLastFrame = TIPCharacterYaw;
			RotationCurveLastFrame = 0.f;
			RotationCurve = 0.f;
		}
		// Set recoil weight
		if (bTurning)
		{
			
			if (bReloading || bChangingWeapon)
			{
				RecoilWeight = 1.f;
			}
			else
			{
				RecoilWeight = 0.f;
			}
		}
		else
		{
			if (bCrouching)
			{
				if (bReloading || bChangingWeapon)
				{
					RecoilWeight = 1.f;
				}
				else
				{
					RecoilWeight = 0.1f;
				}
			}
			else
			{
				RecoilWeight = 1.f;
			}
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

void UShooterCharacterAnimInstance::Lean(float DeltaTime)
{
	if (ShooterCharacter != nullptr)
	{
		CharacterRotationLastFrame = CharacterRotation;
		CharacterRotation = ShooterCharacter->GetActorRotation();
		FRotator RotationDelta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);


		const float Target = RotationDelta.Yaw / DeltaTime;
		const float Interp = FMath::FInterpTo(YawDelta, Target, DeltaTime, 1.f);
		YawDelta = FMath::Clamp(Interp, -90.f, 90.f);
	}
	
}
