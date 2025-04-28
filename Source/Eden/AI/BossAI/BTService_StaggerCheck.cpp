// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BossAI/BTService_StaggerCheck.h"

#include "AIController.h"
#include "AI/EAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/ECharacterBoss.h"

UBTService_StaggerCheck::UBTService_StaggerCheck()
{
	NodeName = TEXT("StaggerCheck");

	Interval = 2.0f;
}

void UBTService_StaggerCheck::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp,NodeMemory,DeltaSeconds);

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon)
	{
		return;
	}

	AECharacterBoss* Boss = Cast<AECharacterBoss>(AICon->GetPawn());
	if (!Boss)
	{
		return;
	}
	
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (!BBComp)
	{
		return;
	}

	// 1) Phase1인지 확인
	const int32 CurrentPhase = BBComp->GetValueAsInt(BBKEY_PHASEINDEX);
	if (CurrentPhase != 1)
	{
		// Phase1이 아니면 무력화 체크 중단
		return;
	}

	// 2) 보스 HP 가져오기 (절댓값인지 퍼센트인지 프로젝트 설계에 따라 다름)
	const float BossHP = BBComp->GetValueAsFloat(BBKEY_BOSSHP);
	
	// 예) BossHP가 100 기준이라면, 85 이하 => 85%, 75 이하 => 75%로 가정

	// 3) HP <= 85 && 아직 트리거 안 했으면 → 무력화 패턴 발동
	if (BossHP <= 85.0f && !bIsTriggered85)
	{
		UE_LOG(LogTemp, Log, TEXT("[StaggerCheck] HP 85%% 이하 진입! 무력화 트리거"));
		UE_LOG(LogTemp, Log, TEXT("%f"), BossHP);
		BBComp->SetValueAsBool(BBKEY_ISSTAGGERED, true);
		Boss->bIsStaggerInProgress = true;
		bIsTriggered85 = true;
	}

	// 4) HP <= 75 && 아직 트리거 안 했으면 → 무력화 패턴 발동
	if (BossHP <= 75.0f && !bIsTriggered75)
	{
		UE_LOG(LogTemp, Log, TEXT("[StaggerCheck] HP 75%% 이하 진입! 무력화 트리거"));
		UE_LOG(LogTemp, Log, TEXT("%f"), BossHP);
		BBComp->SetValueAsBool(BBKEY_ISSTAGGERED, true);
		Boss->bIsStaggerInProgress = true;
		bIsTriggered75 = true;
	}
}
