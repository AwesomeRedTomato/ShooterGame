#include "CharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "BulletHitInterface.h"
#include "Enemy.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Character의 회전이 Controller의 영향을 받지 않음
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	// 캐릭터 무브먼트
	bCanMove = true;
	bCrouching = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 340.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 600.0f;
	
	BaseMovementSpeed = 650.0f;
	CrouchMovementSpeed = 300.0f;
	StandingCapsuleHalfHeight = 88.0f;
	CrouchingCapsuleHalfHeight = 44.0f;
	BaseGroundFriction = 2.0f;
	CrouchingGroundFriction = 100.0f;

	// SpringArm 초기화
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 180.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SocketOffset = FVector(0.0f, 50.0f, 100.0f);

	// Camera 초기화
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;	// 카메라 회전을 막아놓는다.

	// 회전율
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;

	// Fov
	bAiming = false;
	bAimingButtonPressed = false;
	CameraDefaultFov = 0.0f; // BeginPlay에서 값 설정
	CameraZoomedFov = 50.0f;
	CameraCurrentFov = 0.0f;
	ZoomInterpSpeed = 20.0f;

	// 조준경 사용/미사용 감도
	HipTurnRate = 90.0f;
	HipLookUpRate = 90.0f;
	AimingTurnRate = 20.0f;
	AImingLookUpRate = 20.0f;

	// 마우스 감도
	MouseHipTurnRate = 1.0f;
	MouseHipLookUpRate = 1.0f;
	MouseAimingTurnRate = 0.2f;
	MouseAimingLookUpRate = 0.2f;

	// 십자선
	CrosshairSpreadMultiplier = 0.0f;
	CrosshairVelocityFactor = 0.0f;
	CrosshairInAirFactor = 0.0f;
	CrosshairAimFactor = 0.0f;
	CrosshairShootingFactor = 0.0f;
	bFiringBullet = false;
	ShootTimeDuration = 0.05f;

	// 자동 발사
	bFireButtonPressed = false;
	bShouldFire = true;

	// 아이템 트레이스
	bShouldTraceForItems = false;
	OverlappedItemCount = 0;

	// 총알
	Starting9mmAmmo = 85;
	StartingARAmmo = 120;

	// 전투
	CombatState = ECombatState::ECS_Unoccupied;

	// HandSceneComponent
	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComponent"));

	MaxHealth = 200.0f;
	Health = 200.0f;
	DestroyTime = 8.0f;
	bDead = false;

	bAbility_Q_Ready = false;
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;

	if (Camera)
	{
		CameraDefaultFov = GetCamera()->FieldOfView;
		CameraCurrentFov = CameraDefaultFov;
	}

	// 기본 무기 스폰 후 소켓에 장착
	if (SpawnDefaultWeapon())
	{
		EquipWeapon(SpawnDefaultWeapon());

	}
	Inventory.Add(EquippedWeapon);
	EquippedWeapon->SetSlotIndex(0);
	EquippedWeapon->DisableCustomDepth();
	EquippedWeapon->DisableGlowMaterial();
	
	// 총알 초기화
	InitializeAmmoMap();
}

