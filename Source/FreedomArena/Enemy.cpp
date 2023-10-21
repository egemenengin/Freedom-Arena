// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/DamageType.h"
#include "ShooterCharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

// Sets default values
AEnemy::AEnemy() :
	CurrentHealth(100.f),
	MaxHealth(100.f),
	HealthBarTime(4.f),
	HitAnimDelayMin(0.5f),
	HitAnimDelayMax(1.5f),
	bCanHitReact(true),
	HitWidgetTime(2.f),
	bIsDead(false),
	bIsStunned(false),
	StunChance(0.5f),
	Damage(20.f),
	AttackDelayTime(3.f),
	DestroyDelayTime(2.f),
	bIsGameOver(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Agro Sphere"));
	CombatSphere->SetupAttachment(GetRootComponent());

	LeftWeaponCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Left Weapon Box"));
	LeftWeaponCollider->SetupAttachment(GetMesh(), FName("LeftWeaponSocket"));

	RightWeaponCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Right Weaon Box"));
	RightWeaponCollider->SetupAttachment(GetMesh(), FName("RightWeaponSocket"));

	LeftWeaponCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponCollider->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftWeaponCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftWeaponCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightWeaponCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollider->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	//PawnSenseComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSense"));

	/*AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	
	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight"));
	Sight->SightRadius = 400.f;
	Sight->LoseSightRadius = Sight->SightRadius + 100.f;
	Sight->PeripheralVisionAngleDegrees = 45.f;
	Sight->DetectionByAffiliation.bDetectEnemies = true;
	Sight->DetectionByAffiliation.bDetectNeutrals = true;
	Sight->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerceptionComp->ConfigureSense(*Sight);
	AIPerceptionComp->SetDominantSense(Sight->GetSenseImplementation());*/

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	EnemyAIController = Cast<AEnemyAIController>(GetController());

	if (EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), true);
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsGameOver"), false);
	}

	AShooterCharacter* character = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	character->PlayerDiedDelegate.AddDynamic(this, &AEnemy::GameOver);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnCombatSphereOverlap);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnCombatSphereEndOverlap);

	LeftWeaponCollider->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnLeftWeaponOverlap);
	RightWeaponCollider->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnRightWeaponOverlap);

	//PawnSenseComp->OnSeePawn.AddDynamic(this, &AEnemy::OnSeeTarget);
	
	//AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemy::OnPerception);

	SetBlackboardPatrolPoints();

}


// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateHitWidgetLocation();
}

void AEnemy::SetBlackboardPatrolPoints()
{
	FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), FirstPatrolPoint);
	DrawDebugSphere(GetWorld(), WorldPatrolPoint, 25, 12, FColor::Red, true);

	FVector SecondWorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), SecondPatrolPoint);
	DrawDebugSphere(GetWorld(), SecondWorldPatrolPoint, 25, 12, FColor::Blue, true);
	// Find world location without kismet math library
	// WorldPatrolPoint = FVector(GetActorLocation().X + PatrolPoint.X, GetActorLocation().Y + PatrolPoint.Y, GetActorLocation().Z + PatrolPoint.Z);
	// DrawDebugSphere(GetWorld(), WorldPatrolPoint, 25, 12, FColor::Blue, true);

	if (EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsVector(TEXT("FirstPatrolPoint"), WorldPatrolPoint);
		EnemyAIController->GetBlackboardComponent()->SetValueAsVector(TEXT("SecondPatrolPoint"), SecondWorldPatrolPoint);

		EnemyAIController->RunBehaviorTree(EnemyBehaviorTree);

	}
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
	if (!bIsDying)
	{
		ShowHealthBar();

		float StunValue = FMath::FRandRange(0.f, 1.f);
		if (StunValue <= StunChance)
		{
			PlayHitMontage(FindHitDirection(HitResult.TraceStart));
			SetStunned(true);
		}
	}
	
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (CurrentHealth - DamageAmount <= 0)
	{
		CurrentHealth = 0;
		if (!bIsDying)
		{
			Die(DamageCauser->GetActorLocation());
		}
	}
	else
	{
		if (EnemyAIController)
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), DamageCauser);
		}
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
	bIsDying = true;
	bCanHitReact = true;
	if (EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsDead"), true);
		EnemyAIController->StopMovement();
	}
	PlayDeadMontage(FindDieDirection(PlayerLocation));
}

