// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class FREEDOMARENA_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;
	
	// Adds an impulse to the weapon
	void ThrowWeapon();
protected:

	void StopFalling();
private:
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;
	float ThrowPower;
};
