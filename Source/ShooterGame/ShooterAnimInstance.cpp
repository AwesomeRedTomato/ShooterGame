// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"

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

		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		// ���������� �̵��� ����(Yaw) ����
		if (ShooterCharacter->GetVelocity().Size() > 0.0f)
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}

		bAiming = ShooterCharacter->GetAiming();
	}
}