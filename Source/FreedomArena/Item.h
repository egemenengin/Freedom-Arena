// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemType: uint8
{
	EIT_Default UMETA(DisplayName = "Default"),
	EIT_Weapon UMETA(DisplayName = "Weapon"),
	EIT_Ammo UMETA(DisplayName = "Ammo"),
	EIT_Money UMETA(DisplayName = "Money"),

	EIT_MAX UMETA(DisplayName = "Default MAX")
};
UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Common UMETA(DisplayName = "Common"),
	EIR_Uncommon UMETA(DisplayName = "Uncommon"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Legendary UMETA(DisplayName = "Legendary"),
	EIR_Mythical UMETA(DisplayName = "Mythical"),

	EIR_MAX UMETA(DisplayName = "Default MAX")
};
UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_NotEquipped UMETA(DisplayName = "NotEquipped"),
	EIS_EquipInterping UMETA(DisplayName = "EquipInterping"),
	EIS_PickedUp UMETA(DisplayName = "PickedUp"),
	EIS_Falling UMETA(DisplayName = "Falling"),

	EIS_MAX UMETA(DisplayName = "Default MAX")
};
UCLASS()
class FREEDOMARENA_API AItem : public AActor
{
	GENERATED_BODY()


public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when overlapping AreaSphere
	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult);

	// Called when end overlapping AreaSphere
	UFUNCTION()
	void OnSphereEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE EItemState GetItemState() const { return State; }
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
	FORCEINLINE EItemType GetItemType() const { return Type; }
	void SetItemState(EItemState itemState);

private:

	//Skeletal Mesh for Item
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMesh;

	// Line trace collides with box to show HUD widgets
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;

	// Widget that shows information about item. Popup when player looks at the item.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* PickupWidget;

	// Enables Item Tracing when overlapped
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AreaSphere;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemType Type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemRarity Rarity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	TArray<bool> ActiveStars;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemState State;
	
	// The name which appears on the PickupWidget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FString ItemName;

	// The amount of ammo, money etc. which appears on the PickupWidget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int64 ItemAmount;

	// The curve asset to use for the item's Z location when interping
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* ItemZCurve;

	// Starting location when interping begins
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FVector ItemInterpStartLocation;
	
	// Target interp location in front of the camera
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FVector InterpTargetLocation;

	// True when interping
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	bool bInterping;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* Character;

	// Plays when it starts interping
	FTimerHandle ItemInterpTimerHandle;

	//Duration of the curve and timer
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float ZCurveTime;

	// Interp speed for X and Y
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float InterpSpeed;

	// Initial yaw offset between and the interping item
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float InterpInitialYawOffset;
	
	// Curve that is used to scaling an item while interping
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ItemScaleCurve;
	
	// Sets the ActiveStars array of bools based on rarity
	UFUNCTION()
	void SetActiveStars();

	// Set properties of the Item's components according to state
	UFUNCTION()
	void SetItemProperties(EItemState itemState);

	// Handle Item interpolation when in the EquipInterping State
	UFUNCTION()
	void ItemInterp(float DeltaTime);

	// Calculate Interpolation Location
	UFUNCTION()
	void CalculateInterpLocation(float ElapsedTime, float DeltaTime);

	// Calculate Interpolation Rotation
	UFUNCTION()
	void CalculateInterpRotation();

	// Calculate Interpolation Scale
	UFUNCTION()
	void CalculateInterpScale(float ElapsedTime);

	// Called when Item interp timer is finished
	UFUNCTION()
	void FinishInterping();

public:
	// Called from the AShooterCharacter class
	UFUNCTION()
	void StartItemCurve(AShooterCharacter* ShooterChar);
};
