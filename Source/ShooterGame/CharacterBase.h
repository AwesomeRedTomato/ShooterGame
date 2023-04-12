// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Sound/SoundCue.h"
#include "CharacterBase.generated.h"

UCLASS()
class SHOOTERGAME_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/** 캐릭터와 카메라 사이에 배치할 SpringArm */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	/** 캐릭터 시야 카메라 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

public:
	/** Spring Arm 리턴 */
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }

	/** Camera 리턴 */
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }

private:
	/** 적용 회전율(Yaw) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;
	
	/** 적용 회전율(Pitch) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	/** 키보드 노줌 회전율(Yaw) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float HipTurnRate;

	/** 키보드 노줌 회전율(Pitch) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float HipLookUpRate;

	/** 키보드 줌 감도(Yaw) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float AimingTurnRate;

	/** 키보드 줌 감도(Pitch)  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float AImingLookUpRate;

	/** 마우스 노줌 감도(Yaw) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipTurnRate;

	/** 마우스 노줌 감도(Pitch) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate;

	/** 마우스 줌 감도(Yaw) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate;

	/** 마우스 줌 감도(Pitch) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookUpRate;

public:
	/** 앞뒤 이동 */
	void MoveForward(float Value);

	/** 좌우 이동 */
	void MoveRight(float Value);

	/** 키보드 Yaw 회전(Rate: nomalized rate. (1.0f == 100%))  */
	void TurnAtRate(float Rate);

	/** 키보드 Pitch 회전(Rate: nomalized rate. (1.0f == 100%)) */
	void LookUpAtRate(float Rate);
	
	/** 마우스 Yaw 회전 */
	void Turn(float Value);

	/** 마우스 Pitch */
	void LookUp(float Value);

public:
	/** 재생할 랜덤 발포음 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Combat")
	USoundCue* FireSound;

	/** 발사 애니메이션 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Combat")
	UAnimMontage* HipFireMontage;

	/** 총구 이펙트 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Combat")
	UParticleSystem* MuzzleFlash;
	
	/** 총알 충돌 이펙트 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UParticleSystem* ImpactParticles;
	
	/** 총알 연기 트레일 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UParticleSystem* BeamParticles;

private:
	/** 줌 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	/** 디폴트 Fov */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float CameraDefaultFov;

	/** 줌 Fov */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float CameraZoomedFov;

	/** 현재 Fov */
	float CameraCurrentFov;

	/** 줌 보간 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

public:
	/** 줌 여부 리턴 */
	FORCEINLINE bool GetAiming() const { return bAiming; }

	/** Fire Button이 눌렸을 때 호출 */
	void FireWeapon();

	/** 총알 충돌 지점 반환 */
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	/** 조준 여부 설정 */
	void AimingButtonPressed();
	void AimingButtonReleased();

	/** 줌 보간 */
	void CameraInterpZoom(float DeltaTime);

	/** 적용 감도(BaseTurn, BaseLookUp) 설정 */ 
	void SetLookRates();
};
