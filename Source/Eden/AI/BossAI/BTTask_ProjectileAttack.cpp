// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BossAI/BTTask_ProjectileAttack.h"

#include "AIController.h"
#include "Character/ECharacterBoss.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/GameplayStaticsTypes.h"

UBTTask_ProjectileAttack::UBTTask_ProjectileAttack()
{
	NodeName = TEXT("Projectile Attack");
}

EBTNodeResult::Type UBTTask_ProjectileAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp,NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	ACharacter* BossCharacter = Cast<ACharacter>(AIController->GetPawn());
	if (!BossCharacter)
	{
		return EBTNodeResult::Failed;
	}

	UWorld* World = BossCharacter->GetWorld();
	FVector StartLocation = BossCharacter->GetActorLocation() + FVector(0.f, 0.f, 200.f);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(BossCharacter);

	// 보스 머리 위 지점
	FVector BossLoc = BossCharacter->GetActorLocation();
	FVector SpawnBaseLocation = BossLoc + FVector(0.f, 0.f, 200.f);
	
	// 한번에 10발 발사
	int32 NumProjectiles = 10;

	for (int32 i = 0; i < NumProjectiles; i++)
	{
		// 1) 무작위 각도 + 무작위 속도
		float RandomAngle   = FMath::RandRange(0.f, 2.f * PI);  // 0 ~ 360도
		float RandomSpeedXY = FMath::RandRange(600.f, 1000.f); // XY 평면 속도 범위
		float UpwardSpeedZ  = 2000.f;                           // 위로 쏘는 속도 고정 or 랜덤

		// 2) (Angle -> X,Y) 단위 벡터 구하기
		FVector2D Dir2D(FMath::Cos(RandomAngle), FMath::Sin(RandomAngle));

		// 3) 최종 3D 속도
		FVector LaunchVelocity(Dir2D.X * RandomSpeedXY, Dir2D.Y * RandomSpeedXY, UpwardSpeedZ);
		
		// 4) 스폰 파라미터
		FRotator SpawnRotation = BossCharacter->GetActorRotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = BossCharacter;
		SpawnParams.Instigator = BossCharacter;
		
		// 5) 투사체 스폰
		AEBossAttackableProjectile* SpawnedProjectile = BossCharacter->GetWorld()->SpawnActor<AEBossAttackableProjectile>(
			ProjectileClass,
			SpawnBaseLocation,
			SpawnRotation,
			SpawnParams
		);

		ActorsToIgnore.Add(SpawnedProjectile);

		PredictAndDrawImpactPoint(World, StartLocation, LaunchVelocity, 15.f, ActorsToIgnore, 0);

		if (SpawnedProjectile)
		{
			// InitProjectile(0.f)로 초기 Velocity 0
			SpawnedProjectile->InitProjectile(0.f);

			// ProjectileMovementComp가 있을 때, 직접 Velocity 부여 + GravityScale
			if (SpawnedProjectile->MovementComponent)
			{
				SpawnedProjectile->MovementComponent->ProjectileGravityScale = 1.f; 
				SpawnedProjectile->MovementComponent->Velocity = LaunchVelocity;
			}
		}
	}
	
	return EBTNodeResult::Succeeded;
}

void UBTTask_ProjectileAttack::PredictAndDrawImpactPoint(UWorld* World, const FVector& StartLocation,
	const FVector& LaunchVelocity, float CollisionRadius, TArray<AActor*> ActorsToIgnore, float OverrideGravityZ)
{
	if (!World) return;

	// 1) 파라미터 준비
	FPredictProjectilePathParams PredictParams;
	PredictParams.StartLocation = StartLocation;    // 발사 시작 위치
	PredictParams.LaunchVelocity = LaunchVelocity;  // 초기 속도 (포물선)
	PredictParams.bTraceWithCollision = true;       // 충돌 감지 활성화
	PredictParams.ProjectileRadius = CollisionRadius; // 투사체 반경
	PredictParams.ActorsToIgnore = ActorsToIgnore;
	PredictParams.OverrideGravityZ = OverrideGravityZ; // 0이면 default, 아니면 원하는 중력값
	PredictParams.bTraceComplex = true;            // 필요 시 true
	PredictParams.DrawDebugType = EDrawDebugTrace::None; // 함수 자체가 그리는 디버그는 off
	PredictParams.MaxSimTime = 6.0f;

	// 2) 결과 구조체
	FPredictProjectilePathResult PredictResult;

	// 3) 예측 계산
	bool bHit = UGameplayStatics::PredictProjectilePath(World, PredictParams, PredictResult);

	// 4) 최종 충돌 위치(또는 중력에 의해 착지할 위치)
	FVector ImpactPoint;

	if (bHit)
	{
		// 충돌 발생 시 그 지점
		ImpactPoint = PredictResult.HitResult.Location;
	}
	else
	{
		// 충돌 안 하면 마지막 계산된 Trajectory상 최종 위치
		ImpactPoint = PredictResult.LastTraceDestination.Location;
	}

	// 5) 디버그 스피어로 표시(다른 오브젝트 spawn으로 대채 가능)
	DrawDebugSphere(
		World,
		ImpactPoint,
		50.0f,       // 반경
		16,          // 세그먼트
		FColor::Red,
		false,       // 지속 여부
		4.0f         // 몇초 동안 표시할지
	);
}
