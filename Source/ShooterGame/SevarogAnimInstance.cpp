// Fill out your copyright notice in the Description page of Project Settings.


#include "SevarogAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "CharacterBase.h"

void USevarogAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	if (Boss == nullptr)
	{
		Boss = Cast<ABoss>(TryGetPawnOwner());
	}

	if (Target == nullptr)
	{
		Target = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}

	if (Boss)
	{
		// Boss Movement Speed
		FVector Velocity{ Boss->GetVelocity() };
		Velocity.Z = 0.0f;
		Speed = Velocity.Size();

		// Boss Rotation Angle
		FVector Start{ Boss->GetActorForwardVector() };
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Start, Velocity);
		Angle = LookAtRotation.Yaw;

		Pitch = Boss->GetBaseAimRotation().Pitch;
		Yaw = Boss->GetBaseAimRotation().Yaw;

	}
}