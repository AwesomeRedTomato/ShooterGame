// Fill out your copyright notice in the Description page of Project Settings.


#include "SevarogAnimInstance.h"

void USevarogAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	if (Boss == nullptr)
	{
		Boss = Cast<ABoss>(TryGetPawnOwner());
	}

	if (Boss)
	{
		FVector Velocity{ Boss->GetVelocity() };
		Velocity.Z = 0.0f;
		Speed = Velocity.Size();
	}
}