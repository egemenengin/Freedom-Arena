// Fill out your copyright notice in the Description page of Project Settings.



#include "Weapon.h"

AWeapon::AWeapon():
	ThrowWeaponTime(0.7f),
	bFalling(false),
	ThrowPower(5'000.f),
	WeaponCombatState(ECombatState::ECS_CanShoot),
	MagazineMaxCapacity(30),
	MagazineBoneName(TEXT("smg_clip")),
	WeaponSlotIndex(0)
{
	PrimaryActorTick.bCanEverTick = true;
	Type = EItemType::EIT_Weapon;

}
void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (WeaponTypeDataTable != nullptr)
	{
		FWeaponDataTableRowBase* WeaponDataRow = nullptr;
		switch (WeaponType)
		{
			case EWeaponType::EWT_Pistol:
				WeaponDataRow = WeaponTypeDataTable->FindRow<FWeaponDataTableRowBase>(FName("Pistol"), TEXT(""), true);
				break;
			case EWeaponType::EWT_SMG:
				WeaponDataRow = WeaponTypeDataTable->FindRow<FWeaponDataTableRowBase>(FName("SMG"), TEXT(""), true);
				break;
			case EWeaponType::EWT_BurstRifle:
				WeaponDataRow = WeaponTypeDataTable->FindRow<FWeaponDataTableRowBase>(FName("BurstRifle"), TEXT(""), true);
				break;
			case EWeaponType::EWT_AutoRifle:
				WeaponDataRow = WeaponTypeDataTable->FindRow<FWeaponDataTableRowBase>(FName("AutoRifle"), TEXT(""), true);
				break;
			default:
				break;
		}
		if (WeaponDataRow != nullptr)
		{
			SetWeaponTypeData(*WeaponDataRow);
		}
	}
	
}
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	ItemAmount = Ammo;

	if (Ammo <= 0)
	{
		SetCombatState(ECombatState::ECS_OutOfAmmo);
	}

}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetItemState() == EItemState::EIS_Falling && bFalling)
	{
		FRotator MeshRotation = FRotator(0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f);
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AWeapon::ThrowWeapon()
{
	ItemAmount = Ammo;

	ToggleGlowMaterial(true);

	FRotator MeshRotation = FRotator(0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f);
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	FVector ImpulseDirection = GetItemMesh()->GetForwardVector();
	//FVector MeshRight = GetItemMesh()->GetRightVector();
	
	float RandomRotation = FMath::FRandRange(45.f, 135.f);
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, FVector(0.f, 0.f, 1.f));

	ImpulseDirection *= ThrowPower;
	GetItemMesh()->AddImpulse(ImpulseDirection);
	bFalling = true;

	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime, false);
}

bool AWeapon::WeaponHasAmmo()
{
	if (Ammo > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AWeapon::DecrementAmmo()
{
	if (Ammo - 1 <= 0)
	{
		Ammo = 0;
	}
	else
	{
		Ammo--;
	}
}

bool AWeapon::ReloadWeapon(TMap<EAmmoType, int32>& AmmoMap)
{
	int32 neededAmmo = MagazineMaxCapacity - Ammo;

	if (AmmoMap[WeaponAmmoType] > 0 && neededAmmo != 0)
	{
		if (neededAmmo >= AmmoMap[WeaponAmmoType])
		{
			Ammo += AmmoMap[WeaponAmmoType];
			AmmoMap[WeaponAmmoType] = 0;
		}
		else
		{
			Ammo += neededAmmo;
			AmmoMap[WeaponAmmoType] -= neededAmmo;
		}
		return true;
	}
	else
	{
		return false;
	}

}
void AWeapon::StopFalling()
{
	bFalling = false;
	SetItemState(EItemState::EIS_NotEquipped);
}

void AWeapon::SetWeaponTypeData(struct FWeaponDataTableRowBase& WeaponDataRow)
{
	WeaponAmmoType = WeaponDataRow.AmmoType;
	MagazineMaxCapacity = WeaponDataRow.MagCapacity;
	Ammo = WeaponDataRow.Ammo;
	Damage = WeaponDataRow.Damage;
	GetItemMesh()->SetSkeletalMesh(WeaponDataRow.WeaponMesh);
	PickUpSound = WeaponDataRow.PickupSound;
	EquipSound = WeaponDataRow.EquipSound;
	ItemName = WeaponDataRow.WeaponName;
	InventoryIcon = WeaponDataRow.InventoryIcon;
	AmmoIcon = WeaponDataRow.AmmoIcon;
	GetItemMesh()->SetMaterial(GetMaterialIndex(), nullptr);
	SetMaterialInstance(WeaponDataRow.MaterialInstance);
	SetMaterialIndex(WeaponDataRow.MaterialIndex);

	HandleMaterialInstances();
}
