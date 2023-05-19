// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "WeaponEnums.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class FREEDOMARENA_API AAmmo : public AItem
{
	GENERATED_BODY()
public:
	AAmmo();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

	// Override of SetItemProperties to set AmmoMesh properties 
	virtual void SetItemProperties(EItemState State) override;

	UFUNCTION()
	void OnAutoPickupSphereOverlap(
			UPrimitiveComponent* OverlappedComponent, 
			AActor* OtherActor, 
			UPrimitiveComponent* OtherComponent, 
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);
	UFUNCTION()
	void EndAutoPickupSphereOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComponent,
			int32 OtherBodyIndex);
private:

	// Mesh for the ammo 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* AmmoMesh;

	//Ammo type 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;

	// Ammo Icon for Pickup Widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoIconTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AutoPickupCollisionSphere;
public:

	// Getters
	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }

};
