// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"

UShooterAnimInstance::UShooterAnimInstance() 
{
	Speed = 0.0f;
	bIsInAir = false;
	bIsAccelerating = false;
	MovementOffsetYaw = 0.0f;
	bAiming = false;
	CharacterYaw = 0.0f;
	LastMovementOffsetYaw = 0.0f;
	CharacterYawLastFrame = 0.0f;
	RootYawOffset = 0.0f;
}

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

		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		// 마지막으로 이동한 방향(Yaw) 설정
		if (ShooterCharacter->GetVelocity().Size() > 0.0f)
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}

		bAiming = ShooterCharacter->GetAiming();
	}
	TurnInPlace();
}

void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr) return;
	if (Speed > 0)
	{
		RootYawOffset = 0.0f;
		CharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		CharacterYawLastFrame = CharacterYaw;
		RotationCurveLastFrame = 0.0f;
		RotationCurve = 0.0f;
	}
	else
	{
		CharacterYawLastFrame = CharacterYaw;
		CharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		const float YawDelta{ CharacterYaw - CharacterYawLastFrame };

		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - YawDelta);

		const float Turning{ GetCurveValue(TEXT("Turning")) };
		if (Turning > 0)
		{
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));

			const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };

			// Turning Left
			if (RootYawOffset > 0)
			{
				RootYawOffset -= DeltaRotation;
			}
			// Turning Right
			else
			{
				RootYawOffset += DeltaRotation;
			}

			const float ABSRootYawOffset{ FMath::Abs(RootYawOffset) };
			if (ABSRootYawOffset > 90.0f)
			{
				const float YawExcess{ ABSRootYawOffset - 90.0f };
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
	}
}