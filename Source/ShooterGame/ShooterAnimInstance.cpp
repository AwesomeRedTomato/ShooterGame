// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"

UShooterAnimInstance::UShooterAnimInstance() 
{
	Speed = 0.0f;
	MovementOffsetYaw = 0.0f;
	LastMovementOffsetYaw = 0.0f;
	
	bIsInAir = false;
	bIsAccelerating = false;
	bAiming = false;
	bReloading = false;
	
	TIPCharacterYaw = 0.0f;
	TIPCharacterYawLastFrame = 0.0f;
	
	RootYawOffset = 0.0f;
	Pitch = 0.0f;
	OffsetState = EOffsetState::EOS_Hip;
	
	CharacterRotation = FRotator(0.0f);
	CharacterRotationLastFrame = FRotator(0.0f);
	YawDelta = 0.0f;
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
		bCrouching = ShooterCharacter->GetCrouching();
		bReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;

		FVector Velocity = ShooterCharacter->GetVelocity();
		Velocity.Z = 0;
		Speed = Velocity.Size();

		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

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

		if (bReloading)
		{
			OffsetState = EOffsetState::EOS_Reloading;
		}
		else if (bIsInAir)
		{
			OffsetState = EOffsetState::EOS_InAir;
		}
		else if(ShooterCharacter->GetAiming())
		{
			OffsetState = EOffsetState::EOS_Aiming;
		}
		else
		{
			OffsetState = EOffsetState::EOS_Hip;
		}
	}
	TurnInPlace();

	Lean(DeltaTime);
}

void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr) return;

	Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;

	if (Speed > 0 || bIsInAir)
	{
		RootYawOffset = 0.0f;
		
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		TIPCharacterYawLastFrame = TIPCharacterYaw;
	
		RotationCurveLastFrame = 0.0f;
		RotationCurve = 0.0f;
	}
	else
	{
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		
		const float TIPYawDelta{ TIPCharacterYaw - TIPCharacterYawLastFrame };
		

		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TIPYawDelta);

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

void UShooterAnimInstance::Lean(float DeltaTime)
{
	if (ShooterCharacter == nullptr) return;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ShooterCharacter->GetActorRotation();

	FRotator Delta{ UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame) };

	const float Target{ Delta.Yaw / DeltaTime };
	const float Interp{ FMath::FInterpTo(YawDelta, Target, DeltaTime, 6.0f) };

	YawDelta = FMath::Clamp(Interp, -90.0f, 90.0f);
}
