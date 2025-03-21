// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ECharacterBase.h"
#include "Interface/EGeneralAIInterface.h"
#include "ECharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class EDEN_API AECharacterNonPlayer : public AECharacterBase, public IEGeneralAIInterface
{
	GENERATED_BODY()
	
public:
	AECharacterNonPlayer();

protected:
	void SetDead() override;

protected:
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	virtual void SetAIAttackDelegate(const FGeneralAIAttackFinished& InOnAttackFinished) override;
	virtual void AttackByAI() override;

	FGeneralAIAttackFinished OnAttackFinished;

	virtual void NotifyComboActionEnd() override;
};
