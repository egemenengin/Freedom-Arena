// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "ShooterCharacterInputConfigData.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"
// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Dont rotate when the controller rotates. Let the controller only affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	// Create and setup CameraBoom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 60.f);

	// Create and setup FollowCam
	FollowCam = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCam->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach Camera to end of the CameraBoom
	FollowCam->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.f, 270.f, 0.f); // ... at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f; // Measure of how much control the character can have while in the air

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

	enhancedInputComponent->BindAction(InputActions->InputFire, ETriggerEvent::Started, this, &AShooterCharacter::FireHandler);
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
	// Shoting Sound 
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	// Get Muzzle Socket
	const USkeletalMeshSocket* MuzzleSocket = GetMesh()->GetSocketByName("muzzleSocket");

	if (MuzzleSocket != nullptr)
	{
		// Get Socket Transform
		const FTransform socketTransform = MuzzleSocket->GetSocketTransform(GetMesh());

		// Spawn Muzzle Flash
		if (MuzzleFlash != nullptr)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, socketTransform);
		}


		FVector BeamEndPoint;
		bool  bSetBeamEnd = GetBeamEndLocation(socketTransform.GetLocation(), BeamEndPoint);

		if (bSetBeamEnd) // was the setting BeamEndPoint successful?
		{

			// Spawn impact particles at end point
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamEndPoint);
			}

			if (BeamParticles)
			{
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, socketTransform);
				if (Beam != nullptr)
				{
					Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
				}
			
			}
		}
	}

	// HipFireMontage which is shoting animation
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	if (AnimInstance != nullptr && HipFireMontage != nullptr)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}

}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& outBeamLocation)
{
	// Get Current Size of the Viewport
	FVector2D ViewportSize;
	if (GEngine != nullptr && GEngine->GameViewport != nullptr)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}


	// Get Screen Space Location of Crosshairs
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	CrosshairLocation.Y -= 25.f;

	// Get World Position of Crosshairs
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if (bScreenToWorld) // was the deprojection successful?
	{
		FHitResult ScreenTraceHit;
		const FVector Start = CrosshairWorldPosition;
		const FVector End = CrosshairWorldPosition + CrosshairWorldDirection * 50'000.f;

		// Set beam end to line trace end point
		outBeamLocation = End;

		// Trace outward from crosshair world location
		GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, Start, End, ECollisionChannel::ECC_Visibility);

		if (ScreenTraceHit.bBlockingHit) // was there a trace hit?
		{
			// Set Beam End Point to Trace Hit Location
			outBeamLocation = ScreenTraceHit.Location;
		}

		// Perform second line trace, this time from the gun barrel 
		FHitResult WeaponHitResult;
		const FVector WeaponTraceStart = MuzzleSocketLocation;
		const FVector WeaponTraceEnd = outBeamLocation;
		GetWorld()->LineTraceSingleByChannel(WeaponHitResult, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);

		if (WeaponHitResult.bBlockingHit) // is there any object between barrel and BeamEndPoint?
		{
			outBeamLocation = WeaponHitResult.Location;
		}

		return true;
		
	}
	return false;
}

void AShooterCharacter::ReloadHandler(const FInputActionValue& value)
{

}

void AShooterCharacter::ChangeWeaponHandler(const FInputActionValue& value)
{

}