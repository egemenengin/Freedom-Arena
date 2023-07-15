#pragma once
UENUM(BlueprintType)
enum class ECombatState :uint8
{
	ECS_CanShoot UMETA(DisplayName = "CanShoot"),
	ECS_OutOfAmmo UMETA(DisplayName = "OutOfAmmo"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_ShootingCooldown UMETA(DisplayName = "ShootingCooldown"),
	ECS_Equipping UMETA(DisplayName = "Equipping"),
	ECS_MAX  UMETA(DisplayName = "DEFAULT MAX")
};
UENUM(BlueprintType)
enum class EWeaponType :uint8
{
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_SMG UMETA(DisplayName = "SMG"),
	EWT_BurstRifle UMETA(DisplayName = "BurstRifle"),
	EWT_AutoRifle UMETA(DisplayName = "AutoRifle"),
	EWT_None UMETA(DisplayName = "None"),

	ECS_MAX  UMETA(DisplayName = "DEFAULT MAX")
};
UENUM(BlueprintType)
enum class EAmmoType : uint8 {
	EAT_Light UMETA(DisplayName = "Light"),
	EAT_Heavy UMETA(DisplayName = "Heavy"),
	EAT_None UMETA(DisplayName = "None"),
	EAT_MAX UMETA(DisplayName = "DEFAULT MAX")
};
