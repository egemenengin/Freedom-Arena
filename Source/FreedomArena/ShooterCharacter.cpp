// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/DamageType.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "ShooterCharacterInputConfigData.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "BulletHitInterface.h"
#include "Animation/AnimInstance.h"
#include "Weapon.h"
#include "Ammo.h"
#include "FreedomArena.h"
#include "Enemy.h"

#include "PhysicalMaterials/PhysicalMaterial.h"

// Sets default values
AShooterCharacter::AShooterCharacter() :
	MovementSpeed(100.f),
	WalkSpeed(600.f),
	CrouchSpeed(300.f),
	BaseGroundFriction(2.f),
	CrouchingGroundFriction(25.f),
	bAiming(false),
	bAimingButtonPressed(false),
	TurnRate(0.f),
	bCrouching(false),
	CurrentCapsuleHalfHeight(88.f),
	StandingCapsuleHalfHeight(88.f),
	CrouchingCapsuleHalfHeight(44.f),
	// Camera field of view values
	CameraDefaultFOV(0.f), // set in BeginPlay
	CameraZoomedFOV(40.f),
	CameraCurrentFOV(0.f),
	ZoomInterpSpeed(20.f),
	// Item Interp Variables
	ItemInterpOutwardDistance(200.f),
	ItemInterpUpDistance(50.f),
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
	// Trace variables
	ItemTraceHitRange(400.f),
	GunTraceHitRange(10'000.f),
	// Item trace variables
	bShouldTraceForItems(false),
	// Ammo
	StartingLightAmmo(90),
	StartingHeavyAmmo(90),
	bShouldPlayPickupSound(true),
	bShouldPlayEquipSound(true),
	SoundResetTime(1.f),
	bChangingWeapon(false),
	// Inventory
	bInventoryFull(false),
	HighlightedSlot(-1),
	bIsJustShooted(false),
	// Health
	MaxHealth(100.f),
	CurrentHealth(100.f)
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
	CameraBoom->SocketOffset = FVector(0.f, 40.f, 70.f);

	// Create and setup FollowCam
	FollowCam = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCam->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach Camera to end of the CameraBoom
	FollowCam->bUsePawnControlRotation = false; // Camera rotate relative to arm

	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandComponent"));

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.f, 270.f, 0.f); // ... at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f; // Measure of how much control the character can have while in the air

	// Create Interpolation Components
	WeaponInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Interpolation Component"));
	WeaponInterpComp->SetupAttachment(FollowCam);

	FirstInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("First Interpolation Component"));
	FirstInterpComp->SetupAttachment(FollowCam);

	SecondInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("Second Interpolation Component"));
	SecondInterpComp->SetupAttachment(FollowCam);

	ThirdInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("Third Interpolation Component"));
	ThirdInterpComp->SetupAttachment(FollowCam);

	FourthInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("Fourth Interpolation Component"));
	FourthInterpComp->SetupAttachment(FollowCam);

	FifthInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("Fifth Interpolation Component"));
	FifthInterpComp->SetupAttachment(FollowCam);

	SixthInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("Sixth Interpolation Component"));
	SixthInterpComp->SetupAttachment(FollowCam);

	TeamId = FGenericTeamId(0);
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;

	if (FollowCam)
	{
		CameraDefaultFOV = FollowCam->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
	TurnRate = HipTurnRate;

	// Hide the gun which is attached to skeleton
	GetMesh()->HideBoneByName(TEXT("weapon"), EPhysBodyOp::PBO_None);

	EquipWeapon(SpawnDefaultWeapon());
	Inventory.Add(EquippedWeapon);
	EquippedWeapon->SetWeaponSlotIndex(0);

	EquippedWeapon->ToggleGlowMaterial(false);
	EquippedWeapon->ToggleCustomDepth(false);

	EquippedWeapon->SetCharacter(this);

	AmmoMap.Add(EAmmoType::EAT_Light, StartingLightAmmo);
	AmmoMap.Add(EAmmoType::EAT_Heavy, StartingHeavyAmmo);
	AmmoMap.Add(EAmmoType::EAT_None, 0);

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// Create FInterpLocation structs for each interp location. Add to array
	InitializeInterpLocations();
}

