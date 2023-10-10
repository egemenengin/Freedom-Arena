// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEnemy::AEnemy() :
	CurrentHealth(100.f),
	MaxHealth(100.f),
	HealthBarTime(4.f),
	HitAnimDelayMin(0.5f),
	HitAnimDelayMax(1.5f),
	bCanHitReact(true),
	HitWidgetTime(2.f),
	bIsDead(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateHitWidgetLocation();
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::BulletHit_Implementation(FHitResult HitResult)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, HitResult.Location, FRotator(0.f));
	}
	ShowHealthBar();
	
	// TODO Decide that enemy take damage from which side or is it died
	PlayHitMontage(FindHitDirection(HitResult.TraceStart));
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (CurrentHealth - DamageAmount <= 0)
	{
		CurrentHealth = 0;
		Die(DamageCauser->GetActorLocation());
	}
	else
	{
		CurrentHealth -= DamageAmount;
	}
	// TODO Delete
	UE_LOG(LogTemp, Warning, TEXT("DAMAGE: %f | Current Health: %f"), DamageAmount, CurrentHealth);

	return DamageAmount;
}



void AEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &AEnemy::HideHealthBar, HealthBarTime, false);
}

void AEnemy::Die(FVector PlayerLocation)
{
	HideHealthBar();
	bIsDead = true;
	PlayHitMontage(FindDieDirection(PlayerLocation));
	//Destroy();
}

void AEnemy::PlayHitMontage(FName Section, float PlayRate)
{
	if (bCanHitReact || bIsDead)
	{
		UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
		if (AnimInst)
		{
			AnimInst->Montage_Play(HitMontage, PlayRate);
			AnimInst->Montage_JumpToSection(Section, HitMontage);

			bCanHitReact = false;
			const float HitDelay = FMath::RandRange(HitAnimDelayMin, HitAnimDelayMax);

			GetWorldTimerManager().SetTimer(HitAnimDelayTimer, this, &AEnemy::ResetHitReact, HitDelay, false);

		}
	}
	
	
}

void AEnemy::ResetHitReact()
{
	bCanHitReact = true;
}
void AEnemy::AddHitWidget(UUserWidget* newHitWidget, FVector Location)
{
	HitWidgets.Add(newHitWidget, Location);

	FTimerHandle HitWidgetTimer;
	FTimerDelegate HitWidgetDelegate;
	HitWidgetDelegate.BindUFunction(this, FName("DestroyHitWidget"), newHitWidget);
	GetWorldTimerManager().SetTimer(HitWidgetTimer, HitWidgetDelegate, HitWidgetTime, false);
}

void AEnemy::DestroyHitWidget(UUserWidget* HitWidget)
{
	HitWidgets.Remove(HitWidget);
	HitWidget->RemoveFromParent();
}

void AEnemy::UpdateHitWidgetLocation()
{
	for (auto hitPair : HitWidgets)
	{
		UUserWidget* hitWidget = hitPair.Key;
		FVector hitLocation = hitPair.Value;
		FVector2D ScreenPosition;
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), hitLocation, ScreenPosition);

		hitWidget->SetPositionInViewport(ScreenPosition);
	}
}

FName AEnemy::FindHitDirection(FVector PlayerLocation)
{	
	// Vector from enemy to playerlocation
	FVector enemyToPlayer = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), PlayerLocation);
	
	float dotProductResult = FVector::DotProduct(GetActorForwardVector(), enemyToPlayer);
	UE_LOG(LogTemp, Warning, TEXT("%f"), dotProductResult);

	if (dotProductResult >= .50f)
	{
		UE_LOG(LogTemp, Warning, TEXT("HitReactFront"));

		return FName("HitReactFront");
	}
	else if (dotProductResult < .50f && dotProductResult > -.50f)
	{
		dotProductResult = FVector::DotProduct(GetActorRightVector(), enemyToPlayer);

		if (dotProductResult > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("HitReactRight"));
			return FName("HitReactRight");
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("HitReactLeft"));
			return FName("HitReactLeft");
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HitReactBack"));

		return FName("HitReactBack");
	}
	
}
FName AEnemy::FindDieDirection(FVector PlayerLocation)
{
	FVector enemyToPlayer = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), PlayerLocation);

	float dotProductResult = FVector::DotProduct(GetActorForwardVector(), enemyToPlayer);
	if (dotProductResult >= 0.f)
	{
		return FName("DeathFront");
	}
	else
	{
		return FName("DeathBack");
	}
}