// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ECharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "EComboActionData.h"
#include "Animation/EAnimInstance.h"
#include "Physics/ECollision.h"
#include "Engine/DamageEvents.h"
#include "CharacterStat/ECharacterStatComponent.h"
#include "Item/EArrow.h"
#include "UI/DamageFloatingText.h"

// Sets default values
AECharacterBase::AECharacterBase()
{
	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/Eden/Animation/Default/ABP_ECharacter.ABP_ECharacter_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Eden/Animation/Default/AM_Dead.AM_Dead'"));
	if (DeadMontageRef.Object)
	{
		DeadMontage = DeadMontageRef.Object;
	}

	static ConstructorHelpers::FClassFinder<AEArrow> ArrowBPRef(TEXT("/Game/Eden/Item/BP_Arrow.BP_Arrow_C"));
	if (ArrowBPRef.Class)
	{
		ArrowBP = ArrowBPRef.Class;
	}

	// 스탯 컴포넌트
	Stat = CreateDefaultSubobject<UECharacterStatComponent>(TEXT("Stat"));
}

void AECharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Stat->OnHpZero.AddUObject(this, &AECharacterBase::SetDead);
}

void AECharacterBase::SetWeaponData(UEWeaponDataAsset* NewWeaponData)
{
	if (CurrentWeaponData == NewWeaponData)
	{
		return;
	}

	CurrentWeaponData = NewWeaponData;
	OnWeaponDataChanged.Broadcast(NewWeaponData->Weapon);
}

// ProcessComboCommand: 콤보 커맨드를 처리하는 함수입니다.
// - 현재 콤보가 진행 중이지 않다면 콤보 액션을 시작합니다.
// - 타이머가 유효하지 않으면 다음 콤보 명령이 없음을 설정하고, 유효하다면 추가 콤보 입력이 들어왔음을 표시합니다.
void AECharacterBase::ProcessComboCommand()
{
	if (GetCharacterMovement()->IsFalling())
	{
		return;
	}
	
	if (CurrentCombo == 0)
	{
		ComboActionBegin();
		return;
	}

	if (!ComboTimerHandle.IsValid())
	{
		HasNextComboCommand = false;
	}
	else
	{
		HasNextComboCommand = true;
	}
}

// ComboActionBegin: 콤보 액션을 시작하는 함수입니다.
// - 콤보 카운트를 1로 초기화하고, 이동을 중지합니다.
// - 지정된 콤보 애니메이션 몽타주를 재생하고, 몽타주 종료 델리게이트를 바인딩합니다.
// - 이후 콤보 입력을 체크하기 위한 타이머를 설정합니다.
void AECharacterBase::ComboActionBegin()
{
	// 콤보 상태 초기화
	CurrentCombo = 1;
	UEAnimInstance* AnimInstance = Cast<UEAnimInstance>(GetMesh()->GetAnimInstance());

	if (!AnimInstance->bIsBow)
	{
		// 이동 중지: 콤보 실행 중에는 캐릭터가 움직이지 않도록 설정
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	}

	ComboActionMontage = CurrentWeaponData->AttackMontage;
	
	// 애니메이션 설정: 콤보 애니메이션 몽타주 재생
	const float AttackSpeedRate = CurrentWeaponData->AttackSpeed;
	
	AnimInstance->Montage_Play(ComboActionMontage, AttackSpeedRate);

	// 몽타주 종료 델리게이트 설정: 콤보 종료 시 호출될 함수 바인딩
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AECharacterBase::ComboActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, ComboActionMontage);

	// 기존 타이머 무효화 후, 콤보 입력 체크 타이머 설정
	ComboTimerHandle.Invalidate();
	SetComboCheckTimer();
}

// ComboActionEnd: 콤보 액션이 종료될 때 호출되는 함수입니다.
// - 콤보 진행 상태를 초기화하고, 이동 모드를 다시 걷기로 전환합니다.
// - 파생 클래스에서 추가 처리가 필요할 경우 NotifyComboActionEnd()를 호출합니다.
void AECharacterBase::ComboActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	// 콤보 진행 상태가 올바르지 않으면 경고를 발생시킴
	ensure(CurrentCombo != 0);
	CurrentCombo = 0;

	// 이동 모드를 다시 걷기로 설정
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	// 콤보 종료 후 추가 처리를 위한 가상 함수 호출 (파생 클래스에서 오버라이드 가능)
	NotifyComboActionEnd();
}

