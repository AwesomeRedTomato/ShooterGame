// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"

void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<ACharacterBase>(TryGetPawnOwner());

}

void UShooterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<ACharacterBase>(TryGetPawnOwner());
	}

	if (ShooterCharacter)
	{
		// 캐릭터의 velocity를 가져옴
		FVector Velocity = ShooterCharacter->GetVelocity();
		Velocity.Z = 0;
		Speed = Velocity.Size();

		// 캐릭터가 공중에 떠 있는지 가져옴
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		// 캐릭터 가속 확인
		if (Speed > 0.0f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}
	}
}