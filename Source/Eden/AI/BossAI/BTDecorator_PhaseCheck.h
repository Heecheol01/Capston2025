// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_PhaseCheck.generated.h"

/**
 * 
 */
UCLASS()
class EDEN_API UBTDecorator_PhaseCheck : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_PhaseCheck();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Phase)
	int32 RequiredPhase = 0;
};
