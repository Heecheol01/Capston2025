// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateBossHp.generated.h"

/**
 * 
 */
UCLASS()
class EDEN_API UBTService_UpdateBossHp : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateBossHp();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
