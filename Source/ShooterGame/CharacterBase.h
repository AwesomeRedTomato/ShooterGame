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

public:
	/** 캐릭터와 카메라 사이에 배치할 SpringArm */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;

	/** 캐릭터를 따라다니는 카메라 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera;

	/** Yaw 회전률(초당 회전 정도(degree)) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseTurnRate;
	
	/** Pitch 회전률(초당 회전 정도(degree)) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseLookUpRate;

public:
	/** 앞뒤 이동 */
	void MoveForward(float Value);

	/** 양옆 이동 */
	void MoveRight(float Value);

	/** Camera Yaw 회전(Rate: nomalized rate. (1.0f == 100%))  */
	void TurnAtRate(float Rate);

	/** Camera Pitch 회전(Rate: nomalized rate. (1.0f == 100%)) */
	void LookUpAtRate(float Rate);

public:
	/** 랜덤한 Sound Cue 재생 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Combat | Sounds")
	USoundCue* FireSound;

	/** 발사 애니메이션 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Combat | Animation")
	UAnimMontage* HipFireMontage;

	/** 총구 이펙트 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Combat | ParticleSystem")
	UParticleSystem* MuzzleFlash;
	
	/** 총알 충돌 이펙트 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | ParticleSystem")
	UParticleSystem* ImpactParticles;
	
	/** 총알 연기 트레일 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | ParticleSystem")
	UParticleSystem* BeamParticles;

public:
	/** Fire Button이 눌렸을 때 호출 */
	void FireWeapon();

	/** 총알 충돌 지점 */
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

};
