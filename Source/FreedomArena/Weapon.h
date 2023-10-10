// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "WeaponEnums.h"
#include "Animation/AnimInstance.h"
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
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeadShotDamage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutomatic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AutoFireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxRecoilRotation;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MagBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ReloadMontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneNameToHide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MuzzleSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* RecoilCurve;

};
UCLASS()
class FREEDOMARENA_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	// Adds an impulse to the weapon
	void ThrowWeapon();

	void StartSlideTimer();
	void StartRecoil();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE void SetAmmo(int32 ammo) { Ammo = ammo; }
	FORCEINLINE bool GetIsAuto() const { return bAutomatic; }
	FORCEINLINE float GetFireRate() const { return AutoFireRate; }
	
	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE float GetHeadshotDamage() const { return HeadshotDamage; }

	FORCEINLINE ECombatState GetCombatState() const { return WeaponCombatState; }
	FORCEINLINE void SetCombatState(ECombatState state) { WeaponCombatState = state; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EAmmoType GetWeaponAmmoType() const { return WeaponAmmoType; }

	FORCEINLINE FName GetMagazineBoneName() const { return MagazineBoneName; }
	FORCEINLINE void SetMagMoving(bool isMoving) { bMagMoving = isMoving; }
	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; }

	FORCEINLINE int32 GetWeaponSlotIndex() const { return WeaponSlotIndex; }
	FORCEINLINE void SetWeaponSlotIndex(int32 Index) { WeaponSlotIndex = Index; }

	FORCEINLINE USoundCue* GetFireSound() const { return FireSound; } 
	FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return MuzzleFlash; }
	FORCEINLINE FName GetMuzzleSocketName() const { return MuzzleSocketName; }

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

	void UpdateSlideDisplacement();

	void StopSliding();

	void UpdateRecoil();

	void StopRecoil();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponProperties", meta = (AllowPrivateAccess = "true") )
	ECombatState WeaponCombatState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponProperties", meta = (AllowPrivateAccess = "true") )
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponProperties", meta = (AllowPrivateAccess = "true") )
	EAmmoType WeaponAmmoType;

	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;
	float ThrowPower;


	// Damage
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true") )
	float Damage;

	// Damage
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float HeadshotDamage;

	// The speed at which automatic fire happens
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true") )
	float AutoFireRate;

	// Weapon is automatic or not
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	bool bAutomatic;

	// Max Mag Size
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true") )
	int32 MagazineMaxCapacity;

	// Amount of ammo in magazine
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true") )
	int32 Ammo;

	// Name for the clip bone
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true") )
	FName MagazineBoneName;

	// True when magazine is moving while reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true") )
	bool bMagMoving;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true") )
	FName ReloadMontageSection;

	// Slot Index in the InventoryArray of Character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true") )
	int32 WeaponSlotIndex;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true") )
	class UDataTable* WeaponTypeDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true") )
	UTexture2D* InventoryIcon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true") )
	UTexture2D* AmmoIcon;

	// Particle system spawned at muzzle
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true") ) 
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true") )
	FName MuzzleSocketName;

	// Sound played when the weapon is fired
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true") )
	USoundCue* FireSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true") )
	FName BoneNameToHide;

	// Pistol slide displacement amount when fire
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true") )
	float SlideDisplacementAmount;

	// Pistol slide displacement amount when fire
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	float MaxSlideDisplacement;

	// Curve for slide displacement
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* SlidingDisplacementCurve;

	// Timer Handle for slide displacement
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	FTimerHandle SlideDisplacementTimer;

	// Time for displacing the slide during pistol fire
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	float SlideDisplacementTime;

	// True when pistol slide is moving
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	bool bMovingSlide;

	// Max rotation for weapon recoil
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	float MaxRecoilRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = Recoil, meta = (AllowPrivateAccess ="true"))
	UCurveFloat* RecoilCurve;

	// Timer Handle for recoil
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	FTimerHandle RecoilTimer;
	
	// Amount that the weapon rotate during fire
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Recoil, meta = (AllowPrivateAccess = "true"))
	float RecoilRotation;

	// True when the weapon recoils
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	bool bIsRecoiling;
};
