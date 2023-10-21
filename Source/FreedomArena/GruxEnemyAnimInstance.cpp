// Fill out your copyright notice in the Description page of Project Settings.

#include "GruxEnemyAnimInstance.h"
#include "Enemy.h"

void UGruxEnemyAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (Enemy == nullptr)
	{
		Enemy = Cast<AEnemy>(TryGetPawnOwner());
	}

	if (Enemy)
	{
		FVector velocity = Enemy->GetVelocity();
		velocity.Z = 0;

		Speed = velocity.Size();
	}
}