void ACharacterBase::Jump()
{
	if (bCanMove == false) return;

	if (bCrouching)
	{
		bCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
	else
	{
		ACharacter::Jump();
	}
}

void ACharacterBase::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight{};

	if (bCrouching)
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}
	else
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}

	const float InterpHalfHeight = FMath::FInterpTo(
		GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), 
		TargetCapsuleHalfHeight,
		DeltaTime,
		20.0f);
	
	// 앉았을 때 음수, 서있을 때 양수 값을 가짐
	const float DeltaCapsuleHalfHeight{ InterpHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };
	const FVector MeshOffset{ 0.0f, 0.0f, -DeltaCapsuleHalfHeight };

	// 캡슐의 절반 높이만큼 메시를 올려줌
	GetMesh()->AddLocalOffset(MeshOffset);

	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 줌 보간
	CameraInterpZoom(DeltaTime);

	// 줌/노줌 감도 설정
	SetLookRates();

	// 십자선 퍼짐 정도 계산
	CalculateCrosshairSpread(DeltaTime);

	// Overlap 된 아이템 확인, 해당 아이템 트레이스
	TraceForItems();

	// 캡슐 높이 보간
	InterpCapsuleHalfHeight(DeltaTime);

}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind Action
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacterBase::Jump);

	PlayerInputComponent->BindAction("FireButton", EInputEvent::IE_Pressed, this, &ACharacterBase::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", EInputEvent::IE_Released, this, &ACharacterBase::FireButtonReleased);

	PlayerInputComponent->BindAction("AimingButton", EInputEvent::IE_Pressed, this, &ACharacterBase::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", EInputEvent::IE_Released, this, &ACharacterBase::AimingButtonReleased);

	PlayerInputComponent->BindAction("Select", EInputEvent::IE_Pressed, this, &ACharacterBase::SelectButtonPressed);
	PlayerInputComponent->BindAction("Select", EInputEvent::IE_Released, this, &ACharacterBase::SelectButtonReleased);

	PlayerInputComponent->BindAction("ReloadButton", EInputEvent::IE_Released, this, &ACharacterBase::ReloadButtonPressed);
	
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &ACharacterBase::CrouchButtonPressed);

	PlayerInputComponent->BindAction("1Key", EInputEvent::IE_Pressed, this, &ACharacterBase::OneKeyPressed);
	PlayerInputComponent->BindAction("2Key", EInputEvent::IE_Pressed, this, &ACharacterBase::TwoKeyPressed);

	PlayerInputComponent->BindAction("Ability_Q", EInputEvent::IE_Pressed, this, &ACharacterBase::Ability_Q_Ready);
	PlayerInputComponent->BindAction("Ability_Q", EInputEvent::IE_Released, this, &ACharacterBase::Ability_Q);
	
	// Bind Axis
	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::MoveRight);

	PlayerInputComponent->BindAxis("TurnAtRate", this, &ACharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACharacterBase::LookUpAtRate);

	PlayerInputComponent->BindAxis("Turn", this, &ACharacterBase::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ACharacterBase::LookUp);
}

float ACharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UGameplayStatics::PlaySound2D(this, MeleeImpactVoice);

	if (Health - DamageAmount <= 0.0f)
	{
		Health = 0.0f;

		auto EnemyController = Cast<AEnemyController>(EventInstigator);
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName(TEXT("CharacterDead")), true);

		Die();
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;

}

void ACharacterBase::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && bCanMove)
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void ACharacterBase::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && bCanMove)
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}
}

void ACharacterBase::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACharacterBase::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACharacterBase::Turn(float Value)
{
	float TurnScaleFactor{};
	if (bAiming)
	{
		TurnScaleFactor = MouseAimingTurnRate;
	}
	else
	{
		TurnScaleFactor = MouseHipTurnRate;
	}
	AddControllerYawInput(Value * TurnScaleFactor);
}

void ACharacterBase::LookUp(float Value)
{
	float LookUpScaleFactor{};
	if (bAiming)
	{
		LookUpScaleFactor = MouseAimingLookUpRate;
	}
	else
	{
		LookUpScaleFactor = MouseHipLookUpRate;
	}
	AddControllerPitchInput(Value * LookUpScaleFactor);
}

void ACharacterBase::Aim()
{
	bAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
}

void ACharacterBase::StopAiming()
{
	bAiming = false;
	if (!bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
}

void ACharacterBase::FireWeapon()
{
	if (EquippedWeapon == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (WeaponHasAmmo())
	{
		PlayFireSound();
		SendBullet();
		PlayHipFireMontage();
		EquippedWeapon->DecrementAmmo();
	
		StartFireTimer();

		// 발사 시 십자선 반동 효과
		StartCrosshairBulletFire();

		if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol)
		{
			EquippedWeapon->StartSlideTimer();
		}
	}
}

bool ACharacterBase::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult)
{
	FVector OutBeamLocation;
	FHitResult CrosshairHitResult;

	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);

	if (bCrosshairHit)
	{
		OutBeamLocation = CrosshairHitResult.Location;
	}

	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector WeaponTraceEnd{ OutBeamLocation };

	GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility);

	if (!OutHitResult.bBlockingHit)
	{
		OutHitResult.Location = OutBeamLocation;
		return false;
	}

	return true;
}

void ACharacterBase::AimingButtonPressed()
{
	bAimingButtonPressed = true;
	if (CombatState != ECombatState::ECS_Reloading)
	{
		Aim();
	}
}

void ACharacterBase::AimingButtonReleased()
{
	bAimingButtonPressed = false;
	StopAiming();
}

void ACharacterBase::CameraInterpZoom(float DeltaTime)
{
	// 카메라 Fov 설정
	if (bAiming)
	{
		// 줌 Fov 보간
		CameraCurrentFov = FMath::FInterpTo(CameraCurrentFov, CameraZoomedFov, DeltaTime, ZoomInterpSpeed);
	}
	else
	{
		// 줌 해제 Fov 보간
		CameraCurrentFov = FMath::FInterpTo(CameraCurrentFov, CameraDefaultFov, DeltaTime, ZoomInterpSpeed);
	}
	GetCamera()->SetFieldOfView(CameraCurrentFov);
}

