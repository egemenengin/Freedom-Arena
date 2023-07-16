// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "WeaponEnums.h"

#include "Weapon.generated.h"

USTRUCT(BlueprintType)
struct FWeaponDataTableRowBase : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MagCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Ammo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WeaponName;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* InventoryIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* AmmoIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaterialIndex;
};
UCLASS()
class FREEDOMARENA_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;
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

	void SetWeaponTypeData(struct FWeaponDataTableRowBase& WeaponDataRow);
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponProperties", meta = (AllowPrivateAccess = "true"))
	ECombatState WeaponCombatState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponProperties", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponProperties", meta = (AllowPrivateAccess = "true"))
	EAmmoType WeaponAmmoType;

	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;
	float ThrowPower;
	// Damage
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int Damage;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	class UDataTable* WeaponTypeDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	UTexture2D* InventoryIcon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoIcon;
};
