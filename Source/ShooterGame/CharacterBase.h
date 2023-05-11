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
#include "CharacterBase.generated.h"

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_9mm		UMETA(DisplayName = "9mm"),
	EAT_AR		UMETA(DisplayName = "AssaultRifle"),

	EAT_NAX		UMETA(DisplayName = "DefaultMAX")
};

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
	/** 재생할 랜덤 발사음 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	USoundCue* FireSound;

	/** 발사 애니메이션 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* HipFireMontage;

	/** 총구 이펙트 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UParticleSystem* MuzzleFlash;

	/** 총알 충돌 이펙트 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UParticleSystem* ImpactParticles;

	/** 총알 연기 트레일 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UParticleSystem* BeamParticles;

private:
	/** 조준 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	/** 디폴트 Fov */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float CameraDefaultFov;

	/** 조준 감도(조준경 사용 시) Fov */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float CameraZoomedFov;

	/** 현재 Fov */
	float CameraCurrentFov;

	/** 줌 보간 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	/** 전투 상태 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

public:
	/** 조준 여부 리턴 */
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

	void PlayFireSound();

	void SendBullet();

	void PlayHipFireMontage();

private:
	/** 십자선 퍼지는 정도 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	/** Character Movement 속력에 따른 십자선 퍼짐 정도 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	/** 공중에서의 십자선 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;

	/** 조준경 사용 시 십자선 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;

	/** 총 발사 시 십자선 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	/** 십자선의 총 발사 반동효과 변수들 */
	float ShootTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairShootTimer;

public:
	/** 충돌체 정보와 위치를 반환하는 라인 트레이스 */
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);

	/** 십자선 퍼지는 정도 계산 */
	void CalculateCrosshairSpread(float DeltaTime);

	/** Crosshair Spread Multiplier 반환 */
	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	/** 총 발포 시(LButton Pressed) 십자선 반동 효과 타이머 세팅 */
	void StartCrosshairBulletFire();

	/** 십자선 반동 효과 타이머 초기화 */
	UFUNCTION()
	void FinishCrosshairBulletFire();

private:
	/** 좌클릭 시 */
	bool bFireButtonPressed;

	/** True - 발사 가능. False - 발사 타이머가 작동 중 */
	bool bShouldFire;

	/** 자동 발사 초당 횟수 */
	float AutomaticFireRate;

	/** 자동 발사 타이머 */
	FTimerHandle AutoFireTimer;

public:
	/** 발사 버튼 Pressed/Released 시 호출 */
	void FireButtonPressed();
	void FireButtonReleased();

	/** 자동 발사 타이머 Start */
	void StartFireTimer();

	/** 자동 발사 리셋 */
	UFUNCTION()
	void AutoFireReset();

private:
	/** 아이템의 AreaSphere 범위에 있다면 Trace 가능(True) */
	bool bShouldTraceForItems;

	/** Overlap 된 아이템 개수 */
	int8 OverlappedItemCount;

	/** 마지막으로 트레이스 한 아이템 레퍼런스 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	AItemBase* TraceHitItemLastFrame;

public:
	/** Overlap 된 아이템 개수 반환 */
	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

	/** Overlap 된 항목의 추가/삭제 */
	void IncrementOverlappedItemCount(int8 Amount);

	/** 아이템 트레이스 */
	void TraceForItems();

private:
	/** 현재 장착 중인 무기 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	AWeaponBase* EquippedWeapon;

	/** 기본 무기를 세팅할 무기 클래스 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeaponBase> DefaultWeaponClass;

	/** 현재 트레이스 중인 아이템 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	AItemBase* TraceHitItem;

public:
	/** 기본 무기 스폰, 장착 */
	AWeaponBase* SpawnDefaultWeapon();

	/** 획득한 무기 장착 */
	void EquipWeapon(AWeaponBase* WeaponToEquip);

	/** 무기 해제 시 땅에 떨어뜨림 */
	void DropWeapon();

	void SelectButtonPressed();
	void SelectButtonReleased();

	void SwapWeapon(AWeaponBase* WeaponToSwap);

private:
	/** 총알 타입 매핑 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	/** 9mm 탄 시작 개수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo;

	/** AR 탄 시작 개수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 StartingARAmmo;

public:
	/** 총알 초기화 */
	void InitializeAmmoMap();

	/** 총알 소지 여부 */
	bool WeaponHasAmmo();

};