void ACharacterBase::SetLookRates()
{
	if (bAiming)
	{
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AImingLookUpRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}

void ACharacterBase::Die()
{
	bDead = true; 

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	GetWorldTimerManager().SetTimer(
		DestroyTimer,
		this,
		&ACharacterBase::Destroy,
		DestroyTime);

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		DisableInput(PC);
	}
}

void ACharacterBase::Destroy()
{
	Super::Destroy();
}

void ACharacterBase::Ability_Q_Ready()
{
	bAbility_Q_Ready = true;
}

void ACharacterBase::Ability_Q()
{
	bAbility_Q_Ready = false;
	bCanMove = false;

	SetCanMove(1.0f);
}

void ACharacterBase::Ability_R()
{
}

void ACharacterBase::PlayFireSound()
{
	// 발포음 재생
	if (EquippedWeapon->GetFireSound())
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->GetFireSound());
	}
}

void ACharacterBase::SendBullet()
{
	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");

	if (BarrelSocket)
	{
		FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());

		// 총구 이펙트
		if (EquippedWeapon->GetMuzzleFlash())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquippedWeapon->GetMuzzleFlash(), SocketTransform);
		}

		// 총알 충돌 확인
		FHitResult BeamHitResult;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamHitResult);

		if (bBeamEnd)
		{
			// 총알 충돌 지점 Particle
			if (BeamHitResult.Actor.IsValid())
			{
				IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(BeamHitResult.Actor);

				if (BulletHitInterface)
				{
					BulletHitInterface->BulletHit_Implementation(BeamHitResult);
				}

				AEnemy* HitEnemy = Cast<AEnemy>(BeamHitResult.Actor);
				if (HitEnemy)
				{
					int32 Damage{};

					// 헤드샷
					if (BeamHitResult.BoneName.ToString() == HitEnemy->GetHeadBone())
					{
						Damage = EquippedWeapon->GetHeadShotDamage();
						UGameplayStatics::ApplyDamage(
							HitEnemy,
							Damage,
							GetController(),
							this,
							UDamageType::StaticClass());
					}
					// 몸샷
					else
					{
						Damage = EquippedWeapon->GetDamage();
						UGameplayStatics::ApplyDamage(
							HitEnemy,
							Damage,
							GetController(),
							this,
							UDamageType::StaticClass());
					}

				}
			}
			else
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					BeamHitResult.Location);
			}


			UParticleSystemComponent* Beam =
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform);

			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamHitResult.Location);
			}
		}
	}
}

void ACharacterBase::PlayHipFireMontage()
{
	// 총 반동 애니메이션 몽타주 재생
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName(TEXT("StartFire")));
	}
}

void ACharacterBase::ReloadButtonPressed()
{
	ReloadWeapon();
}

void ACharacterBase::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	
	if (EquippedWeapon == nullptr) return;

	if (CarryingAmmo() && !EquippedWeapon->ClipIsFull())
	{
		if (bAiming)
		{
			StopAiming();
		}

		CombatState = ECombatState::ECS_Reloading;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && ReloadMontage)
		{
			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSection());
		}
	}
}

void ACharacterBase::FinishReloading()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (bAimingButtonPressed)
	{
		Aim();
	}

	if (EquippedWeapon == nullptr) return;

	const EAmmoType AmmoType{ EquippedWeapon->GetAmmoType() };
	if (AmmoMap.Contains(AmmoType))
	{
		int32 CarriedAmmo = AmmoMap[AmmoType];
		const int32 MagEmptySpace = EquippedWeapon->GetMagazineCapacity() - EquippedWeapon->GetAmmo();

		if (MagEmptySpace > CarriedAmmo)
		{
			EquippedWeapon->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
		else
		{
			EquippedWeapon->ReloadAmmo(MagEmptySpace);
			CarriedAmmo -= MagEmptySpace;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
	}
}

void ACharacterBase::FinishEquipping()
{
	CombatState = ECombatState::ECS_Unoccupied;
	if (bAimingButtonPressed)
	{
		Aim();
	}
}

bool ACharacterBase::CarryingAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	auto AmmoType = EquippedWeapon->GetAmmoType();

	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}

	return false;
}