void AShooterCharacter::Jump()
{
	if (bCrouching)
	{
		bCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
	else
	{
		ACharacter::Jump();
	}
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
	
	//Interpolate the capsule half height based on crouching/standing
	InterpCapsuleHalfHeight(DeltaTime);

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
	enhancedInputComponent->BindAction(InputActions->InputRelaod, ETriggerEvent::Started, this, &AShooterCharacter::ReloadInputHandler);

	enhancedInputComponent->BindAction(InputActions->InputPickupItem, ETriggerEvent::Started, this, &AShooterCharacter::PickupItemInputHandler);
	enhancedInputComponent->BindAction(InputActions->InputDropItem, ETriggerEvent::Started, this, &AShooterCharacter::DropItemInputHandler);

	enhancedInputComponent->BindAction(InputActions->InventorySelection, ETriggerEvent::Started, this, &AShooterCharacter::InventorySelectInputHandler);
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
	if ( (value.Get<float>() > 0) && (!GetCharacterMovement()->IsFalling()) )
	{
		bCrouching = !bCrouching;
	}
	if (bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
		GetCharacterMovement()->GroundFriction = CrouchingGroundFriction;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		GetCharacterMovement()->GroundFriction = BaseGroundFriction;
	}
}

void AShooterCharacter::FireInputHandler(const FInputActionValue& value)
{
	if (value.Get<float>() > 0)
	{
		if ( (EquippedWeapon != nullptr) &&
			(EquippedWeapon->GetCombatState() == ECombatState::ECS_CanShoot) &&
			(bIsJustShooted == false))
		{
			bIsJustShooted = true;
			PlayFireSound();

			SendBullet();
			
			PlayHipFireMontage();

			// Set bCrosshairBulletFire to true and start crosshair bullet fire timer to set it false again.
			StartCrosshairShooting();

			// Subtract 1 from the weapon's ammo
			EquippedWeapon->DecrementAmmo();

			EquippedWeapon->SetCombatState(ECombatState::ECS_ShootingCooldown);

			GetWorldTimerManager().SetTimer(ShootingTimerHandle, this, &AShooterCharacter::SetCanShoot, EquippedWeapon->GetFireRate(), false);
			
			if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol)
			{
				EquippedWeapon->StartSlideTimer();
			}
			EquippedWeapon->StartRecoil();
		}
		else if (EquippedWeapon->GetCombatState() != ECombatState::ECS_OutOfAmmo)
		{
			// Play out of ammo sound
		}
	}
	else
	{
		bIsJustShooted = false;
	}

}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult)
{

	// Check for crosshair tracehit
	FHitResult CrosshairHitResult;
	FVector OutBeamLocation;
	// TODO: GunTraceHitRange = EquippedWeapon->GetRange();

	bool bCrosshairTrace = TraceUnderCrosshair(CrosshairHitResult, GunTraceHitRange);
	
	if (bCrosshairTrace)
	{
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else
	{
		OutBeamLocation = CrosshairHitResult.TraceEnd;
			
	}
	// Perform second line trace, this time from the gun barrel 
	const FVector WeaponTraceStart = MuzzleSocketLocation;
	const FVector WeaponTraceEnd = OutBeamLocation;
	GetWorld()->LineTraceSingleByChannel(OutHitResult, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);

	// TODO Delete
	//DrawDebugLine(GetWorld(), WeaponTraceStart, WeaponHitResult.Location, FColor::Red, true, 5.f);
		
	if (!OutHitResult.bBlockingHit) // is there any object between barrel and BeamEndPoint?
	{
		return false;
	}
	return true;
	

}

void AShooterCharacter::AimingInputHandler(const FInputActionValue& value)
{
	if (value.Get<float>() > 0.f)
	{

		bAimingButtonPressed = true;
		
	}
	else
	{
		bAimingButtonPressed = false;
	}
	SetAiming(bAimingButtonPressed);


}

void AShooterCharacter::PickupItemInputHandler(const FInputActionValue& value)
{
	if (value.Get<float>() > 0)
	{
		// TODO pickup item
		if ( (TraceHitItemLastFrame != nullptr) &&
			(EquippedWeapon->GetCombatState() != ECombatState::ECS_Reloading) &&
			(EquippedWeapon->GetCombatState() != ECombatState::ECS_Equipping) &&
			(!bChangingWeapon) )
		{
			/*AWeapon* traceHitWeapon = Cast<AWeapon>(TraceHitItemLastFrame);
			SwapWeapon(traceHitWeapon);*/
			//GetPickupItem(TraceHitItemLastFrame);
			TraceHitItemLastFrame->StartItemCurve(this);
			TraceHitItemLastFrame = nullptr;

		}
	}
}

void AShooterCharacter::DropItemInputHandler(const FInputActionValue& value)
{
	if (value.Get<float>() > 0)
	{
		DropWeapon();
	}
}

void AShooterCharacter::InventorySelectInputHandler(const FInputActionValue& value)
{
	int32 newWeaponIndex = -1;

	if (value.Get<FVector>().X < 0)
	{
		newWeaponIndex = 0;
	}
	else if (value.Get<FVector>().X == 1)
	{
		newWeaponIndex = 1;
	}
	else if (value.Get<FVector>().X == 2)
	{
		newWeaponIndex = 2;
	}
	else if (value.Get<FVector>().X == 3)
	{
		newWeaponIndex = 3;
	}
	else if (value.Get<FVector>().X == 4)
	{
		newWeaponIndex = 4;
	}
	else if (value.Get<FVector>().X == 5)
	{
		newWeaponIndex = 5;
	}
	ChangeEquippedWeapon(EquippedWeapon->GetWeaponSlotIndex(), newWeaponIndex);
}

void AShooterCharacter::PlayFireSound()
{
	// Shoting Sound 
	if (EquippedWeapon->GetFireSound() != nullptr)
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->GetFireSound());
	}
}

