// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "ShooterCharacter.h"
AAmmo::AAmmo()
{
	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	SetRootComponent(AmmoMesh);

	GetCollisionBox()->SetupAttachment(GetRootComponent());
	GetPickupWidget()->SetupAttachment(GetRootComponent());
	GetAreaSphere()->SetupAttachment(GetRootComponent());


	AutoPickupCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AutoPickupCollisionSphere"));
	AutoPickupCollisionSphere->SetupAttachment(GetRootComponent());
	AutoPickupCollisionSphere->SetSphereRadius(50.f);


}
void AAmmo::BeginPlay()
{
	Super::BeginPlay();

	AutoPickupCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAmmo::OnAutoPickupSphereOverlap);
	AutoPickupCollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AAmmo::EndAutoPickupSphereOverlap);

}
void AAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AAmmo::SetItemProperties(EItemState NewState)
{
	Super::SetItemProperties(NewState);

	switch (NewState)
	{
		case EItemState::EIS_NotEquipped:
			// Set mesh properties
			AmmoMesh->SetSimulatePhysics(false);
			AmmoMesh->SetEnableGravity(false);
			AmmoMesh->SetVisibility(true);
			AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			break;

		case EItemState::EIS_Equipped:
			// Set mesh properties
			AmmoMesh->SetSimulatePhysics(false);
			AmmoMesh->SetEnableGravity(false);
			AmmoMesh->SetVisibility(true);
			AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			break;

		case EItemState::EIS_EquipInterping:
			// Set mesh properties
			AmmoMesh->SetSimulatePhysics(false);
			AmmoMesh->SetEnableGravity(false);
			AmmoMesh->SetVisibility(true);
			AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			break;

		case EItemState::EIS_PickedUp:
			
			break;

		case EItemState::EIS_Falling:
			// Set mesh properties
			AmmoMesh->SetSimulatePhysics(true);
			AmmoMesh->SetEnableGravity(true);
			AmmoMesh->SetVisibility(true);
			AmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AmmoMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	
			break;

		default:
			break;
	}
}

void AAmmo::OnAutoPickupSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor)
	{
		AShooterCharacter* OverlappedCharacter = Cast<AShooterCharacter>(OtherActor);
		if (OverlappedCharacter)
		{
			StartItemCurve(OverlappedCharacter);
			AutoPickupCollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}
void AAmmo::EndAutoPickupSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{

}