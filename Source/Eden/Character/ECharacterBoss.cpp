// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ECharacterBoss.h"
#include "AI/BossAI/EBossAIController.h"
#include "CharacterStat/ECharacterStatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Physics/ECollision.h"
#include "UI/DamageFloatingText.h"
#include "UI/EEnemyHPBarWidget.h"
#include "UI/EHpBarWidget.h"
#include "UI/EWidgetComponent.h"

AECharacterBoss::AECharacterBoss()
{
	AIControllerClass = AEBossAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_ENPCCAPSULE);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BossMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Eden/Characters/Warrok_W_Kurniawan.Warrok_W_Kurniawan'"));
	if (BossMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(BossMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> BossAnimInstanceClassRef(TEXT("/Game/Eden/Animation/Boss/ABP_EBoss.ABP_EBoss_C"));
	if (BossAnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(BossAnimInstanceClassRef.Class);
	}
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> BossDeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Eden/Animation/Boss/AM_Dead.AM_Dead'"));
	if (BossDeadMontageRef.Object)
	{
		DeadMontage = BossDeadMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UEWeaponDataAsset> BossWeaponDataRef(TEXT("/Script/Eden.WeaponDataAsset'/Game/Eden/GameData/WeaponData/DA_BossAttack.DA_BossAttack'"));
	if (BossWeaponDataRef.Object)
	{
		BossWeapon = BossWeaponDataRef.Object;;
	}

	HpBar = CreateDefaultSubobject<UEWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0, 0, 250.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/Eden/UI/WBP_HpBarWidget.WBP_HpBarWidget_C"));
	if (HpBarWidgetRef.Class)
	{
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(100.f, 10.f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	bIsStaggerInProgress = false;
	CurrentStaggerGauge = 0;
}

void AECharacterBoss::BeginPlay()
{
	Super::BeginPlay();

	Stat->SetMaxHp(1500.f);

	if (AEBossAIController* BossAIController = Cast<AEBossAIController>(GetController()))
	{
		BossAIController->RunBossAI();
	}
}

void AECharacterBoss::SetDead()
{
	Super::SetDead();

	AEBossAIController* AEAIController = Cast<AEBossAIController>(GetController());
	if (AEAIController)
	{
		AEAIController->StopBossAI();
	}

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			Destroy();
		}
	), DeadEventDelayTime, false);
}

void AECharacterBoss::BaseAttack()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(BaseAttackMontage, 0.55f/*AttackSpeedRate*/);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AECharacterBoss::BaseAttackEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, BaseAttackMontage);
}

void AECharacterBoss::BaseAttackEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	NotifyComboActionEnd();
}

void AECharacterBoss::SetAIAttackDelegate(const FBaseAttackFinished& InOnAttackFinished)
{
	OnAttackFinished = InOnAttackFinished;
}

void AECharacterBoss::AttackByBoss()
{
	BaseAttack();
}

void AECharacterBoss::NotifyComboActionEnd()
{
	Super::NotifyComboActionEnd();
	OnAttackFinished.ExecuteIfBound();
}

void AECharacterBoss::CloseAttackHitCheck()
{
	// 충돌 정보를 저장할 변수
	FHitResult OutHitResult;
	// 콜리전 쿼리 파라미터 설정 (자신은 제외)
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	// 공격 범위, 반경, 그리고 데미지 값을 정의합니다.
	const float AttackRange = 400.f;
	const float AttackRadius = 80.f;
	const float AttackDamage = 50.f;

	// 공격 시작 지점: 캐릭터 위치에 콜리전 캡슐 반경만큼 전진한 위치
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	// 공격 끝 지점: 시작 지점에서 추가로 전진한 위치
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	// 지정된 범위 내에서 스윕 충돌 검사를 수행합니다.
	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, CCHANNEL_EACTION, FCollisionShape::MakeSphere(AttackRadius), Params);

	// 적중 시, 대상 액터에 데미지를 적용합니다.
	if (HitDetected)
	{
		FDamageEvent DamageEvent;
		OutHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);

		FVector SpawnLoc = OutHitResult.GetActor()->GetActorLocation() + FVector(0,0,100);
		GetWorld()->SpawnActor<ADamageFloatingText>(ADamageFloatingText::StaticClass(),SpawnLoc,FRotator::ZeroRotator)->Init(AttackDamage);
	}

	// 디버그 목적으로 공격 범위를 시각화합니다.
#if ENABLE_DRAW_DEBUG

	// 디버그 캡슐의 중심과 반 높이를 계산합니다.
	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	// 적중 여부에 따라 색상을 결정합니다 (적중: 초록, 미적중: 빨강).
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

	// 캡슐 형태로 공격 범위를 디버그 드로잉합니다.
	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);

#endif
}

float AECharacterBoss::GetAIPatrolRadius()
{
	return 100.f;
}

float AECharacterBoss::GetAIDetectRange()
{
	return 1000.f;
}

float AECharacterBoss::GetAIAttackRange()
{
	return 400.f;
}

float AECharacterBoss::GetAITurnSpeed()
{
	return 1.f;
}

void AECharacterBoss::SetUpCharacterWidget(class UEUserWidget* InUserWidget)
{
	if (UEHpBarWidget* HpBarWidget = Cast<UEHpBarWidget>(InUserWidget))
	{
		HpBarWidget->BindStatComponent(Stat);
		HpBarWidget->UpdateHpBar(Stat->GetCurrentHp());
		Stat->OnHpChanged.AddUObject(HpBarWidget, &UEHpBarWidget::UpdateHpBar);
	}
}

void AECharacterBoss::HealUp(float Amount)
{
	Stat->HealUp(Amount);
}
