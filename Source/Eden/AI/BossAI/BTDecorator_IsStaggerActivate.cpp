// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BossAI/BTDecorator_IsStaggerActivate.h"
#include "AI/EAI.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_IsStaggerActivate::UBTDecorator_IsStaggerActivate()
{
	NodeName = TEXT("Check IsStaggered");
}

bool UBTDecorator_IsStaggerActivate::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	bool bBaseResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	if (!bBaseResult)
	{
		return false;
	}

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (!BBComp)
	{
		return false;
	}
	
	const bool bStaggerActive = BBComp->GetValueAsBool(BBKEY_ISSTAGGERED);
	return bStaggerActive;
}
