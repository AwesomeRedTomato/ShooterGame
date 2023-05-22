// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

AWeaponBase::AWeaponBase()
{
	Ammo = 30;
	MagazineCapacity = 30;

	WeaponType = EWeaponType::EWT_SubmachineGun;
	AmmoType = EAmmoType::EAT_9mm;
	ReloadMontageSection = FName(TEXT("Reload SMG"));
	
	ClipBoneName = FName(TEXT("smg_clip"));
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
}

void AWeaponBase::DecrementAmmo()
{
	if (Ammo - 1 <= 0)
	{
		Ammo = 0;
	}
	else
	{
		--Ammo;
	}
}

void AWeaponBase::ReloadAmmo(int32 Amount)
{
	checkf(Ammo + Amount <= MagazineCapacity, TEXT("Attempted to reload with more than magazine capacity"));
	Ammo += Amount;
}
