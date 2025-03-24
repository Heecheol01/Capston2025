// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BossAI/BTTask_WideAoE.h"

#include "AIController.h"
#include "AI/EAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/ECharacterBoss.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_WideAoE::UBTTask_WideAoE()
{
	NodeName = TEXT("WideAoE");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_WideAoE::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp,NodeMemory);

	ElapsedTime = 0.f;
	bHasExploded = false;
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* ControllingPawn = AIController->GetPawn();
	if (!ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	AECharacterBoss* Boss = Cast<AECharacterBoss>(ControllingPawn);

	Boss->GetCharacterMovement()->SetMovementMode(MOVE_None);

	if (AoEMontage)
	{
		Boss->PlayAnimMontage(AoEMontage, 1.f);
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_WideAoE::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp,NodeMemory,DeltaSeconds);

	ElapsedTime += DeltaSeconds;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return;
	}

	APawn* ControllingPawn = AIController->GetPawn();
	if (!ControllingPawn)
	{
		return ;
	}

	AECharacterBoss* Boss = Cast<AECharacterBoss>(ControllingPawn);

	if (!bHasExploded && ElapsedTime >= CastTime)
	{
		TArray<FOverlapResult> Overlaps;
		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(WideAoE), false, ControllingPawn);

		GetWorld()->OverlapMultiByChannel(
			Overlaps,
			ControllingPawn->GetActorLocation(),
			FQuat::Identity,
			ECC_Pawn,
			FCollisionShape::MakeSphere(AoERadius),
			QueryParams);

		for (auto& Result : Overlaps)
		{
			AActor* Target = Result.GetActor();
			if (Target && Target != ControllingPawn)
			{
				UGameplayStatics::ApplyDamage(Target, 100.f, AIController, ControllingPawn, nullptr);
			}
		}

#if ENABLE_DRAW_DEBUG
		DrawDebugSphere(GetWorld(), ControllingPawn->GetActorLocation(), AoERadius, 32, FColor::Red, false, 2.0f);
#endif

		bHasExploded = true;

		OwnerComp.GetBlackboardComponent()->SetValueAsBool(BBKEY_ISSTAGGERED, false);

		Boss->GetCharacterMovement()->SetMovementMode(MOVE_NavWalking);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