void AShooterCharacter::SendBullet()
{
	// Get Muzzle Socket
	const USkeletalMeshSocket* MuzzleSocket = EquippedWeapon->GetItemMesh()->GetSocketByName(EquippedWeapon->GetMuzzleSocketName());

	if (MuzzleSocket != nullptr)
	{
		// Get Socket Transform
		const FTransform socketTransform = MuzzleSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());

		// Spawn Muzzle Flash
		if (EquippedWeapon->GetMuzzleFlash() != nullptr)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquippedWeapon->GetMuzzleFlash(), socketTransform);
		}


		FHitResult OutHitResult;
		bool  bSetBeamEnd = GetBeamEndLocation(socketTransform.GetLocation(), OutHitResult);

		if (bSetBeamEnd) // was the setting BeamEndPoint successful?
		{
			if (OutHitResult.GetActor())
			{
				IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(OutHitResult.GetActor());
				if (BulletHitInterface)
				{
					BulletHitInterface->BulletHit_Implementation(OutHitResult);
					AEnemy* HitEnemy = Cast<AEnemy>(OutHitResult.GetActor());
					if (HitEnemy)
					{
						if (OutHitResult.BoneName.ToString() == HitEnemy->GetHeadBoneName())
						{
							UGameplayStatics::ApplyDamage(OutHitResult.GetActor(), EquippedWeapon->GetHeadshotDamage(), GetController(),
								this, UDamageType::StaticClass());

							// Show Hit Amount Widget
							HitEnemy->ShowHitAmount(EquippedWeapon->GetHeadshotDamage(), OutHitResult.Location, true);
						}
						else
						{
							UGameplayStatics::ApplyDamage(OutHitResult.GetActor(), EquippedWeapon->GetDamage(), GetController(),
								this, UDamageType::StaticClass());

							// Show Hit Amount Widget
							HitEnemy->ShowHitAmount(EquippedWeapon->GetDamage(), OutHitResult.Location, false);
						}
						
					}
				}
				else
				{
					// Spawn default particles
					if (ImpactParticles)
					{
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, OutHitResult.Location);
					}

					if (BeamParticles)
					{
						UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, socketTransform);
						if (Beam != nullptr)
						{
							Beam->SetVectorParameter(FName("Target"), OutHitResult.Location);
						}

					}
				}
			}
			
			
		}
	}
}

