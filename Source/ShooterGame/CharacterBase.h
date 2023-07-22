// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Sound/SoundCue.h"
#include "ItemBase.h"
#include "WeaponBase.h"
#include "AmmoType.h"
#include "Ammo.h"
#include "CharacterBase.generated.h"

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied				UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress		UMETA(DisplayName = "FireTimerInProgress"),
	ECS_Reloading				UMETA(DisplayName = "Reloading"),
	ECS_Equipping				UMETA(DisplayNmae = "Equipping"),

	ECS_MAX						UMETA(DisplayName = "DefaultMAX")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);

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

	virtual void Jump() override;

	/** 캡슐 높이 변경 보간 */
	void InterpCapsuleHalfHeight(float DeltaTime);

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
	/** 발사 애니메이션 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* HipFireMontage;
	
	/** 재장전 애니메이션 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* ReloadMontage;

	/** 무기 교체 애니메이션 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* EquipMontage;

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

	/** 조준 버튼(마우스 우측 버튼) 눌림 여부 */
	bool bAimingButtonPressed;

private:
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
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }

	/** 조준 */
	void Aim();
	void StopAiming();

	/** Fire Button이 눌렸을 때 호출 */
	void FireWeapon();

	/** 에임 라인 트레이싱 */
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult);

	/** 조준 여부 설정 */
	void AimingButtonPressed();
	void AimingButtonReleased();

	/** 줌 보간 */
	void CameraInterpZoom(float DeltaTime);

	/** 적용 감도(BaseTurn, BaseLookUp) 설정 */
	void SetLookRates();

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

private:
	/** 좌클릭 시 */
	bool bFireButtonPressed;

	/** 자동 발사 */
	float AutomaticFireRate;

	/** True - 발사 가능. False - 발사 타이머가 작동 중 */
	bool bShouldFire;

	/** 자동 발사 타이머 */
	FTimerHandle AutoFireTimer;

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

	/** 아이템 라인 트레이스 */
	void TraceForItems();

	/** 아이템 줍기 */
	void GetPickupItem(AItemBase* Item);

private:
	/** 현재 장착중인 무기 수정 예정 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	AWeaponBase* EquippedWeapon;

	/** 기본 무기를 세팅할 무기 클래스 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeaponBase> DefaultWeaponClass;

	/** 현재 트레이스 중인 아이템 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	AItemBase* TraceHitItem;

	/** 총알 타입 Map */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	/** 9mm 탄 시작 개수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo;

	/** AR 탄 시작 개수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int32 StartingARAmmo;

	/** 재장전 탄창 트랜스폼 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FTransform ClipTransform;
	
	/** 캐릭터 손 월드 트랜스폼 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;

public:
	/** 기본 무기 스폰, 장착 */
	AWeaponBase* SpawnDefaultWeapon();

	/** 획득한 무기 장착 */
	void EquipWeapon(AWeaponBase* WeaponToEquip, bool bSwapping = false);

	/** 무기 해제 시 땅에 떨어뜨림 */
	void DropWeapon();

	/** 줍기 버튼(E) 눌렀을 때 실행 */
	void SelectButtonPressed();
	void SelectButtonReleased();

	/** 무기 교체 */
	void SwapWeapon(AWeaponBase* WeaponToSwap);

	/** 발포음 재생 */
	void PlayFireSound();

	/** 총 발사 슝 */
	void SendBullet();

	/** 발사 애니메이션 몽타주 재생 */
	void PlayHipFireMontage();

	/** 총알 초기화 */
	void InitializeAmmoMap();

	/** 총알 소지 여부 */
	bool WeaponHasAmmo();

	/** 재장전 여부 설정 */
	void ReloadButtonPressed();

	/** 무기 재장전 */
	void ReloadWeapon();

	/** FinishReload 노티파이에 의해 호출 */
	UFUNCTION(BlueprintCallable)
	void FinishReloading();
	
	/** FinishEquipping 노티파이에 의해 호출*/
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	/** 해당 무기 타입에 맞는 총알인지 확인 */
	bool CarryingAmmo();

	/** GrabClip 노티파이에 의해 호출 시  */
	UFUNCTION(BlueprintCallable)
	void GrabClip();

	/** Replace Clip 노티파이에 의해 호출됨 */
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

	/** 총알 획득 시 충전 및 재장전 */
	void PickupAmmo(AAmmo* Ammo);

private:
	/** 앉기 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bCrouching;

	/** 기본 이동 속도 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float BaseMovementSpeed;
	
	/** 앉았을 때 이동 속도 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CrouchMovementSpeed;

	/** 현재 캡슐 절반 높이 */
	float CurrentCapsuleHalfHeight;

	/** 서있을 때 캡슐 절반 높이 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float StandingCapsuleHalfHeight;

	/** 앉았을 때 캡슐 절반 높이 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CrouchingCapsuleHalfHeight;

	/** 기본 지면 마찰 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float BaseGroundFriction;
	
	/** 앉았을 때 지면 마찰 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CrouchingGroundFriction;

public:
	FORCEINLINE bool GetCrouching() const { return bCrouching; }

	/** 좌측Ctrl */
	void CrouchButtonPressed();

private:
	/** TArray 인벤토리 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<AWeaponBase*> Inventory;

	/** 장비 수용량(현재 장착 무기 포함) */
	const int32 INVENTORY_CAPACITY{ 2 };

	/** Delegate */
	UPROPERTY(BlueprintAssignable, Category = "Delegates", meta = (AllowPrivateAccess = "true"))
	FEquipItemDelegate EquipItemDelegate;

public:
	/** 장비 슬롯 변경 */
	void OneKeyPressed();
	void TwoKeyPressed();
	void ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex);

	FORCEINLINE AWeaponBase* GetEquippedWeapon() const { return EquippedWeapon; }
};