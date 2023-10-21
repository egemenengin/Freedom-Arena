// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Enemy.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "ShooterCharacter.h"

AEnemyAIController::AEnemyAIController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	check(BlackboardComponent);

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	check(BehaviorTreeComponent);

	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

	SightConf = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConf->SightRadius = 1000.f;
	SightConf->LoseSightRadius = SightConf->SightRadius + 500.f;
	SightConf->PeripheralVisionAngleDegrees = 45.f;
	SightConf->DetectionByAffiliation.bDetectEnemies = true;

	AIPerceptionComp->ConfigureSense(*SightConf);
	AIPerceptionComp->SetDominantSense(SightConf->GetSenseImplementation());

	SetGenericTeamId(FGenericTeamId(1));
}


void AEnemyAIController::OnPossess(APawn* pawn)
{
	Super::OnPossess(pawn);

	if (pawn == nullptr) { return; }

	AEnemy* enemy = Cast<AEnemy>(pawn);
	if (enemy)
	{
		BlackboardComponent->InitializeBlackboard(*(enemy->GetBehaviorTree()->GetBlackboardAsset()));
	}
	AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerception);

}

void AEnemyAIController::OnPerception(AActor* Actor, FAIStimulus Stimulus)
{

	AShooterCharacter* target = Cast<AShooterCharacter>(Actor);
	
	if (target && Stimulus.WasSuccessfullySensed())
	{
		BlackboardComponent->SetValueAsObject(TEXT("Target"), target);
	}
	else
	{
		BlackboardComponent->ClearValue(TEXT("Target"));
	}








}