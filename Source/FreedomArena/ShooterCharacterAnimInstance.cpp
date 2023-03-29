// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterCharacterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UShooterCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
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
	if(ShooterCharacter != nullptr)
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
		/*
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
		
	}
}
