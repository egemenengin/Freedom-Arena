// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "ShooterCharacter.h"

// Sets default values
AItem::AItem():
	Type(EItemType::EIT_Default),
	Rarity(EItemRarity::EIR_Common),
	ItemName(FString("Default")),
	ItemAmount(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Item Mesh");
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("Collision Box");
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);


	PickupWidget = CreateDefaultSubobject<UWidgetComponent>("PickupWidget");
	PickupWidget->SetupAttachment(RootComponent);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	// Hide pickup widget
	PickupWidget->SetVisibility(false);
	
	//Setup overlap for AreaSphere
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	if (PickupWidget != nullptr)
	{
		// Sets the ActiveStars array of bools based on rarity
		SetActiveStars();
	}
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr)
	{
		AShooterCharacter* shooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (shooterCharacter != nullptr)
		{
			shooterCharacter->IncrementOverlappedItemCount(1);
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{

	if (OtherActor != nullptr)
	{
		AShooterCharacter* shooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (shooterCharacter != nullptr)
		{
			shooterCharacter->IncrementOverlappedItemCount(-1);
		}
	}

	// Hide pickup widget
	PickupWidget->SetVisibility(false);
}



// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem::SetActiveStars()
{
	int NumberOfStars = 0;

	switch (Rarity)
	{
		case EItemRarity::EIR_Common:
			NumberOfStars = 1;
			break;
		case EItemRarity::EIR_Uncommon:
			NumberOfStars = 2;
			break;
		case EItemRarity::EIR_Rare:
			NumberOfStars = 3;
			break;
		case EItemRarity::EIR_Legendary:
			NumberOfStars = 4;
			break;
		case EItemRarity::EIR_Mythical:
			NumberOfStars = 5;
			break;
		default:
			NumberOfStars = 0;
	}
	for (int i = 1; i < 6; i++)
	{
		if (i <= NumberOfStars)
		{
			ActiveStars.Add(true);
		}
		else
		{
			ActiveStars.Add(false);
		}
	}
}