void AShooterCharacter::PlayHipFireMontage()
{
	// HipFireMontage which is shoting animation
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance != nullptr && HipFireMontage != nullptr)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

void AShooterCharacter::ReloadInputHandler(const FInputActionValue& value)
{
	if (value.Get<float>() > 0)
	{
		if ( (EquippedWeapon != nullptr) &&
			(EquippedWeapon->GetCombatState() != ECombatState::ECS_Reloading) &&
			(EquippedWeapon->GetCombatState() != ECombatState::ECS_Equipping) &&
			(!bChangingWeapon) )
		{
			bool bReloadSuccess = EquippedWeapon->ReloadWeapon(AmmoMap);
			if (bReloadSuccess)
			{
				if (bAiming)
				{
					SetAiming(false);
				}

				EquippedWeapon->SetCombatState(ECombatState::ECS_Reloading);

				FName MontageSection = TEXT("Reload");
				SetReloadMontageSection(MontageSection);

				// Play reload montage
				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				if (AnimInstance != nullptr && ReloadMontage != nullptr)
				{
					AnimInstance->Montage_Play(ReloadMontage);
					AnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSection());
				}
				GetWorldTimerManager().SetTimer(ReloadingTimerHandle, this, &AShooterCharacter::ReloadFinished, 2.20f, false);
			}
			else
			{
				SetCanShoot();
				//TODO play out of ammo sound
			}
			
		}

	}
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
	if ( (EquippedWeapon != nullptr) &&
		 (EquippedWeapon->GetCombatState() != ECombatState::ECS_Reloading) &&
		 (EquippedWeapon->GetCombatState() != ECombatState::ECS_Equipping) &&
		 (!bChangingWeapon) )
	{
		if (EquippedWeapon->GetAmmo() > 0)
		{
			EquippedWeapon->SetCombatState(ECombatState::ECS_CanShoot);
		}
		else
		{
			EquippedWeapon->SetCombatState(ECombatState::ECS_OutOfAmmo);
		}

		if (EquippedWeapon->GetIsAuto() == true)
		{
			bIsJustShooted = false;
		}
	}
}
void AShooterCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight;
	if (bCrouching)
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}
	else
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}

	const float InterpHalfHeight = FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), TargetCapsuleHalfHeight, DeltaTime, 20.f);
	
	// Negative value if crouching; Positive vaue if standing
	const float DeltaCapsuleHalfHeight = InterpHalfHeight - CurrentCapsuleHalfHeight;

	const FVector MeshOffset = FVector(0.f, 0.f, -DeltaCapsuleHalfHeight);
	GetMesh()->AddLocalOffset(MeshOffset);

	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
	CurrentCapsuleHalfHeight = InterpHalfHeight;
}

void AShooterCharacter::ReloadFinished()
{
	if (EquippedWeapon != nullptr)
	{
		if (EquippedWeapon->GetAmmo() > 0)
		{
			EquippedWeapon->SetCombatState(ECombatState::ECS_CanShoot);
		}
		else
		{
			EquippedWeapon->SetCombatState(ECombatState::ECS_OutOfAmmo);
		}
	}

	SetAiming(bAimingButtonPressed);

}

