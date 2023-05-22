// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "AmmoType.h"
#include "WeaponBase.generated.h"

/**
 *
 */

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_SubmachineGun	UMETA(DisplayName = "SubmachineGun"),
	EWT_AssaultRifle	UMETA(DisplayName = "AssaultRifle"),
	
	EWT_MAX				UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class SHOOTERGAME_API AWeaponBase : public AItemBase
{
	GENERATED_BODY()

public:
	AWeaponBase();

	void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	/** 탄창에 든 총알 수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int32 Ammo;

	/** 탄창 수용량 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int32 MagazineCapacity;

	/** 무기 타입 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;

	/** 해당 무기의 총알 타입 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;

	/** 몽타주 섹션 FName */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FName ReloadMontageSection;

	/** 탄창 bone 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FName ClipBoneName;

	/** 재장전 하는 동안 탄창 이동 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	bool bMovingClip;


public:
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetMagazineCapacity() const { return MagazineCapacity; }
	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; }
	FORCEINLINE FName GetClipBoneName() const { return ClipBoneName; }

	/** 총알 개수 감소 */
	void DecrementAmmo();

	/** 재장전 */
	void ReloadAmmo(int32 Amount);

	void SetMovingClip(bool Move) { bMovingClip = Move; }
};
