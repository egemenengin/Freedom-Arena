// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BulletHitInterface.h"
#include "Enemy.generated.h"

UCLASS()
class FREEDOMARENA_API AEnemy : public ACharacter, public IBulletHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar();
	void ShowHealthBar_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();
	
	void Die(FVector PlayerLocation);

	void PlayHitMontage(FName Section, float PlayRate = 1.f);

	void ResetHitReact();

	FName FindHitDirection(FVector PlayerLocation);
	FName FindDieDirection(FVector PlayerLocation);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BulletHit_Implementation(FHitResult HitResult) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitAmount(float HitAmount, FVector HitLocation, bool isHeadShot);

	UFUNCTION(BlueprintCallable)
	void AddHitWidget(UUserWidget* newHitWidget, FVector Location);

	UFUNCTION()
	void DestroyHitWidget(UUserWidget* HitWidget);

	void UpdateHitWidgetLocation();
	

	FORCEINLINE FString GetHeadBoneName() const { return HeadBoneName; }
protected:
	// Particles to spawn when hit by bullets
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowedPrivateAccess = "true"))
	class UParticleSystem* ImpactParticles;

	// Sound to play when hit by bullets
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowedPrivateAccess = "true"))
	class USoundCue* ImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health, meta = (AllowPrivateAccess = "true"))
	FString HeadBoneName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health, meta = (AllowPrivateAccess = "true"))
	FTimerHandle HealthBarTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health, meta = (AllowPrivateAccess = "true"))
	float HealthBarTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitMontage;

	FTimerHandle HitAnimDelayTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health, meta = (AllowPrivateAccess = "true"))
	float HitAnimDelayMin;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health, meta = (AllowPrivateAccess = "true"))
	float HitAnimDelayMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health, meta = (AllowPrivateAccess = "true"))
	bool bCanHitReact;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TMap<UUserWidget*, FVector> HitWidgets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HitWidgetTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bIsDead;

};