void AShooterCharacter::GrabMag()
{
	EquippedWeapon->SetMagMoving(true);
	
	// Index for the clip bone on the equipped weapon
	int32 MagBoneIndex = EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetMagazineBoneName());
	// Store the transform of the clip
	MagazineTransform = EquippedWeapon->GetItemMesh()->GetBoneTransform(MagBoneIndex);

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("Hand_L")));
	HandSceneComponent->SetWorldTransform(MagazineTransform);

}

void AShooterCharacter::ReplaceMag()
{
	EquippedWeapon->SetMagMoving(false);
}


float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

AWeapon* AShooterCharacter::GetEquippedWeapon()
{
	return EquippedWeapon;
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


void AShooterCharacter::SetReloadMontageSection(FName& MontageSection)
{
	switch (EquippedWeapon->GetWeaponType())
	{
		case EWeaponType::EWT_Pistol:
			MontageSection = TEXT("ReloadPistol");
			break;
		case EWeaponType::EWT_SMG:
			MontageSection = TEXT("ReloadSMG");
			break;
		case EWeaponType::EWT_BurstRifle:
			MontageSection = TEXT("ReloadRifle");
			break;
		case EWeaponType::EWT_AutoRifle:
			MontageSection = TEXT("ReloadRifle");
			break;
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
		// TODO Delete
		//DrawDebugLine(GetWorld(), Start, End, FColor::Red, true, 5.f);
		//UE_LOG(LogTemp, Warning, TEXT("HIT NAME: %s"), *OutHitResult.GetActor()->GetName());
		if (OutHitResult.bBlockingHit)
		{
			return true;
		}
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
			if ((HitItem != nullptr) && (HitItem->GetItemState() != EItemState::EIS_PickupInterping))
			{

				if (HitItem->GetPickupWidget() != nullptr)
				{
					//Show Item's pickup widget
					HitItem->GetPickupWidget()->SetVisibility(true);
					HitItem->ToggleCustomDepth(true);
				}

				// Highlight Empty Inventory Slot if there is an empty slot
				const AWeapon* TraceHitWeapon = Cast<AWeapon>(HitItem);
				if (TraceHitWeapon != nullptr)
				{
					if (HighlightedSlot == -1)
					{
						// Start Highlight the Slot
						HighlightInventorySlot(true);
					}
				}
				else
				{
					if (HighlightedSlot != -1)
					{
						// Stop Highlight the Slot
						HighlightInventorySlot(false);
					}
				}

				// check it hit an AItem last frame
				if (TraceHitItemLastFrame != nullptr)
				{
					if (HitItem != TraceHitItemLastFrame)
					{
						// It is hitting a different AItem this frame from last frame or HitItem is null
						TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
						TraceHitItemLastFrame->ToggleCustomDepth(false);
					}
				}

				// Store a reference to HitItem for next frame
				TraceHitItemLastFrame = HitItem;
			}
			else
			{
				HighlightInventorySlot(false);
				if (TraceHitItemLastFrame != nullptr)
				{
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
					TraceHitItemLastFrame->ToggleCustomDepth(false);
					TraceHitItemLastFrame = nullptr;
				}
			}
			
		}
		else if (TraceHitItemLastFrame != nullptr)
		{
			TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
			TraceHitItemLastFrame->ToggleCustomDepth(false);
			TraceHitItemLastFrame = nullptr;
			HighlightInventorySlot(false);

		}
	}
	else if (TraceHitItemLastFrame != nullptr)
	{
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
		TraceHitItemLastFrame->ToggleCustomDepth(false);
		TraceHitItemLastFrame = nullptr;
		HighlightInventorySlot(false);

	}
	
}

AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
	// Check tsubclassof
	if (WeaponClass)
	{
		
		// Spawn and return it
		return GetWorld()->SpawnActor<AWeapon>(WeaponClass);
	}
	return nullptr;
}

