// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "ShooterCharacterInputConfigData.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create and setup CameraBoom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create and setup FollowCam
	FollowCam = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCam->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach Camera to end of the CameraBoom
	FollowCam->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Get player controller
	APlayerController* playerController = Cast<APlayerController>(GetController());

	// Get enhanced input local player subsystem
	UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer());

	// Clear out existing mappings and add its own mapping
	subsystem->ClearAllMappings();
	subsystem->AddMappingContext(InputMapping, 0);

	// Get EnhancedInputComponent
	UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	// Bind Input Actions
	enhancedInputComponent->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &AShooterCharacter::MoveHandler);
	enhancedInputComponent->BindAction(InputActions->InputLook, ETriggerEvent::Triggered, this, &AShooterCharacter::LookHandler);
	enhancedInputComponent->BindAction(InputActions->InputJump, ETriggerEvent::Started, this, &AShooterCharacter::JumpHandler);
	enhancedInputComponent->BindAction(InputActions->InputCrouch, ETriggerEvent::Started, this, &AShooterCharacter::CrouchHandler);
}

void AShooterCharacter::MoveHandler(const FInputActionValue& value)
{
	if (Controller != nullptr)
	{
		const FVector2D MoveValue = value.Get<FVector2D>();
		const FRotator MovementRotation(0, Controller->GetControlRotation().Yaw, 0);
		if (MoveValue.Y != 0)
		{
			// Find out which way is forward
			//const FVector Direction = MovementRotation.RotateVector(FVector::ForwardVector);
			const FVector Direction = FRotationMatrix(MovementRotation).GetUnitAxis(EAxis::X);

			AddMovementInput(Direction, MoveValue.Y * GetWorld()->GetDeltaSeconds() * MovementSpeed);
		}
		if (MoveValue.X != 0)
		{
			// Find out which way is right
			//const FVector Direction = MovementRotation.RotateVector(FVector::RightVector);
			const FVector Direction = FRotationMatrix(MovementRotation).GetUnitAxis(EAxis::Y);

			AddMovementInput(Direction, MoveValue.X * GetWorld()->GetDeltaSeconds() * MovementSpeed);
		}
	}
}

void AShooterCharacter::LookHandler(const FInputActionValue& value)
{
	if (Controller != nullptr)
	{
		const FVector2D LookValue = value.Get<FVector2D>();

		if (LookValue.Y != 0.f)
		{
			// Calculate delta for this frame for the rate information 
			AddControllerPitchInput(-1 * LookValue.Y * TurnRate * GetWorld()->GetDeltaSeconds()); // deg/sec * sec/frame
		}
		if (LookValue.X != 0.f)
		{
			// Calculate delta for this frame for the rate information
			AddControllerYawInput(LookValue.X * TurnRate * GetWorld()->GetDeltaSeconds());
		}
	}
}

void AShooterCharacter::JumpHandler(const FInputActionValue& value)
{
	if (value.Get<float>() > 0)
	{
		Jump();
	}

}

void AShooterCharacter::CrouchHandler(const FInputActionValue& value)
{

}

void AShooterCharacter::FireHandler(const FInputActionValue& value)
{

}

void AShooterCharacter::ReloadHandler(const FInputActionValue& value)
{

}

void AShooterCharacter::ChangeWeaponHandler(const FInputActionValue& value)
{

}