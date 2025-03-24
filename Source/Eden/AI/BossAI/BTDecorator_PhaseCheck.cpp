// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BossAI/BTDecorator_PhaseCheck.h"

#include "AI/EAI.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_PhaseCheck::UBTDecorator_PhaseCheck()
{
	NodeName = TEXT("PhaseCheck");
}

bool UBTDecorator_PhaseCheck::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult =  Super::CalculateRawConditionValue(OwnerComp,NodeMemory);

	UBlackboardComponent* BBComponent = OwnerComp.GetBlackboardComponent();
	if (!BBComponent)
	{
		return false;
	}

	const int32 CurrentPhase = BBComponent->GetValueAsInt(BBKEY_PHASEINDEX);

	bResult = (CurrentPhase >= RequiredPhase);
	
	return bResult;
}