void AShooterCharacter::GetPickupItem(AItem* item)
{
	if (item->GetItemType() == EItemType::EIT_Weapon)
	{
		AWeapon* weapon = Cast<AWeapon>(item);
		if (weapon)
		{
			if (!bInventoryFull)
			{
				weapon->SetWeaponSlotIndex(Inventory.Num());
				Inventory.Add(weapon);
				weapon->SetItemState(EItemState::EIS_PickedUp);
				if (Inventory.Num() == INVENTORY_CAPACITY)
				{
					bInventoryFull = true;
				}
			}
			else // Inventory is full! Swap with EquippedWeapon
			{
				SwapWeapon(weapon);
			}
		}
	}
	if (item->GetItemType() == EItemType::EIT_Ammo)
	{
		AAmmo* ammo = Cast<AAmmo>(item);
		if (ammo)
		{
			TakeAmmo(ammo);
		}
	}
	
}

void AShooterCharacter::ChangeEquippedWeapon(int32 CurrentWeaponIndex, int32 NewWeaponIndex)
{
	if ( (CurrentWeaponIndex == NewWeaponIndex) || (NewWeaponIndex >= Inventory.Num() ) ||
		 (NewWeaponIndex < 0) ||
		 (EquippedWeapon->GetCombatState() == ECombatState::ECS_Reloading) ) {
		return;
	}
	if (Inventory.IsValidIndex(NewWeaponIndex))
	{
		if (bAiming)
		{
			SetAiming(false);
		}
		AWeapon* NewWeapon = Cast<AWeapon>(Inventory[NewWeaponIndex]);
		EquippedWeapon->SetItemState(EItemState::EIS_PickedUp);
		EquipWeapon(NewWeapon);
		EquippedWeapon->SetCombatState(ECombatState::ECS_Equipping);
		bChangingWeapon = true;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && EquipMontage)
		{
			AnimInstance->Montage_Play(EquipMontage, 1.0);
			AnimInstance->Montage_JumpToSection(FName("Equip"));
		}
	}


}

void AShooterCharacter::FinishChangingWeapon()
{
	bChangingWeapon = false;
	EquippedWeapon->SetCombatState(ECombatState::ECS_CanShoot);
	if (bAimingButtonPressed == true)
	{
		SetAiming(true);
	}
}

void AShooterCharacter::SetAiming(bool AimingButtonPressed)
{
	if (AimingButtonPressed)
	{
		if ( (EquippedWeapon != nullptr) &&
			(EquippedWeapon->GetCombatState() != ECombatState::ECS_Reloading) &&
			(EquippedWeapon->GetCombatState() != ECombatState::ECS_Equipping) &&
			(!bChangingWeapon) )
		{
			bAiming = true;
			GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
		}
		
	}
	else
	{
		bAiming = false;
		if (!bCrouching)
		{
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		}
	}
}


void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		const USkeletalMeshSocket* WeaponSocket = GetMesh()->GetSocketByName(FName("weapon_r"));
		if (WeaponSocket)
		{
			//WeaponToEquip->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("weapon_r"));
			WeaponSocket->AttachActor(WeaponToEquip, GetMesh());
			WeaponToEquip->SetOwner(this);
			if (EquippedWeapon == nullptr)
			{
				// No EquippedWeapon yet. No need to reverse the icon animation
				EquipItemDelegate.Broadcast(-1, WeaponToEquip->GetWeaponSlotIndex());
			}
			else
			{
				EquipItemDelegate.Broadcast(EquippedWeapon->GetWeaponSlotIndex(), WeaponToEquip->GetWeaponSlotIndex());
			}


			EquippedWeapon = WeaponToEquip;
			EquippedWeapon->SetItemState(EItemState::EIS_Equipped);

			


		}
	}
}