// NotifyComboActionEnd: 콤보 액션 종료 시 호출되는 가상 함수입니다.
// 파생 클래스에서 콤보 종료 후 추가적인 동작을 구현할 수 있도록 설계되었습니다.
void AECharacterBase::NotifyComboActionEnd()
{
	// 기본 구현은 빈 함수입니다.
}

// SetComboCheckTimer: 콤보 입력의 유효 시간(Effective Time)을 계산하여 타이머를 설정합니다.
// - ComboActionData에 정의된 프레임 수와 프레임 레이트를 사용하여 유효 시간을 결정합니다.
void AECharacterBase::SetComboCheckTimer()
{
	// 현재 콤보 인덱스 계산 (0부터 시작)
	int32 ComboIndex = CurrentCombo - 1;
	ensure(CurrentWeaponData->ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIndex));
	
	const float AttackSpeedRate = CurrentWeaponData->AttackSpeed;
	// 유효 시간 계산: (프레임 수 / 프레임 레이트) / 공격 속도
	float ComboEffectiveTime = (CurrentWeaponData->ComboActionData->EffectiveFrameCount[ComboIndex] / CurrentWeaponData->ComboActionData->FrameRate) / AttackSpeedRate;
	if (ComboEffectiveTime > 0.0f)
	{
		// 설정한 시간 후 ComboCheck() 함수가 호출되도록 타이머를 시작합니다.
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &AECharacterBase::ComboCheck, ComboEffectiveTime, false);
	}
}

// ComboCheck: 타이머 만료 시 추가 콤보 입력 여부를 확인하는 함수입니다.
// - 추가 콤보 입력이 있다면, 콤보 카운트를 증가시키고 애니메이션 몽타주의 다음 섹션으로 전환합니다.
void AECharacterBase::ComboCheck()
{
	// 타이머 무효화
	ComboTimerHandle.Invalidate();
	if (HasNextComboCommand)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		// 현재 콤보 카운트를 1 증가시키며 최대 콤보 제한을 넘지 않도록 합니다.
		CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, CurrentWeaponData->ComboActionData->MaxComboCount);
		// 다음 콤보 애니메이션 섹션 이름 생성 (예: Section1, Section2, ...)
		FName NextSection = *FString::Printf(TEXT("%s%d"), *CurrentWeaponData->ComboActionData->MontageSectionNamePrefix, CurrentCombo);
		// 몽타주 내에서 다음 섹션으로 점프하여 애니메이션을 전환합니다.
		AnimInstance->Montage_JumpToSection(NextSection, ComboActionMontage);
		// 추가 콤보 입력 확인을 위한 타이머 재설정
		SetComboCheckTimer();
		// 다음 콤보 명령 플래그 초기화
		HasNextComboCommand = false;
	}
}

// AttackHitCheck: 공격이 상대에게 적중했는지 확인하는 함수입니다.
// - Sweep 방식으로 콜리전 체크를 수행하여, 명시된 범위 내의 대상에게 데미지를 적용합니다.
void AECharacterBase::AttackHitCheck()
{
	// 충돌 정보를 저장할 변수
	FHitResult OutHitResult;
	// 콜리전 쿼리 파라미터 설정 (자신은 제외)
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	// 공격 범위, 반경, 그리고 데미지 값을 정의합니다.
	const float AttackRange = CurrentWeaponData->AttackRange;
	const float AttackRadius = CurrentWeaponData->AttackRadius;
	const float AttackDamage = CurrentWeaponData->BaseDamage;

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

// TakeDamage: 캐릭터가 데미지를 받을 때 호출되는 함수입니다.
// - 상위 클래스의 TakeDamage를 호출한 후, Stat 컴포넌트에 데미지를 적용합니다.
float AECharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// 상위 클래스의 데미지 처리를 실행합니다.
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 캐릭터의 스탯 컴포넌트에 데미지를 반영합니다.
	Stat->ApplyDamage(DamageAmount);

	if (bIsStaggerInProgress)
	{
		CurrentStaggerGauge += DamageAmount;
	}

	// 적용된 데미지 값을 반환합니다.
	return DamageAmount;
}

void AECharacterBase::CloseAttackHitCheck()
{
}

void AECharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	SetActorEnableCollision(false);
}

void AECharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadMontage, 1.0f);
}
