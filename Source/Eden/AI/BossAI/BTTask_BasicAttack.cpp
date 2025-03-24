// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BossAI/BTTask_BasicAttack.h"

#include "AIController.h"
#include "Interface/EBossAIInterface.h"

UBTTask_BasicAttack::UBTTask_BasicAttack()
{
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTask_BasicAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp,NodeMemory);

	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (ControllingPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	IEBossAIInterface* AIPawn = Cast<IEBossAIInterface>(ControllingPawn);
	if (AIPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FBaseAttackFinished OnAttackFinished;
	OnAttackFinished.BindLambda(
		[&]()
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	);
	AIPawn->SetAIAttackDelegate(OnAttackFinished);
	AIPawn->AttackByBoss();
	return EBTNodeResult::InProgress;
}
