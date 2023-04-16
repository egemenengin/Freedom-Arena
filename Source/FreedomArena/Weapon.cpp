// Fill out your copyright notice in the Description page of Project Settings.



#include "Weapon.h"

AWeapon::AWeapon():
	ThrowWeaponTime(0.7f),
	bFalling(false),
	ThrowPower(5'000.f),
	WeaponCombatState(ECombatState::ECS_CanShoot),
	MagazineMaxCapacity(30),
	MagazineBoneName(TEXT("smg_clip"))
{
	PrimaryActorTick.bCanEverTick = true;
	Type = EItemType::EIT_Weapon;

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	ItemAmount = MagazineMaxCapacity;
	Ammo = ItemAmount;
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