void AShooterCharacter::DropWeapon()
{
	if ( (EquippedWeapon != nullptr) &&
		(EquippedWeapon->GetCombatState() != ECombatState::ECS_Reloading) &&
		(EquippedWeapon->GetCombatState() != ECombatState::ECS_Equipping) &&
		(!bChangingWeapon) )
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();

		//EquippedWeapon = nullptr;

	}
}

void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	if (Inventory.IsValidIndex(EquippedWeapon->GetWeaponSlotIndex()))
	{
		Inventory[EquippedWeapon->GetWeaponSlotIndex()] = WeaponToSwap;
		WeaponToSwap->SetWeaponSlotIndex(EquippedWeapon->GetWeaponSlotIndex());
	}
	DropWeapon();
	EquipWeapon(WeaponToSwap);
}

/* No longer needed. AItem has GetInterpLocation
FVector AShooterCharacter::GetCameraInterpLocation()
{
	const FVector CameraWorldLocation = FollowCam->GetComponentLocation();
	const FVector CameraForwardDistance = FollowCam->GetForwardVector() * ItemInterpOutwardDistance;
	const FVector CameraUpDistance = FollowCam->GetUpVector() * ItemInterpUpDistance;
	// const FVector CameraUpDistance = FVector(0.f, 0.f, ItemInterpUpDistance);
	return CameraWorldLocation + CameraForwardDistance + CameraUpDistance;
}
*/
void AShooterCharacter::TakeAmmo(class AAmmo* Ammo)
{
	//check to see if AmmoMap contains Ammo's ammo type
	if (AmmoMap.Find(Ammo->GetAmmoType()))
	{
		AmmoMap[Ammo->GetAmmoType()] += Ammo->GetItemAmount();
	}
	else
	{
		AmmoMap.Add(Ammo->GetAmmoType(), Ammo->GetItemAmount());
	}

	//TODO can be add automatic reload if weapon is empty.

	Ammo->Destroy();
}
void AShooterCharacter::InitializeInterpLocations()
{
	FInterpLocation WeaponLocation = FInterpLocation(WeaponInterpComp, 0);
	InterpLocations.Add(WeaponLocation);

	FInterpLocation interpLocation = FInterpLocation(FirstInterpComp, 0);
	InterpLocations.Add(interpLocation);

	interpLocation = FInterpLocation(SecondInterpComp, 0);
	InterpLocations.Add(interpLocation);

	interpLocation = FInterpLocation(ThirdInterpComp, 0);
	InterpLocations.Add(interpLocation);

	interpLocation = FInterpLocation(FourthInterpComp, 0);
	InterpLocations.Add(interpLocation);

	interpLocation = FInterpLocation(FifthInterpComp, 0);
	InterpLocations.Add(interpLocation);

	interpLocation = FInterpLocation(SixthInterpComp, 0);
	InterpLocations.Add(interpLocation);

}


int32 AShooterCharacter::GetInterpLocationIndex()
{
	int32 lowestIndex = 1;

	int32 lowestCount = INT_MAX;

	for (int32 i = 1; i < InterpLocations.Num(); i++)
	{
		if (InterpLocations[i].ItemCount < lowestCount)
		{
			lowestIndex = i;
			lowestCount = InterpLocations[i].ItemCount;
		}
	}
	return lowestIndex;
}

FInterpLocation AShooterCharacter::GetInterpLocation(int32 index)
{
	if (index < InterpLocations.Num())
	{
		return InterpLocations[index];
	}
	return FInterpLocation();
}

void AShooterCharacter::HandleInterpLocItemCount(int32 index, int32 itemAmount)
{
	if (itemAmount < -1 || itemAmount > 1) return;
	if (InterpLocations.Num() > index)
	{
		InterpLocations[index].ItemCount += itemAmount;
	}
}

