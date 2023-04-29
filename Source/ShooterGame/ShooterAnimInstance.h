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
UCLASS()
class SHOOTERGAME_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	ACharacterBase* ShooterCharacter;

public:
	/** ĳ������ �̵� �ӵ� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Speed;
	
	/** ĳ���Ͱ� ���߿� �ִ��� üũ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsInAir;
	
	/** ĳ���Ͱ�w �����ϴ��� üũ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsAccelerating;

	/** ĳ���� �̵� Yaw ������ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementOffsetYaw;

	/** �����ϱ� ���� Offset Yaw */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float LastMovementOffsetYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bAiming;
};
