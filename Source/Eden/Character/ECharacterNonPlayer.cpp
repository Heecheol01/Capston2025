// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ECharacterNonPlayer.h"

AECharacterNonPlayer::AECharacterNonPlayer()
{
}

void AECharacterNonPlayer::SetDead()
{
	Super::SetDead();

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			Destroy();
		}
	), DeadEventDelayTime, false);
}