void AShooterCharacter::StartSoundTimer(EItemState itemState)
{
	if (itemState == EItemState::EIS_PickedUp)
	{ 
		bShouldPlayPickupSound = false;
		GetWorldTimerManager().SetTimer(PickupSoundTimer, this,  &AShooterCharacter::ResetPickupSoundTimer, SoundResetTime, false);
	}
	else if (itemState == EItemState::EIS_Equipped)
	{
		bShouldPlayEquipSound = false;
		GetWorldTimerManager().SetTimer(EquipSoundTimer, this, &AShooterCharacter::ResetEquipSoundTimer, SoundResetTime, false);

	}
}
void AShooterCharacter::ResetPickupSoundTimer()
{
		bShouldPlayPickupSound = true;
}
void AShooterCharacter::ResetEquipSoundTimer()
{
		bShouldPlayEquipSound = true;
}

int AShooterCharacter::GetEmptyInventorySlot()
{
	if (bInventoryFull)
	{
		return -1;
	}
	else
	{
		for (int i = 0; i < Inventory.Num(); i++)
		{
			if (Inventory[i] == nullptr)
			{
				return i;
			}
		}
		return Inventory.Num();
	}
}
void AShooterCharacter::HighlightInventorySlot(bool IsHighlighting)
{
	if (IsHighlighting)
	{
		HighlightedSlot = GetEmptyInventorySlot();

		HighlightIconDelegate.Broadcast(HighlightedSlot, true);
	}
	else
	{
		HighlightIconDelegate.Broadcast(HighlightedSlot, false);
		HighlightedSlot = -1;
	}


}
float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (CurrentHealth - DamageAmount <= 0)
	{
		CurrentHealth = 0;
		PlayerDiedDelegate.Broadcast();
		Die();
		UE_LOG(LogTemp, Warning, TEXT("DIEEEEEE"));
	}
	else
	{
		CurrentHealth -= DamageAmount;
	}
	return DamageAmount;
}

EPhysicalSurface AShooterCharacter::GetFootstepSurface()
{
	FHitResult HitResult;
	const FVector Start = GetActorLocation();
	const FVector End = GetActorLocation() + FVector(0.f, 0.f, -400.f);
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;

	bool isHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);
	return HitResult.PhysMaterial->SurfaceType;
	//return UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());

}

FGenericTeamId AShooterCharacter::GetGenericTeamId() const
{
	return TeamId;
}
FName AShooterCharacter::DecideHitReactMontageSection(FVector EnemyLocation)
{
	FVector playerToEnemyVec = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), EnemyLocation);
	float dotProductResult = FVector::DotProduct(GetActorForwardVector(), playerToEnemyVec);

	if (dotProductResult >= 50.f)
	{
		return "HitReactFront";
	}
	else if (dotProductResult < 50.f && dotProductResult > -50.f)
	{
		dotProductResult = FVector::DotProduct(GetActorRightVector(), playerToEnemyVec);
		if (dotProductResult > 0)
		{
			return "HitReactRight";
		}
		else
		{
			return "HitReactLeft";
		}
	}
	else
	{
		return "HitReactBack";
	}

}

void AShooterCharacter::HitReact(AEnemy* enemy, FVector HitLocation)
{
	if (CurrentHealth > 0)
	{
		if (MeleeImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, MeleeImpactSound, GetActorLocation(), 0.2f);
		}
		if (HitReactMontage)
		{
			FName montageSection = DecideHitReactMontageSection(HitLocation);

			UAnimInstance* animInst = GetMesh()->GetAnimInstance();

			animInst->Montage_Play(HitReactMontage);
			animInst->Montage_JumpToSection(montageSection, HitReactMontage);
		}
	}
	
}

void AShooterCharacter::Die()
{
	UAnimInstance* animInst = GetMesh()->GetAnimInstance();
	if (animInst)
	{
		animInst->Montage_Play(DieMontage);
		animInst->Montage_JumpToSection(FName("Default"), DieMontage);
	}
}

void AShooterCharacter::FinishDie()
{
	GetMesh()->bPauseAnims = true;
	DisableInput(UGameplayStatics::GetPlayerController(this, 0));
}
