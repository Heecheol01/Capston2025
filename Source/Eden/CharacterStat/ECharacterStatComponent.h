// 프로젝트 설정의 Description 페이지에서 저작권 정보를 작성하세요.

#pragma once

#include "CoreMinimal.h"
#include "ECharacterStatType.h"
#include "Components/ActorComponent.h"
#include "Game/CharacterStatSnapshot.h"
#include "GameData/StatDataRow.h"
#include "ECharacterStatComponent.generated.h"

// HP가 0이 되었을 때 발생하는 이벤트 델리게이트 선언
DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE(FLevelChangedDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnExpGainDelegate, float /*InExp*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnExpChangedDelegate, float /*InExp*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate,float /*NewHp*/);

// UECharacterStatComponent 클래스는 캐릭터의 스탯(예: HP, 공격 사거리 등)을 관리하는 컴포넌트입니다.
// Blueprint에서 스폰이 가능하도록 meta 설정을 포함하고 있습니다.
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class EDEN_API UECharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// 생성자: 컴포넌트의 기본 속성 값을 초기화합니다.
	UECharacterStatComponent();

protected:
	// 컴포넌트가 초기화될 때 호출되는 함수.
	// 초기화 작업이나 다른 컴포넌트와의 연동 작업을 수행할 수 있습니다.
	virtual void InitializeComponent() override;

public:
	// HP가 0이 되었을 때 호출되는 델리게이트 (예: 캐릭터 사망 처리)
	FOnHpZeroDelegate OnHpZero;

	// HP가 변경될 때마다 호출되는 델리게이트 (파라미터: 변경된 HP 값)
	FOnHpChangedDelegate OnHpChanged;

	FOnExpGainDelegate OnExpGain;

	FOnExpChangedDelegate OnExpChanged;

	FLevelChangedDelegate OnLevelChanged;

	// 인라인 함수: 현재 HP 값을 반환합니다.
	FORCEINLINE float GetCurrentHp() const { return CurrentHp; }

	// ApplyDamage: 외부에서 데미지를 적용할 때 호출되는 함수.
	// 데미지 값(InDamage)을 받아 HP를 차감하고, 관련 이벤트를 발생시킵니다.
	float ApplyDamage(float InDamage);

protected:
	// SetHp: HP 값을 설정하는 내부 함수.
	// HP 변경 후, OnHpChanged 델리게이트를 호출하고, HP가 0이 되면 OnHpZero 델리게이트를 실행합니다.
	void SetHp(float NewHp);

	// 현재 HP 값.
	// Transient, VisibleInstanceOnly 속성은 런타임 동안에만 유효하며 에디터에서 볼 수 있도록 합니다.
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentHp;

	// 공격 사거리: 캐릭터의 공격 범위를 나타내는 값.
	// Transient, VisibleInstanceOnly 속성으로 설정되어 런타임 동안 값이 유지됩니다.
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	float AttackRadius;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Level)
	int32 CurrentLevel = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Level)
	float CurrentExp = 0;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float BaseMaxHp = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float BaseAttack = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float BaseDefence = 5.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Stat)
	float BaseCriticalChance = 0.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Stat)
	float BaseCriticalDamage = 0.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Stat)
	float BonusMaxHp = 0.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Stat)
	float BonusAttack = 0.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Stat)
	float BonusDefence = 0.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Stat)
	float BonusCriticalChance = 0.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Stat)
	float BonusCriticalDamage = 0.f;


public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetMaxHp(float NewMaxHp) { CurrentHp = NewMaxHp; BaseMaxHp = CurrentHp; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetCurrentLevel() const { return CurrentLevel; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32  GetCurrentExp() const { return CurrentExp; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetMaxHp() const { return BaseMaxHp + BonusMaxHp * 10; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetAttack() const {return BaseAttack + BonusAttack * 5;}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetDefense() const {return BonusDefence + BonusDefence * 3;}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCriticalChance() const {return BaseCriticalChance + BonusCriticalChance;}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCriticalDamage() const { return BaseCriticalDamage + BonusCriticalDamage * 10; }
	
	UFUNCTION(BlueprintCallable)
	void HealUp(float Amount); 

// 경험치 섹션
public:
	UFUNCTION(BlueprintCallable)
	void AddExp(float InExp);

protected:
	void LevelUp();

public:
	UPROPERTY(EditAnywhere,Category = "Stat")
	UDataTable* StatDataTable;

	const FStatDataRow* GetStatRow(int32 Level) const;

// 스탯 섹션
public:
	int32 StatPoints = 3;

	UFUNCTION(BlueprintCallable)
	float GetBonusStat(ECharacterStatType StatType) const;
	
	UFUNCTION(BlueprintCallable)
	void AddBonusStat(ECharacterStatType StatType);

// stat snapshot 섹션
public:
	UFUNCTION(BlueprintCallable)
	FCharacterStatSnapshot MakeStatSnapshot() const;

	UFUNCTION(BlueprintCallable)
	void ApplyStatSnapshot(const FCharacterStatSnapshot& InSnapshot);
};