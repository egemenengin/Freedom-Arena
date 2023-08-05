// Fill out your copyright notice in the Description page of Project Settings.



#include "Weapon.h"

AWeapon::AWeapon() :
	ThrowWeaponTime(0.7f),
	bFalling(false),
	ThrowPower(5'000.f),
	WeaponCombatState(ECombatState::ECS_CanShoot),
	MagazineMaxCapacity(30),
	MagazineBoneName(TEXT("smg_clip")),
	WeaponSlotIndex(0),
	SlideDisplacementAmount(0),
	SlideDisplacementTime(0.5f),
	MaxSlideDisplacement(4.f),
	bMovingSlide(false),
	MaxRecoilRotation(20.f),
	bIsRecoiling(false)
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

	if (BoneNameToHide != FName(""))
	{
		GetItemMesh()->HideBoneByName(BoneNameToHide, EPhysBodyOp::PBO_None);
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
	UpdateSlideDisplacement();
	UpdateRecoil();

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

void AWeapon::StartSlideTimer()
{
	bMovingSlide = true;
	GetWorldTimerManager().SetTimer(SlideDisplacementTimer, this, &AWeapon::StopSliding, SlideDisplacementTime, false);
}

void AWeapon::StartRecoil()
{
	bIsRecoiling = true;
	GetWorldTimerManager().SetTimer(RecoilTimer, this, &AWeapon::StopRecoil, AutoFireRate, false);
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
	bAutomatic = WeaponDataRow.bAutomatic;
	AutoFireRate = WeaponDataRow.AutoFireRate;
	MaxRecoilRotation = WeaponDataRow.MaxRecoilRotation;

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

	MagazineBoneName = WeaponDataRow.MagBoneName;
	ReloadMontageSection = WeaponDataRow.ReloadMontageSection;

	GetItemMesh()->SetAnimInstanceClass(WeaponDataRow.AnimBP);

	MuzzleFlash = WeaponDataRow.MuzzleFlash;
	FireSound = WeaponDataRow.FireSound;

	BoneNameToHide = WeaponDataRow.BoneNameToHide;
	MuzzleSocketName = WeaponDataRow.MuzzleSocketName;

	RecoilCurve = WeaponDataRow.RecoilCurve;
}

void AWeapon::UpdateSlideDisplacement()
{
	if ( (SlidingDisplacementCurve != nullptr) && (bMovingSlide == true) )
	{
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(SlideDisplacementTimer);
		const float CurveValue = SlidingDisplacementCurve->GetFloatValue(ElapsedTime);

		SlideDisplacementAmount = CurveValue * MaxSlideDisplacement;
	}
}

void AWeapon::StopSliding()
{
	bMovingSlide = false;
}

void AWeapon::UpdateRecoil()
{
	if ((RecoilCurve != nullptr) && (bIsRecoiling == true))
	{
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(RecoilTimer);
		const float CurveValue = RecoilCurve->GetFloatValue(ElapsedTime);
		RecoilRotation = CurveValue * MaxRecoilRotation;
	}
}

void AWeapon::StopRecoil()
{
	bIsRecoiling = false;
}
