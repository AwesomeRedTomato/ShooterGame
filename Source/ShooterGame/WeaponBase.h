// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "WeaponBase.generated.h"

/**
 *
 */
UCLASS()
class SHOOTERGAME_API AWeaponBase : public AItemBase
{
	GENERATED_BODY()

public:
	AWeaponBase();

protected:
	virtual void BeginPlay() override;

private:
	// �Ѿ� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		int32 Ammo;

public:
	FORCEINLINE int32 GetAmmo() const { return Ammo; }

	// �Ѿ� ���� ����
	void DecrementAmmo();
};
