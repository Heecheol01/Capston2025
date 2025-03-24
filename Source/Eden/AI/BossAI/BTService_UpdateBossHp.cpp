// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BossAI/BTService_UpdateBossHp.h"

#include "AIController.h"
#include "AI/EAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/ECharacterBoss.h"
#include "CharacterStat/ECharacterStatComponent.h"

UBTService_UpdateBossHp::UBTService_UpdateBossHp()
{
	NodeName = TEXT("UpdateBossHp");
	Interval = 0.5f;
}

void UBTService_UpdateBossHp::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp,NodeMemory,DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return;
	}

	IEBossAIInterface* AIPawn = Cast<IEBossAIInterface>(ControllingPawn);
	if (!AIPawn)
	{
		return;
	}

	AECharacterBoss* Boss = Cast<AECharacterBoss>(ControllingPawn);

	UECharacterStatComponent* StatComp = Boss->FindComponentByClass<UECharacterStatComponent>();

	const float CurrentHp = StatComp->GetCurrentHp();
	const float MaxHp = StatComp->GetMaxHp();
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(BBKEY_BOSSHP, CurrentHp / MaxHp * 100.0f);

	if ((CurrentHp / MaxHp * 100.f) <= 70.0f)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(BBKEY_PHASEINDEX, 2);
	}
	
	if ((CurrentHp / MaxHp * 100.f) <= 30.0f)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(BBKEY_PHASEINDEX, 3);
	}
}
