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
		// ĳ������ velocity�� ������
		FVector Velocity = ShooterCharacter->GetVelocity();
		Velocity.Z = 0;
		Speed = Velocity.Size();

		// ĳ���Ͱ� ���߿� �� �ִ��� ������
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		// ĳ���� ���� Ȯ��
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