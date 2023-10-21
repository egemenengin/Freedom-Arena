#pragma once

UENUM(BlueprintType)
enum class EEnemyStatus :uint8
{
	EES_Patrol UMETA(DisplayName = "Patrol"),
	EES_Chase UMETA(DisplayName = "Chasing"),
	EES_Attack UMETA(DisplayName = "Attack"),
	EES_Stunned UMETA(DisplayName = "Stunned"),
	EES_Dead UMETA(DisplayName = "Dead"),

	EES_MAX UMETA(DisplayName = "MAX")
};