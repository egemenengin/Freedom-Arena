// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "WeaponEnums.h"

#include "Weapon.generated.h"


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

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE void SetAmmo(int32 ammo) { Ammo = ammo; }

	FORCEINLINE ECombatState GetCombatState() const { return WeaponCombatState; }
	FORCEINLINE void SetCombatState(ECombatState state) { WeaponCombatState = state; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EAmmoType GetWeaponAmmoType() const { return WeaponAmmoType; }

	FORCEINLINE FName GetMagazineBoneName() const { return MagazineBoneName; }
	FORCEINLINE void SetMagMoving(bool isMoving) { bMagMoving = isMoving; }

	FORCEINLINE int32 GetWeaponSlotIndex() const { return WeaponSlotIndex; }
	FORCEINLINE void SetWeaponSlotIndex(int32 Index) { WeaponSlotIndex = Index; }

	// Check the make sure the weapon has ammo or not
	UFUNCTION()
	bool WeaponHasAmmo();

	UFUNCTION()
	void DecrementAmmo();

	UFUNCTION()
	bool ReloadWeapon(TMap<EAmmoType, int32>& AmmoMap);
protected:

	void StopFalling();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponProperties", meta = (AllowPrivateAccess = "true"))
	ECombatState WeaponCombatState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponProperties", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponProperties", meta = (AllowPrivateAccess = "true"))
	EAmmoType WeaponAmmoType;

	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;
	float ThrowPower;

	// Max Mag Size
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 MagazineMaxCapacity;

	// Amount of ammo in magazine
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 Ammo;

	// Name for the clip bone
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FName MagazineBoneName;

	// True when magazine is moving while reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	bool bMagMoving;

	// Slot Index in the InventoryArray of Character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	int32 WeaponSlotIndex;
};
