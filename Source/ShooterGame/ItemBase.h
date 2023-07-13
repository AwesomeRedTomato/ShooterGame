// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Sound/SoundCue.h"
#include "Engine/DataTable.h"
#include "ItemBase.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Damaged		UMETA(DisplayName = "Damaged"),
	EIR_Common		UMETA(DisplayName = "Common"),
	EIR_UnCommon	UMETA(DisplayName = "UnCommon"),
	EIR_Rare		UMETA(DisplayName = "Rare"),
	EIR_Legendary	UMETA(DisplayName = "Legendary"),

	EIR_MAX			UMETA(DisplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Pickup			UMETA(DisplayName = "Pickup"),
	EIS_EquipInterping	UMETA(DisplayName = "EquipInterping"),
	EIS_PickedUp		UMETA(DisplayName = "PickedUp"),
	EIS_Equipped		UMETA(DisplayName = "Equipped"),
	EIS_Falling			UMETA(DisplayName = "Falling"),

	EIS_MAX				UMETA(DisplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Weapon		UMETA(DisplayName = "Weapon"),
	EIT_Ammo		UMETA(DisplayName = "Ammo"),
	EIT_Potion		UMETA(DisplayName = "Potion"),

	EIT_MAX			UMETA(DisplayName = "DefaultMAX"),
};

USTRUCT(BlueprintType)
struct FItemColorTable : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor GlowColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor LightColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor DarkColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CustomDepthStencil;
};

UCLASS()
class SHOOTERGAME_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** AreaSphere Overlap 시 호출 */
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	/** AreaSphere Overlap 해제 시 호출 */
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	/** 초기 상태 Custom Depth Enabled */
	virtual void InitializeCustomDepth();

	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** 아이템 상태 세팅 */
	virtual void SetItemState(EItemState State);

private:
	/** 라인 트레이스 충돌 상자 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionBox;

	/** 아이템 스켈레탈 메시 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMesh;

	/** 아이템 위젯 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* PickupWidget;

	/** 캐릭터 overlap 범위 구체 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AreaSphere;

	/** 아이템을 획득 효과음 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USoundCue* PickupSound;

	/** 아이템을 장착 효과음 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USoundCue* EquipSound;

	/** 아이템 타입 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	EItemType ItemType;

	/** 위젯에 바인딩 할 아이템 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FString ItemName;

	/** 위젯에 바인딩 할 아이템 개수 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 ItemCount;

	/** 위젯에 바인딩 할 아이템 희귀도 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemRarity ItemRarity;

	/** 위젯에 바인딩 할 아이템의 등급(별 개수) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	TArray<bool> ActiveStars;

	/** 아이템 상태 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemState ItemState;

	/** 아이템 낙하 타이머 */
	FTimerHandle ThrowItemTimer;
	float ThrowItemTime;

	/** 낙하 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	bool bFalling;

	/** 런타임 시점에 변경할 Material Instance Index */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 MaterialIndex;

	/** 런타임 시점에 변경할 Material Instance */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* DynamicMaterialInstance;
	
	/** Dynamic Material Instance에서 사용할 Material Instance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* MaterialInstance;

	/** Custom Depth 수정 가능 여부 */
	bool bCanChangeCustomDepth;

	/** 무기 아이콘 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	UTexture2D* WeaponIcon;

	/** 총알 아이콘 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoIcon;
	 
	/** 슬롯 번호 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 SlotIndex;

	/** 아이템 데이터 테이블 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* ItemColorDataTable;

	/** 위젯에 표시할 색상 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	FLinearColor GlowColor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	FLinearColor LightColor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	FLinearColor DarkColor;

public:
	/** Get */
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
	FORCEINLINE USoundCue* GetPickupSound() const { return PickupSound; }
	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }
	FORCEINLINE int32 GetItemCount() const { return ItemCount; }
	FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; }
	FORCEINLINE FString GetItemName() const { return ItemName; }
	FORCEINLINE UMaterialInstance* GetMaterialInstance() const { return MaterialInstance; }
	FORCEINLINE UMaterialInstanceDynamic* GetDynamicMaterialInstance() const { return DynamicMaterialInstance; }
	FORCEINLINE FLinearColor GetGlowColor() const { return GlowColor; }
	FORCEINLINE int32 GetMaterialIndex() const { return MaterialIndex; }

	/** Set */
	FORCEINLINE void SetSlotIndex(int32 Index) { SlotIndex = Index; }
	FORCEINLINE void SetItemType(EItemType Type) { ItemType = Type; }
	FORCEINLINE void SetItemName(FString Name) { ItemName = Name; }
	FORCEINLINE void SetAmmoIcon(UTexture2D* Icon) { AmmoIcon = Icon; }
	FORCEINLINE void SetWeaponIcon(UTexture2D* Icon) {  WeaponIcon = Icon; }
	FORCEINLINE void SetPickupSound(USoundCue* Sound) { PickupSound = Sound; }
	FORCEINLINE void SetEquipSound(USoundCue* Sound) { EquipSound = Sound; }
	FORCEINLINE void SetMaterialInstance(UMaterialInstance* Instance) { MaterialInstance = Instance; }
	FORCEINLINE void SetDynamicMaterialInstance(UMaterialInstanceDynamic* DynamicInstance) { DynamicMaterialInstance = DynamicInstance; }
	FORCEINLINE void SetGlowColor(FLinearColor Color) { GlowColor = Color; }
	FORCEINLINE void SetMaterialIndex(int32 Index) { MaterialIndex = Index; }

	/** 활성 등급(별 개수) */
	void SetActiveStars();

	/** 상태에 따른 아이템 속성 설정 */
	virtual void SetItemProperties(EItemState State);

	/** 아이템 낙하 효과 */
	void ThrowItem();
	void StopFalling();

	/** 획득 효과음 재생 */
	void PlayPickupSound();

	/** Render Custom Depth ture/false */
	virtual void EnableCustomDepth();
	virtual void DisableCustomDepth();

	void EnableGlowMaterial();
	void DisableGlowMaterial();
};