void ACharacterBase::GrabClip()
{
	if (EquippedWeapon == nullptr) return;
	if (HandSceneComponent == nullptr) return;

	int32 ClipBoneIndex{ EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetClipBoneName()) };
	ClipTransform = EquippedWeapon->GetItemMesh()->GetBoneTransform(ClipBoneIndex);

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("Hand_L")));
	HandSceneComponent->SetWorldTransform(ClipTransform);

	EquippedWeapon->SetMovingClip(true);
}

void ACharacterBase::ReleaseClip()
{
	EquippedWeapon->SetMovingClip(false);
}

void ACharacterBase::PickupAmmo(AAmmo* Ammo)
{
	EAmmoType AmmoType = Ammo->GetAmmoType();

	if (AmmoMap.Find(AmmoType))
	{
		int32 AmmoCount{ AmmoMap[AmmoType] };
		AmmoCount += Ammo->GetItemCount();
		AmmoMap[AmmoType] = AmmoCount;
	}

	if (EquippedWeapon->GetAmmoType() == AmmoType)
	{
		if (EquippedWeapon->GetAmmo() == 0)
		{
			ReloadWeapon();
		}
	}

	Ammo->Destroy();
}

void ACharacterBase::CrouchButtonPressed()
{
	GEngine->AddOnScreenDebugMessage(1, -1, FColor::Green, TEXT("Crouch"));

	if (!GetCharacterMovement()->IsFalling())
	{
		bCrouching = !bCrouching;
	}

	if (bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
		GetCharacterMovement()->GroundFriction = CrouchingGroundFriction;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
		GetCharacterMovement()->GroundFriction = BaseGroundFriction;
	}
}

void ACharacterBase::SetCanMove(float Time, bool Loop, float firstDelay)
{
	GetWorld()->GetTimerManager().SetTimer(CanMoveTimer, this, &ACharacterBase::CanMove, Time);
}

void ACharacterBase::OneKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 0) return;
	
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 0);	
}

void ACharacterBase::TwoKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 1) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 1);
}

void ACharacterBase::ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex)
{
	if ((CurrentItemIndex == NewItemIndex) || (NewItemIndex >= Inventory.Num()) || (CombatState == ECombatState::ECS_Reloading)) return;

	if (bAiming)
	{
		StopAiming();
	}

	auto OldEquippedWeapon = EquippedWeapon;
	auto NewWeapon = Cast<AWeaponBase>(Inventory[NewItemIndex]);
	EquipWeapon(NewWeapon);

	OldEquippedWeapon->SetItemState(EItemState::EIS_PickedUp);
	NewWeapon->SetItemState(EItemState::EIS_Equipped);

	CombatState = ECombatState::ECS_Equipping;
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(FName("Equip"));
	}
}

EPhysicalSurface ACharacterBase::GetSurfaceType()
{
	FHitResult HitResult;
	const FVector Start{ GetActorLocation() };
	const FVector End{ Start + FVector(0.0f, 0.0f, -400.0f) };
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility,
		QueryParams);

	return UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
}

void ACharacterBase::CalculateCrosshairSpread(float DeltaTime)
{
	// 십자선 고정
	FVector2D WalkSpeedRange{ 0.0f, GetMovementComponent()->GetMaxSpeed() };
	FVector2D VelocityMultiplierRange{ 0.0f, 1.0f };
	FVector Velocity{ GetVelocity() };

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	// 공중에서의 십자선 퍼짐 정도 계산(점프했을 때 더 벌어짐)
	if (GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	// 땅에서의 십자선 퍼짐 정도 계산
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.0f, DeltaTime, 30.0f);
	}

	// 조준 시 십자선 퍼짐 정도 계산
	if (bAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.5f, DeltaTime, 30.0f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.0f, DeltaTime, 30.0f);
	}

	if (bFiringBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 60.0f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.0f, DeltaTime, 60.0f);
	}

	CrosshairSpreadMultiplier =
		0.5f +
		CrosshairVelocityFactor +
		CrosshairInAirFactor -
		CrosshairAimFactor +
		CrosshairShootingFactor;
}

float ACharacterBase::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void ACharacterBase::StartCrosshairBulletFire()
{
	bFiringBullet = true;

	GetWorldTimerManager().SetTimer(
		CrosshairShootTimer,
		this,
		&ACharacterBase::FinishCrosshairBulletFire,
		ShootTimeDuration);
}

void ACharacterBase::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

void ACharacterBase::FireButtonPressed()
{
	bFireButtonPressed = true;
	FireWeapon();
}

void ACharacterBase::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void ACharacterBase::StartFireTimer()
{
	if (EquippedWeapon == nullptr) return;

	CombatState = ECombatState::ECS_FireTimerInProgress;

	GetWorldTimerManager().SetTimer(
		AutoFireTimer, 
		this, 
		&ACharacterBase::AutoFireReset, 
		EquippedWeapon->GetAutoFireRate());

}

