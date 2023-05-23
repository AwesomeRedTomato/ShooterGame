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
	/** 제자리 회전 */
	void TurnInPlace();

	/** 달릴 때 몸 기울기 */
	void Lean(float DeltaTime);

private:
	/** 캐릭터의 이동 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed;
	
	/** 캐릭터가 공중에 있는지 체크 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;
	
	/** 캐릭터가w 가속하는지 체크 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	/** 캐릭터 이동 Yaw 오프셋 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

	/** 정지하기 직전 Offset Yaw */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;

	/** 조준 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	/** 에임 오프셋 Yaw */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float RootYawOffset;

	/** 에임 오프셋 Pitch */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float Pitch;

	/** 재장전 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	bool bReloading;

	/** Offset State */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	EOffsetState OffsetState;
	
	/** 현 프레임의 Rotation Curve */
	float RotationCurve;

	/** 이전 프레임의 Rotation Curve */
	float RotationCurveLastFrame;

	/** 가만히 서있을 때 현 프레임에서의 캐릭터 상반신 회전(Yaw) */
	float TIPCharacterYaw;
	/** 가만히 서있을 때 이전 프레임에서의 캐릭터 상반신 회전(Yaw) */
	float TIPCharacterYawLastFrame;

	/** 현 프레임에서의 캐릭터 상반신 회전(Yaw) */
	FRotator CharacterRotation;
	/** 이전 프레임에서의 캐릭터 상반신 회전(Yaw) */
	FRotator CharacterRotationLastFrame;

private:
	/** 달릴 때 몸 기울기에 사용 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Lean", meta = (AllowPrivateAccess = "true"))
	float YawDelta;
	
};
