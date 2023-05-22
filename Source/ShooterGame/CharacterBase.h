// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Sound/SoundCue.h"
#include "ItemBase.h"
#include "WeaponBase.h"
#include "AmmoType.h"
#include "CharacterBase.generated.h"

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied				UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress		UMETA(DisplayName = "FireTimerInProgress"),
	ECS_Reloading				UMETA(DisplayName = "Reloading"),

	ECS_MAX						UMETA(DisplayName = "DefaultMAX")
};

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
	/** ĳ���Ϳ� ī�޶� ���̿� ��ġ�� SpringArm */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	/** ĳ���� �þ� ī�޶� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

public:
	/** Spring Arm ���� */
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }

	/** Camera ���� */
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }

private:
	/** ���� ȸ����(Yaw) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	/** ���� ȸ����(Pitch) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	/** Ű���� ���� ȸ����(Yaw) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float HipTurnRate;

	/** Ű���� ���� ȸ����(Pitch) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float HipLookUpRate;

	/** Ű���� �� ����(Yaw) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float AimingTurnRate;

	/** Ű���� �� ����(Pitch)  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float AImingLookUpRate;

	/** ���콺 ���� ����(Yaw) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipTurnRate;

	/** ���콺 ���� ����(Pitch) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate;

	/** ���콺 �� ����(Yaw) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate;

	/** ���콺 �� ����(Pitch) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookUpRate;

public:
	/** �յ� �̵� */
	void MoveForward(float Value);

	/** �¿� �̵� */
	void MoveRight(float Value);

	/** Ű���� Yaw ȸ��(Rate: nomalized rate. (1.0f == 100%))  */
	void TurnAtRate(float Rate);

	/** Ű���� Pitch ȸ��(Rate: nomalized rate. (1.0f == 100%)) */
	void LookUpAtRate(float Rate);

	/** ���콺 Yaw ȸ�� */
	void Turn(float Value);

	/** ���콺 Pitch */
	void LookUp(float Value);

public:
	/** ����� ���� �߻��� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	USoundCue* FireSound;

	/** �߻� �ִϸ��̼� ��Ÿ�� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* HipFireMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* ReloadMontage;

	/** �ѱ� ����Ʈ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UParticleSystem* MuzzleFlash;

	/** �Ѿ� �浹 ����Ʈ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UParticleSystem* ImpactParticles;

	/** �Ѿ� ���� Ʈ���� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UParticleSystem* BeamParticles;

private:
	/** ���� ���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	/** ����Ʈ Fov */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float CameraDefaultFov;

	/** ���� ����(���ذ� ��� ��) Fov */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float CameraZoomedFov;

	/** ���� Fov */
	float CameraCurrentFov;

	/** �� ���� �ӵ� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	/** ���� ���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

public:
	/** ���� ���� ���� */
	FORCEINLINE bool GetAiming() const { return bAiming; }

	/** Fire Button�� ������ �� ȣ�� */
	void FireWeapon();

	/** ���� ���� Ʈ���̽� */
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult);

	/** ���� ���� ���� */
	void AimingButtonPressed();
	void AimingButtonReleased();

	/** �� ���� */
	void CameraInterpZoom(float DeltaTime);

	/** ���� ����(BaseTurn, BaseLookUp) ���� */
	void SetLookRates();

private:
	/** ���ڼ� ������ ���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	/** Character Movement �ӷ¿� ���� ���ڼ� ���� ���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	/** ���߿����� ���ڼ� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;

	/** ���ذ� ��� �� ���ڼ� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;

	/** �� �߻� �� ���ڼ� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	/** ���ڼ��� �� �߻� �ݵ�ȿ�� ������ */
	float ShootTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairShootTimer;

private:
	/** ��Ŭ�� �� */
	bool bFireButtonPressed;

	/** True - �߻� ����. False - �߻� Ÿ�̸Ӱ� �۵� �� */
	bool bShouldFire;

	/** �ڵ� �߻� �ʴ� Ƚ�� */
	float AutomaticFireRate;

	/** �ڵ� �߻� Ÿ�̸� */
	FTimerHandle AutoFireTimer;

public:
	/** �浹ü ������ ��ġ�� ��ȯ�ϴ� ���� Ʈ���̽� */
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);

	/** ���ڼ� ������ ���� ��� */
	void CalculateCrosshairSpread(float DeltaTime);

	/** Crosshair Spread Multiplier ��ȯ */
	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	/** �� ���� ��(LButton Pressed) ���ڼ� �ݵ� ȿ�� Ÿ�̸� ���� */
	void StartCrosshairBulletFire();

	/** ���ڼ� �ݵ� ȿ�� Ÿ�̸� �ʱ�ȭ */
	UFUNCTION()
	void FinishCrosshairBulletFire();

	/** �߻� ��ư Pressed/Released �� ȣ�� */
	void FireButtonPressed();
	void FireButtonReleased();

	/** �ڵ� �߻� Ÿ�̸� Start */
	void StartFireTimer();

	/** �ڵ� �߻� ���� */
	UFUNCTION()
	void AutoFireReset();

private:
	/** �������� AreaSphere ������ �ִٸ� Trace ����(True) */
	bool bShouldTraceForItems;

	/** Overlap �� ������ ���� */
	int8 OverlappedItemCount;

	/** ���������� Ʈ���̽� �� ������ ���۷��� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	AItemBase* TraceHitItemLastFrame;

public:
	/** Overlap �� ������ ���� ��ȯ */
	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

	/** Overlap �� �׸��� �߰�/���� */
	void IncrementOverlappedItemCount(int8 Amount);

	/** ������ Ʈ���̽� */
	void TraceForItems();

private:
	/** ���� ���� ���� ���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	AWeaponBase* EquippedWeapon;

	/** �⺻ ���⸦ ������ ���� Ŭ���� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeaponBase> DefaultWeaponClass;

	/** ���� Ʈ���̽� ���� ������ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	AItemBase* TraceHitItem;

	/** �Ѿ� Ÿ�� ���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	/** 9mm ź ���� ���� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo;

	/** AR ź ���� ���� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int32 StartingARAmmo;

	/** ������ źâ Ʈ������ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FTransform ClipTransform;
	
	/** ĳ���� �� ���� Ʈ������ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;

public:
	/** �⺻ ���� ����, ���� */
	AWeaponBase* SpawnDefaultWeapon();

	/** ȹ���� ���� ���� */
	void EquipWeapon(AWeaponBase* WeaponToEquip);

	/** ���� ���� �� ���� ����߸� */
	void DropWeapon();

	void SelectButtonPressed();
	void SelectButtonReleased();

	void SwapWeapon(AWeaponBase* WeaponToSwap);

	/** ������ ��� */
	void PlayFireSound();

	/** �� �߻� �� */
	void SendBullet();

	/** �߻� �ִϸ��̼� ��Ÿ�� ��� */
	void PlayHipFireMontage();

	/** �Ѿ� �ʱ�ȭ */
	void InitializeAmmoMap();

	/** �Ѿ� ���� ���� */
	bool WeaponHasAmmo();

	/** ������ ���� ���� */
	void ReloadButtonPressed();

	/** ���� ������ */
	void ReloadWeapon();

	/** ������ ���� �� ȣ�� */
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	/** �ش� ���� Ÿ�Կ� �´� �Ѿ����� Ȯ�� */
	bool CarryingAmmo();

	/** Grab Clip ��Ƽ���̿� ȣ��� */
	UFUNCTION(BlueprintCallable)
	void GrabClip();

	/** Replace Clip ��Ƽ���̿� ȣ��� */
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

};