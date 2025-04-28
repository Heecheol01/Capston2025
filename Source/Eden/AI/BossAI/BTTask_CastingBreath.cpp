// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BossAI/BTTask_CastingBreath.h"

#include "AIController.h"
#include "AI/EAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/ECharacterBoss.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_CastingBreath::UBTTask_CastingBreath()
{
	NodeName = TEXT("Casting Breath");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_CastingBreath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp,NodeMemory);

	ElapsedTime = 0.f;
	bDidBreath = false;
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	AECharacterBoss* Boss = Cast<AECharacterBoss>(AIController->GetPawn());
	if (!Boss)
	{
		return EBTNodeResult::Failed;
	}

	Boss->GetCharacterMovement()->SetMovementMode(MOVE_None);

	if (CastingMontage)
	{
		if (UAnimInstance* AnimInstance = Boss->GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(CastingMontage, 1.0f);
		}
	}
	
	return EBTNodeResult::InProgress;
}

void UBTTask_CastingBreath::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp,NodeMemory,DeltaSeconds);

	ElapsedTime += DeltaSeconds;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return;
	}

	APawn* Boss = AIController->GetPawn();
	if (!Boss)
	{
		return;
	}

	AECharacterBoss* BossCharcter = Cast<AECharacterBoss>(AIController->GetPawn());
	if (!BossCharcter)
	{
		return;
	}

	if (ElapsedTime < CastTime)
	{
		return;
	}

	if (!bDidBreath)
	{
		bDidBreath = true;

		if (BreathMontage)
		{
			if (UAnimInstance* AnimInst = BossCharcter->GetMesh()->GetAnimInstance())
			{
				AnimInst->Montage_Play(BreathMontage, 1.0f);
			}
		}
	}

	float TimeSinceBreathStart = ElapsedTime - CastTime;
	if (TimeSinceBreathStart <= BreathTime)
	{
		DamageAccum += DeltaSeconds;

		if (DamageAccum >= DamageInterval)
		{
			TArray<AActor*> AllTargets;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllTargets);

			for (AActor* Target : AllTargets)
			{
				if (Target == BossCharcter)
				{
					continue;
				}

				FVector ToTarget = Target->GetActorLocation() - Boss->GetActorLocation();
				float Distance = ToTarget.Size();
				ToTarget.Normalize();

				float Dot = FVector::DotProduct(BossCharcter->GetActorForwardVector(), ToTarget);
				float Degree = FMath::RadiansToDegrees(FMath::Acos(Dot));

				if (Distance <= Range && Degree <= Angle)
				{
					UE_LOG(LogTemp, Warning, TEXT("Breath Applied"));
					UGameplayStatics::ApplyDamage(Target, 20.f, AIController, BossCharcter, nullptr);
				}
				
			}
#if ENABLE_DRAW_DEBUG
			DrawDebugCone(
				Boss->GetWorld(),
				Boss->GetActorLocation(),
				Boss->GetActorForwardVector(),
				Range,
				FMath::DegreesToRadians(Angle),
				FMath::DegreesToRadians(Angle),
				12,
				FColor::Red,
				false,
				0.1f  // Tick마다 재그리면 0.1초면 충분
			);
#endif

			DamageAccum = 0.f;
		}
		// 아직 브레스 끝나지 않았으므로 그대로 진행
		return;
	}

	if (UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent())
	{
		BBComp->SetValueAsBool(BBKEY_ISCASTING, false);
	}

	BossCharcter->GetCharacterMovement()->SetMovementMode(MOVE_NavWalking);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
