// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

AWeaponBase::AWeaponBase()
{
	SetItemType(EItemType::EIT_Weapon);

	// Ammmo
	Ammo = 30;
	MagazineCapacity = 30;

	// Type
	WeaponType = EWeaponType::EWT_SubmachineGun;
	AmmoType = EAmmoType::EAT_9mm;
	
	// Reload
	ReloadMontageSection = FName(TEXT("Reload SMG"));
	ClipBoneName = FName(TEXT("smg_clip"));
	bMovingClip = false;
	
	// Slide
	bMovingSlide = false;
	SlideDisplacement = 0.0f;
	MaxSlideDisplacement = 4.0f;
	SlideDisplacementTime = 0.2f;

	bAutomatic = true;
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 권총 슬라이드
	UpdateSlideDisplacement();
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	if (BoneToHide != FName(""))
	{
		GetItemMesh()->HideBoneByName(BoneToHide, EPhysBodyOp::PBO_None);
	}
}

void AWeaponBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FString WeaponTablePath{ TEXT("DataTable'/Game/DataTable/DT_Weapon.DT_Weapon'") };
	UDataTable* WeaponTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *WeaponTablePath));

	if (WeaponTableObject)
	{
		FWeaponDataTable* WeaponDataRow = nullptr;

		switch (WeaponType)
		{
		case EWeaponType::EWT_SubmachineGun:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("SubmachineGun"), TEXT(""));
			break;

		case EWeaponType::EWT_AssaultRifle:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("AssaultRifle"), TEXT(""));
			break;

		case EWeaponType::EWT_Pistol:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("Pistol"), TEXT(""));
			break;

		case EWeaponType::EWT_MAX:
			break;
		default:
			break;
		}

		// Set Data Row
		if (WeaponDataRow)
		{
			AmmoType = WeaponDataRow->AmmoType;
			Ammo = WeaponDataRow->WeaponAmmo;
			MagazineCapacity = WeaponDataRow->MagazingCapacity;
			SetPickupSound(WeaponDataRow->PickupSound);
			SetEquipSound(WeaponDataRow->EquipSound);
			GetItemMesh()->SetSkeletalMesh(WeaponDataRow->ItemMesh);
			SetItemName(WeaponDataRow->ItemName);
			SetWeaponIcon(WeaponDataRow->WeaponIcon);
			SetAmmoIcon(WeaponDataRow->AmmoIcon);
			SetMaterialInstance(WeaponDataRow->MaterialInstance);
			PreviousMaterialIndex = GetMaterialIndex();
			GetItemMesh()->SetMaterial(PreviousMaterialIndex, nullptr);
			SetMaterialIndex(WeaponDataRow->MaterialIndex);
			SetClipBoneName(WeaponDataRow->ClipBoneName);
			SetReloadMontageSection(WeaponDataRow->ReloadMontageSection);
			GetItemMesh()->SetAnimInstanceClass(WeaponDataRow->AnimBP);
			SetFireSound(WeaponDataRow->FireSound);
			AutoFireRate = WeaponDataRow->AutoFireRate;
			MuzzleFlash = WeaponDataRow->MuzzleFlash;
			bAutomatic = WeaponDataRow->bAutomatic;

			CrosshairsMiddle = WeaponDataRow->CrosshairsMiddle;
			CrosshairsLeft	= WeaponDataRow->CrosshairsLeft;
			CrosshairsRight = WeaponDataRow->CrosshairsRight;
			CrosshairsTop = WeaponDataRow->CrosshairsTop;
			CrosshairsBottom = WeaponDataRow->CrosshairsBottom;

			BoneToHide = WeaponDataRow->BoneToHide;
			GetItemMesh()->HideBoneByName(BoneToHide, PBO_None);
		}

		if (GetMaterialInstance())
		{
			SetDynamicMaterialInstance(UMaterialInstanceDynamic::Create(GetMaterialInstance(), this));
			GetDynamicMaterialInstance()->SetVectorParameterValue(TEXT("FresnelColor"), GetGlowColor());

			GetItemMesh()->SetMaterial(GetMaterialIndex(), GetDynamicMaterialInstance());

			EnableGlowMaterial();
		}
	}
}

void AWeaponBase::FinishMovingSlide()
{
	bMovingSlide = false;
}

void AWeaponBase::UpdateSlideDisplacement()
{
	if (SlideDisplacementCurve && bMovingSlide)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateSlide"));

		const float ElapsedTime{ GetWorldTimerManager().GetTimerElapsed(SlideTimer) };
		const float CurveValue{ SlideDisplacementCurve->GetFloatValue(ElapsedTime) };
		SlideDisplacement = CurveValue * MaxSlideDisplacement;
	}
}

bool AWeaponBase::ClipIsFull()
{
	return Ammo >= MagazineCapacity;
}

void AWeaponBase::StartSlideTimer()
{
	bMovingSlide = true;

	GetWorldTimerManager().SetTimer(
		SlideTimer,
		this,
		&AWeaponBase::FinishMovingSlide,
		SlideDisplacement);
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
