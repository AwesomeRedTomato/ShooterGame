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
	/** źâ�� �� �Ѿ� �� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int32 Ammo;

	/** źâ ���뷮 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int32 MagazineCapacity;

	/** ���� Ÿ�� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;

	/** �ش� ������ �Ѿ� Ÿ�� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;

	/** ��Ÿ�� ���� FName */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FName ReloadMontageSection;

	/** źâ bone �̸� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FName ClipBoneName;

	/** ������ �ϴ� ���� źâ �̵� ���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	bool bMovingClip;


public:
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetMagazineCapacity() const { return MagazineCapacity; }
	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; }
	FORCEINLINE FName GetClipBoneName() const { return ClipBoneName; }

	/** �Ѿ� ���� ���� */
	void DecrementAmmo();

	/** ������ */
	void ReloadAmmo(int32 Amount);

	void SetMovingClip(bool Move) { bMovingClip = Move; }
};