void ACharacterBase::AutoFireReset()
{
	CombatState = ECombatState::ECS_Unoccupied;
	if (EquippedWeapon == nullptr) return;

	// 총알이 있으면 발사, 없으면 재장전
	if (WeaponHasAmmo())
	{
		if (bFireButtonPressed)
		{
			if (bFireButtonPressed && EquippedWeapon->GetAutomatic())
			{
				FireWeapon();
			}
		}
	}
	else
	{
		ReloadWeapon();
	}
}

bool ACharacterBase::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	// 뷰포트 크기
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// 십자선의 월드 좌표
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection * 50'000.f };
		OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);
		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}
	return false;
}

void ACharacterBase::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;

	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}

void ACharacterBase::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		FHitResult ItemTraceResult;
		FVector HitLocation;
		
		TraceUnderCrosshairs(ItemTraceResult, HitLocation);

		if (ItemTraceResult.bBlockingHit)
		{
			TraceHitItem = Cast<AItemBase>(ItemTraceResult.Actor);
			
			if (TraceHitItem && TraceHitItem->GetItemState() == EItemState::EIS_EquipInterping)
			{
				TraceHitItem = nullptr;
			}

			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
				TraceHitItem->EnableCustomDepth();
			}

			if (TraceHitItemLastFrame)
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
					TraceHitItemLastFrame->DisableCustomDepth();
				}
			}

			// 마지막으로 위젯이 활성화 된 아이템의 레퍼런스 저장
			TraceHitItemLastFrame = TraceHitItem;
		}
	}
	else if (TraceHitItemLastFrame)
	{
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
		TraceHitItemLastFrame->DisableCustomDepth();
	}
}

void ACharacterBase::GetPickupItem(AItemBase* Item)
{
	Item->PlayPickupSound();
	
	Item->DisableCustomDepth();
	Item->DisableGlowMaterial();

	auto Weapon = Cast<AWeaponBase>(Item);
	if (Weapon)
	{
		if (Inventory.Num() < INVENTORY_CAPACITY)
		{
			Weapon->SetSlotIndex(Inventory.Num());
			Inventory.Add(Weapon);
			Weapon->SetItemState(EItemState::EIS_PickedUp);
		}
		// 인벤토리가 다 찼을 땐 무기 교체
		else
		{
			SwapWeapon(Weapon);
		}
	}

	auto Ammo = Cast<AAmmo>(Item);
	if (Ammo)
	{
		PickupAmmo(Ammo);
	}
}

AWeaponBase* ACharacterBase::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		// 무기 스폰
		return GetWorld()->SpawnActor<AWeaponBase>(DefaultWeaponClass);
	}

	return nullptr;
}

void ACharacterBase::EquipWeapon(AWeaponBase* WeaponToEquip, bool bSwapping)
{
	if (WeaponToEquip)
	{
		// 부착할 소켓
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}

		if (WeaponToEquip->GetEquipSound())
		{
			UGameplayStatics::PlaySound2D(this, WeaponToEquip->GetEquipSound());
		}

		if (EquippedWeapon == nullptr)
		{
			EquipItemDelegate.Broadcast(-1, WeaponToEquip->GetSlotIndex());
		}
		else if(!bSwapping)
		{
			EquipItemDelegate.Broadcast(EquippedWeapon->GetSlotIndex(), WeaponToEquip->GetSlotIndex());
		}

		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

void ACharacterBase::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowItem();

		EquippedWeapon->EnableGlowMaterial();
	}
}

void ACharacterBase::SelectButtonPressed()
{
	if (TraceHitItem)
	{
		GetPickupItem(TraceHitItem);
		TraceHitItem = nullptr;
	}
}

void ACharacterBase::SelectButtonReleased()
{
}

void ACharacterBase::SwapWeapon(AWeaponBase* WeaponToSwap)
{
	if (Inventory.Num() - 1 >= EquippedWeapon->GetSlotIndex())
	{
		Inventory[EquippedWeapon->GetSlotIndex()] = WeaponToSwap;
		WeaponToSwap->SetSlotIndex(EquippedWeapon->GetSlotIndex());
	}

	DropWeapon();
	EquipWeapon(WeaponToSwap, true);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

void ACharacterBase::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9mm, Starting9mmAmmo);
	AmmoMap.Add(EAmmoType::EAT_AR, StartingARAmmo);
}

bool ACharacterBase::WeaponHasAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	return EquippedWeapon->GetAmmo() > 0;
}