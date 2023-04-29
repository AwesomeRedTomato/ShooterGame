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
	/** 캐릭터의 이동 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Speed;
	
	/** 캐릭터가 공중에 있는지 체크 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsInAir;
	
	/** 캐릭터가w 가속하는지 체크 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsAccelerating;

	/** 캐릭터 이동 Yaw 오프셋 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementOffsetYaw;

	/** 정지하기 직전 Offset Yaw */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float LastMovementOffsetYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bAiming;
};
