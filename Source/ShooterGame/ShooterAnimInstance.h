// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterBase.h"
#include "WeaponType.h"
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

	/** 앉기 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bCrouching;
	
	/** 무기 교체 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bEquipping;

	/** 캐릭터가w 가속하는지 체크 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	/** 조준 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	/** 캐릭터 이동 Yaw 오프셋 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

	/** 정지하기 직전 Offset Yaw */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;

	/** 재장전 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	bool bReloading;

	/** 에임 오프셋 Yaw */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float RootYawOffset;

	/** 에임 오프셋 Pitch */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float Pitch;

	/** Offset State */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	EOffsetState OffsetState;
	
	/** 방향 틀 때 몸 기울기 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Lean", meta = (AllowPrivateAccess = "true"))
	float YawDelta;

	/** 반동 세기 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float RecoilWeight;
	
	/** 제자리 회전 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bTurningInPlace;

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

	/** 현재 장착 중인 무기 타입 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	EWeaponType EquippedWeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bAbilityQReady;

	/** Ability Shift Start Targeting */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bIsDroneDeploying;

	/** 재장전이나 장착중에는 FABRIK 적용하지 않음 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bShouldUseFABRIK;
	
};