void AEnemy::PlayHitMontage(FName Section, float PlayRate)
{
	if (bCanHitReact)
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

void AEnemy::PlayDeadMontage(FName Section, float PlayRate)
{
	if (bIsDying)
	{
		UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
		if (AnimInst)
		{
			AnimInst->Montage_Play(HitMontage, PlayRate);
			AnimInst->Montage_JumpToSection(Section, HitMontage);
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

	if (dotProductResult >= .50f)
	{
		return FName("HitReactFront");
	}
	else if (dotProductResult < .50f && dotProductResult > -.50f)
	{
		dotProductResult = FVector::DotProduct(GetActorRightVector(), enemyToPlayer);

		if (dotProductResult > 0)
		{
			return FName("HitReactRight");
		}
		else
		{
			return FName("HitReactLeft");
		}
	}
	else
	{
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

void AEnemy::OnCombatSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AShooterCharacter* target = Cast<AShooterCharacter>(OtherActor);
	if (target)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), true);
	}
}

void AEnemy::OnCombatSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponet, int32 OtherBodyIndex)
{
	AShooterCharacter* target = Cast<AShooterCharacter>(OtherActor);
	if (target)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), false);
	}
}

void AEnemy::OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;
	AShooterCharacter* player = Cast<AShooterCharacter>(OtherActor);
	if (player)
	{
		UGameplayStatics::ApplyDamage(player, Damage, EnemyAIController, this, UDamageType::StaticClass());
		const FVector hitLocation = GetMesh()->GetSocketLocation(FName("FX_Trail_L_01"));
		player->HitReact(this, hitLocation);

	}
}

void AEnemy::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;
	AShooterCharacter* player = Cast<AShooterCharacter>(OtherActor);
	if (player)
	{
		UGameplayStatics::ApplyDamage(player, Damage, EnemyAIController, this, UDamageType::StaticClass());
		const FVector hitLocation = GetMesh()->GetSocketLocation(FName("FX_Trail_R_01"));
		player->HitReact(this, hitLocation);
	}
}

void AEnemy::ActivateWeapon(bool isLeft)
{
	if (isLeft)
	{
		LeftWeaponCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		RightWeaponCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void AEnemy::DeactivateWeapon(bool isLeft)
{
	if (isLeft)
	{
		LeftWeaponCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		RightWeaponCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AEnemy::OnSeeTarget(APawn* OtherPawn)
{
	if (OtherPawn == nullptr) return;
	AShooterCharacter* target = Cast<AShooterCharacter>(OtherPawn);
	if (target)
	{
		// TODO Linetrace to check if there is any obstacle between enemy and target
		EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), target);
	}
	else
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), nullptr);
	}
}

void AEnemy::OnPerception(AActor* Actor, FAIStimulus Stimulus)
{
	if (Actor == nullptr)
	{
		return;
	}

	AShooterCharacter* target = Cast<AShooterCharacter>(Actor);
	if (Stimulus.WasSuccessfullySensed())
	{
		if (target)
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), target);
		}

	}
	else
	{
		EnemyAIController->GetBlackboardComponent()->ClearValue(TEXT("Target"));
	}


}

void AEnemy::SetStunned(bool IsStunned)
{
	bIsStunned = IsStunned;
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsStunned"), IsStunned);
}

void AEnemy::PlayAttackMontage(FName Section, float PlayRate)
{
	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst)
	{
		AnimInst->Montage_Play(AttackMontage, PlayRate);
		AnimInst->Montage_JumpToSection(Section, AttackMontage);
		bCanAttack = false;
		if (EnemyAIController)
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), false);
		}

		GetWorldTimerManager().SetTimer(AttackDelayTimer, this, &AEnemy::ResetAttack, AttackDelayTime, false);
	}

}
void AEnemy::ResetAttack()
{
	bCanAttack = true;
	if (EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), true);
	}
}

void AEnemy::FinishDying()
{
	GetMesh()->bPauseAnims = true;

	GetWorldTimerManager().SetTimer(DestroyDelayTimer, this, &AEnemy::DestroyEnemy, DestroyDelayTime, false);
}

void AEnemy::DestroyEnemy()
{
	Destroy();
}

void AEnemy::GameOver()
{
	bIsGameOver = true;
	if (EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsGameOver"), true);
	}
}


