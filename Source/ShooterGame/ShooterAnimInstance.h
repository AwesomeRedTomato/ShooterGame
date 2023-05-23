// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterBase.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_Aiming			UMETA(DisplayName = "Aiming"),
	EOS_Hip				UMETA(DisplayName = "Hip"),
	EOS_Reloading		UMETA(DisplayName = "Reloading"),
	EOS_InAir			UMETA(DisplayName = "InAir"),

	EOS_MAX				UMETA(DisplayName = "DefaultMAX"),
};

UCLASS()
class SHOOTERGAME_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UShooterAnimInstance();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	ACharacterBase* ShooterCharacter;

protected:
	/** ���ڸ� ȸ�� */
	void TurnInPlace();

	/** �޸� �� �� ���� */
	void Lean(float DeltaTime);

private:
	/** ĳ������ �̵� �ӵ� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed;
	
	/** ĳ���Ͱ� ���߿� �ִ��� üũ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;
	
	/** ĳ���Ͱ�w �����ϴ��� üũ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	/** ĳ���� �̵� Yaw ������ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

	/** �����ϱ� ���� Offset Yaw */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;

	/** ���� ���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	/** ���� ������ Yaw */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float RootYawOffset;

	/** ���� ������ Pitch */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float Pitch;

	/** ������ ���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	bool bReloading;

	/** Offset State */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	EOffsetState OffsetState;
	
	/** �� �������� Rotation Curve */
	float RotationCurve;

	/** ���� �������� Rotation Curve */
	float RotationCurveLastFrame;

	/** ������ ������ �� �� �����ӿ����� ĳ���� ��ݽ� ȸ��(Yaw) */
	float TIPCharacterYaw;
	/** ������ ������ �� ���� �����ӿ����� ĳ���� ��ݽ� ȸ��(Yaw) */
	float TIPCharacterYawLastFrame;

	/** �� �����ӿ����� ĳ���� ��ݽ� ȸ��(Yaw) */
	FRotator CharacterRotation;
	/** ���� �����ӿ����� ĳ���� ��ݽ� ȸ��(Yaw) */
	FRotator CharacterRotationLastFrame;

private:
	/** �޸� �� �� ���⿡ ��� */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Lean", meta = (AllowPrivateAccess = "true"))
	float YawDelta;
	
};
