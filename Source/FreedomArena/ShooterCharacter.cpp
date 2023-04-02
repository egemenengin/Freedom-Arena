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

#include "Weapon.h"
// Sets default values
AShooterCharacter::AShooterCharacter() :
	bAiming(false),
	TurnRate(0.f),
	// Camera field of view values
	CameraDefaultFOV(0.f), // set in BeginPlay
	CameraZoomedFOV(40.f),
	CameraCurrentFOV(0.f),
	ZoomInterpSpeed(20.f),
	// Turn rates for aiming or not 
	AimingTurnRate(20.f),
	HipTurnRate(90.f),
	// Mouse turn sensivity scale factors
	MouseAimingSensScale(0.25f),
	MouseHipSensScale(1.0f),
	// Crosshair Spread Factors
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimFactor(0.f),
	CrosshairShootingFactor(0.f),
	// Crosshair bullet fire factor timer variables
	CrosshairShootTimeDuration(0.05f),
	bCrosshairShooting(false),
	// Shooting delay variables
	bCanShoot(true),
	ShootingDelay(0.15f),
	// Trace variables
	ItemTraceHitRange(300.f),
	GunTraceHitRange(10'000.f),
	// Item trace variables
	bShouldTraceForItems(false)
	
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
	CameraBoom->TargetArmLength = 200.f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);

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
	
	if (FollowCam)
	{
		CameraDefaultFOV = FollowCam->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
	TurnRate = HipTurnRate;
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Setting Follow Camera FOV according to aiming
	SetCameraFOV(DeltaTime);

	// Set turn rate based on aiming
	SetTurnRate();

	// Set current crosshairspread
	SetCrosshairSpread(DeltaTime);

	// Check bShouldTraceForItems which means checking OverlappedItemCount and then trace for items 
	TraceForItems();
	
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
	enhancedInputComponent->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &AShooterCharacter::MoveInputHandler);
	enhancedInputComponent->BindAction(InputActions->InputLook, ETriggerEvent::Triggered, this, &AShooterCharacter::LookInputHandler);

	enhancedInputComponent->BindAction(InputActions->InputJump, ETriggerEvent::Started, this, &AShooterCharacter::JumpInputHandler);
	enhancedInputComponent->BindAction(InputActions->InputCrouch, ETriggerEvent::Started, this, &AShooterCharacter::CrouchInputHandler);

	enhancedInputComponent->BindAction(InputActions->InputFire, ETriggerEvent::Triggered, this, &AShooterCharacter::FireInputHandler);
	enhancedInputComponent->BindAction(InputActions->InputAiming, ETriggerEvent::Triggered, this, &AShooterCharacter::AimingInputHandler);
}

void AShooterCharacter::MoveInputHandler(const FInputActionValue& value)
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

