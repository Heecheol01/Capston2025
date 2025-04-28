// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ECharacterBase.h"
#include "GameData/EItemDataAsset.h"
#include "Interface/EBossAIInterface.h"
#include "Interface/ECharacterWidgetInterface.h"
#include "ECharacterBoss.generated.h"

/**
 * 
 */
UCLASS()
class EDEN_API AECharacterBoss : public AECharacterBase, public IEBossAIInterface, public IECharacterWidgetInterface
{
	GENERATED_BODY()

public:
	AECharacterBoss();

protected:
	virtual void BeginPlay() override;

protected:
	virtual void SetDead() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	UEWeaponDataAsset* BossWeapon;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> BaseAttackMontage;
	
	void BaseAttack();
	
	void BaseAttackEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded);
	
	virtual void CloseAttackHitCheck() override;

protected:
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	virtual void SetAIAttackDelegate(const FBaseAttackFinished& InOnAttackFinished) override;
	virtual void AttackByBoss() override;

	FBaseAttackFinished OnAttackFinished;

	virtual void NotifyComboActionEnd() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> HpBar;

	virtual void SetUpCharacterWidget(class UEUserWidget* InUserWidget) override;

public:
	void HealUp(float Amount);
};
