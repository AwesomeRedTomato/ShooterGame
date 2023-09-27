// Fill out your copyright notice in the Description page of Project Settings.


#include "SevarogAnimInstance.h"

void USevarogAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	if (BossEnemy == nullptr)
	{
		BossEnemy = Cast<ABossEnemy>(TryGetPawnOwner());
	}

	if (BossEnemy)
	{
		FVector Velocity{ BossEnemy->GetVelocity() };
		Velocity.Z = 0.0f;
		Speed = Velocity.Size();
	}
}