void AShooterCharacter::LookInputHandler(const FInputActionValue& value)
{
	if (Controller != nullptr)
	{
		FVector2D LookValue = value.Get<FVector2D>();
		SetMouseSensScale(LookValue);
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

void AShooterCharacter::JumpInputHandler(const FInputActionValue& value)
{
	if (value.Get<float>() > 0)
	{
		Jump();
	}

}

void AShooterCharacter::CrouchInputHandler(const FInputActionValue& value)
{

}

void AShooterCharacter::FireInputHandler(const FInputActionValue& value)
{
	if (bCanShoot)
	{
		bCanShoot = false;
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

		// Set bCrosshairBulletFire to true and start crosshair bullet fire timer to set it false again.
		StartCrosshairShooting();


		GetWorldTimerManager().SetTimer(ShootingTimerHandle, this, &AShooterCharacter::SetCanShoot, ShootingDelay, false);

	}
}



bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& outBeamLocation)
{

	// Check for crosshair tracehit
	FHitResult CrosshairHitResult;
	bool bCrosshairTrace = TraceUnderCrosshair(CrosshairHitResult, GunTraceHitRange);
	if (bCrosshairTrace)
	{
		if (CrosshairHitResult.bBlockingHit)
		{
			outBeamLocation = CrosshairHitResult.Location;
		}
		else
		{
			outBeamLocation = CrosshairHitResult.TraceEnd;
			
		}
		// Perform second line trace, this time from the gun barrel 
		FHitResult WeaponHitResult;
		const FVector WeaponTraceStart = MuzzleSocketLocation;
		const FVector WeaponTraceEnd = outBeamLocation;
		GetWorld()->LineTraceSingleByChannel(WeaponHitResult, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);

		// TODO Delete
		//DrawDebugLine(GetWorld(), WeaponTraceStart, WeaponHitResult.Location, FColor::Red, true, 5.f);

		if (WeaponHitResult.bBlockingHit) // is there any object between barrel and BeamEndPoint?
		{
			outBeamLocation = WeaponHitResult.Location;
		}
		return true;
	}
	return false;
}



void AShooterCharacter::AimingInputHandler(const FInputActionValue& value)
{
	if (value.Get<float>() > 0.f)
	{
		bAiming = true;
	}
	else
	{
		bAiming = false;
	}

}

void AShooterCharacter::ReloadInputHandler(const FInputActionValue& value)
{

}

void AShooterCharacter::ChangeWeaponInputHandler(const FInputActionValue& value)
{

}


void AShooterCharacter::SetCameraFOV(float DeltaTime)
{
	// Set current field of view according to aiming or not
	if (bAiming)
	{
		// Interpolate to zoomed FOV
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
	}
	else
	{
		// Interpolate to default FOV
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
	}

	FollowCam->SetFieldOfView(CameraCurrentFOV);
}

void AShooterCharacter::SetTurnRate()
{
	// Set current Turn Rate according to aiming or not
	if (bAiming)
	{
		TurnRate = AimingTurnRate;
	}
	else
	{
		TurnRate = HipTurnRate;
	}
}

void AShooterCharacter::SetMouseSensScale(FVector2D& LookValue)
{
	if (bAiming)
	{
		LookValue = LookValue * MouseAimingSensScale;
	}
	else
	{
		LookValue = LookValue * MouseHipSensScale;
	}
}

void AShooterCharacter::SetCrosshairSpread(float DeltaTime)
{
	// Set velocity factor
	FVector2D WalkSpeedRange = FVector2D(0.f, 600.f);
	FVector2D VelocityMultiplierRange = FVector2D(0.f, 1.0f);
	FVector CurrentVelocity = GetVelocity();
	CurrentVelocity.Z = 0.f;
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, CurrentVelocity.Size());

	// Set InAir Factor
	if (GetCharacterMovement()->IsFalling()) // is in air?
	{
		// Spread the crosshair slowly while in air
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.f, DeltaTime, 2.f);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 15.f);
	}

	// Set Aim Factor
	if (bAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.5f, DeltaTime, 30.f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}

	// Set Bullet Fire Factor
	if (bCrosshairShooting)
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 30.f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 30.f);
	}

	CrosshairSpreadMultiplier = 
		0.5f + 
		CrosshairVelocityFactor + 
		CrosshairInAirFactor - 
		CrosshairAimFactor + 
		CrosshairShootingFactor;
}

void AShooterCharacter::StartCrosshairShooting()
{
	bCrosshairShooting = true;

	GetWorldTimerManager().SetTimer(
		CrosshairShootTimer, 
		this, 
		&AShooterCharacter::FinishCrosshairShooting,
		CrosshairShootTimeDuration, false);
}

void AShooterCharacter::FinishCrosshairShooting()
{
	bCrosshairShooting = false;
}

void AShooterCharacter::SetCanShoot()
{
	bCanShoot = !bCanShoot;
}


float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void AShooterCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	OverlappedItemCount += Amount;
	if (OverlappedItemCount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		bShouldTraceForItems = true;
	}
}

bool AShooterCharacter::TraceUnderCrosshair(FHitResult& OutHitResult, const float TraceRange)
{
	// Get viewport size
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// Get Screen Space Location of Crosshairs
	FVector2D CrosshairLocation = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);
	CrosshairLocation.Y -= 25.f;
	
	// Get World Position of Crosshairs
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this,0),
		CrosshairLocation, CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		//Trace from crosshair to world

		const FVector Start = CrosshairWorldPosition;
		const FVector End = Start + CrosshairWorldDirection * TraceRange;
		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);

		return true;
	}

	return false;
}

void AShooterCharacter::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		FHitResult ItemTraceResult;
		bool bTraceHit = TraceUnderCrosshair(ItemTraceResult, ItemTraceHitRange);
		if (ItemTraceResult.bBlockingHit)
		{
			AItem* HitItem = Cast<AItem>(ItemTraceResult.GetActor());
			if (HitItem != nullptr && HitItem->GetPickupWidget() != nullptr)
			{
				//Show Item's pickup widget
				HitItem->GetPickupWidget()->SetVisibility(true);
			}

			// check it hit an AItem last frame
			if (TraceHitItemLastFrame)
			{
				if (HitItem != TraceHitItemLastFrame)
				{
					// It is hitting a different AItem this fram from last frame or HitItem is null
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
				}
			}

			// Store a reference to HitItem for next frame
			TraceHitItemLastFrame = HitItem;
		}
		else if (TraceHitItemLastFrame)
		{
			TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
		}
	}
	
}
