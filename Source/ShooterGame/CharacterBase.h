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
	/** ĳ���Ϳ� ī�޶� ���̿� ��ġ�� SpringArm */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;

	/** ĳ���͸� ����ٴϴ� ī�޶� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera;

	/** Yaw ȸ����(�ʴ� ȸ�� ����(degree)) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseTurnRate;
	
	/** Pitch ȸ����(�ʴ� ȸ�� ����(degree)) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseLookUpRate;

public:
	/** �յ� �̵� */
	void MoveForward(float Value);

	/** �翷 �̵� */
	void MoveRight(float Value);

	/** Camera Yaw ȸ��(Rate: nomalized rate. (1.0f == 100%))  */
	void TurnAtRate(float Rate);

	/** Camera Pitch ȸ��(Rate: nomalized rate. (1.0f == 100%)) */
	void LookUpAtRate(float Rate);

public:
	/** ������ Sound Cue ��� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Combat | Sounds")
	USoundCue* FireSound;

	/** �߻� �ִϸ��̼� ��Ÿ�� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Combat | Animation")
	UAnimMontage* HipFireMontage;

	/** �ѱ� ����Ʈ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Combat | ParticleSystem")
	UParticleSystem* MuzzleFlash;
	
	/** �Ѿ� �浹 ����Ʈ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | ParticleSystem")
	UParticleSystem* ImpactParticles;
	
	/** �Ѿ� ���� Ʈ���� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | ParticleSystem")
	UParticleSystem* BeamParticles;

public:
	/** Fire Button�� ������ �� ȣ�� */
	void FireWeapon();

	/** �Ѿ� �浹 ���� */
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

};
