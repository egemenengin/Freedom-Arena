// Fill out your copyright notice in the Description page of Project Settings.

#include "BTT_EnemyAttack.h"
#include "Enemy.h"
#include "AIController.h"

UBTT_EnemyAttack::UBTT_EnemyAttack()
{
	NodeName = "EnemyAttack";
}

EBTNodeResult::Type UBTT_EnemyAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	if (OwnerComp.GetAIOwner() == nullptr) 
	{ 
		return EBTNodeResult::Failed; 
	}
	AEnemy* Owner = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	
	if (Owner == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FName MontageSectionName = GetRandomMontageSectionName();
	Owner->PlayAttackMontage(MontageSectionName);

	return EBTNodeResult::Succeeded;
}

FName UBTT_EnemyAttack::GetRandomMontageSectionName()
{
	FName SectionName;
	int SectionInd = FMath::RandRange(1, 4);

	switch (SectionInd)
	{
		case 1:
			SectionName = "AttackL";
			break;
		case 2:
			SectionName = "AttackR";
			break;
		case 3:
			SectionName = "AttackLFast";
			break;
		case 4:
			SectionName = "AttackRFast";
			break;
	}


	return SectionName;
}