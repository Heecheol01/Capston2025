// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_CloseAttackCheck.h"

#include "Interface/EAnimationAttackInterface.h"

void UAnimNotify_CloseAttackCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                          const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		IEAnimationAttackInterface* AttackPawn = Cast<IEAnimationAttackInterface>(MeshComp->GetOwner());
		if (AttackPawn)
		{
			AttackPawn->CloseAttackHitCheck();
		}
	}
}
