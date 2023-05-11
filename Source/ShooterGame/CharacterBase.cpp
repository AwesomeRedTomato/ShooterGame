#include "CharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"

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
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 340.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 600.0f;

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

	// 조준경 사용/미사용 Fov
	bAiming = false;
	CameraDefaultFov = 0.0f; // BeginPlay에서 값 설정
	CameraZoomedFov = 35.0f;
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
	AutomaticFireRate = 0.1f;

	// 아이템 트레이스
	bShouldTraceForItems = false;
	OverlappedItemCount = 0;

	// 총알
	Starting9mmAmmo = 85;
	StartingARAmmo = 120;

	// 전투
	CombatState = ECombatState::ECS_Unoccupied;
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (Camera)
	{
		CameraDefaultFov = GetCamera()->FieldOfView;
		CameraCurrentFov = CameraDefaultFov;
	}

	// 기본 무기 스폰 후 소켓에 장착
	EquipWeapon(SpawnDefaultWeapon());

	// 총알 초기화
	InitializeAmmoMap();
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
}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind Action
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("FireButton", EInputEvent::IE_Pressed, this, &ACharacterBase::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", EInputEvent::IE_Released, this, &ACharacterBase::FireButtonReleased);

	PlayerInputComponent->BindAction("AimingButton", EInputEvent::IE_Pressed, this, &ACharacterBase::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", EInputEvent::IE_Released, this, &ACharacterBase::AimingButtonReleased);

	PlayerInputComponent->BindAction("Select", EInputEvent::IE_Pressed, this, &ACharacterBase::SelectButtonPressed);
	PlayerInputComponent->BindAction("Select", EInputEvent::IE_Released, this, &ACharacterBase::SelectButtonReleased);

	PlayerInputComponent->BindAction("ReloadButton", EInputEvent::IE_Released, this, &ACharacterBase::ReloadButtonPressed);
	
	// Bind Axis
	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::MoveRight);

	PlayerInputComponent->BindAxis("TurnAtRate", this, &ACharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACharacterBase::LookUpAtRate);

	PlayerInputComponent->BindAxis("Turn", this, &ACharacterBase::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ACharacterBase::LookUp);
}

void ACharacterBase::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void ACharacterBase::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
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
	}
}

bool ACharacterBase::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
{
	FHitResult CrosshairHitResult;

	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);

	// 라인 트레이스 충돌 시 해당 위치 설정
	if (bCrosshairHit)
	{
		OutBeamLocation = CrosshairHitResult.Location;
	}

	// 라인 트레이스가 충돌하지 않았을 때 두번째 충돌 검사(총구 - 목표점)
	FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ OutBeamLocation - MuzzleSocketLocation };
	const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f };

	GetWorld()->LineTraceSingleByChannel(
		WeaponTraceHit,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility);

	// 충돌체가 총구와 목표 지점 사이에 있다면
	if (WeaponTraceHit.bBlockingHit)
	{
		OutBeamLocation = WeaponTraceHit.Location;
		return true;
	}
	return false;
}

void ACharacterBase::AimingButtonPressed()
{
	bAiming = true;
}

void ACharacterBase::AimingButtonReleased()
{
	bAiming = false;
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

void ACharacterBase::PlayFireSound()
{
	// 발포음 재생
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
}

void ACharacterBase::SendBullet()
{
	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");

	if (BarrelSocket)
	{
		FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());

		// 총구 이펙트
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		// 총알 충돌 확인
		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);

		if (bBeamEnd)
		{
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamEnd);
			}

			UParticleSystemComponent* Beam =
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform);

			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
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

	// 탄약 타입 매칭과 탄약이 있는지?? 추가 예정
	// 재장전도 추가 예정 고로 무기도 다양하게 추가 예정
	if (true)
	{
		FName MontageSection(TEXT("Reload SMG"));
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		
		if (AnimInstance && ReloadMontage)
		{
			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(MontageSection);
		}
	}
}

void ACharacterBase::FinishReloading()
{
	// 업데이트 AmmoMap 추가 예정
	CombatState = ECombatState::ECS_Unoccupied;
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
	if (WeaponHasAmmo())
	{
		StartFireTimer();
	}
}

void ACharacterBase::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void ACharacterBase::StartFireTimer()
{
	CombatState = ECombatState::ECS_FireTimerInProgress;

	GetWorldTimerManager().SetTimer(AutoFireTimer, this, &ACharacterBase::AutoFireReset, AutomaticFireRate);
}

void ACharacterBase::AutoFireReset()
{
	CombatState = ECombatState::ECS_Unoccupied;

	// 총알이 있으면 발사, 없으면 재장전
	if (WeaponHasAmmo())
	{
		if (bFireButtonPressed)
		{
			FireWeapon();
		}
	}
	else
	{
		ReloadWeapon();
	}
}

bool ACharacterBase::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	// 현재 뷰포트 사이즈
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// 월드에서의 십자선 위치, 방향
	FVector2D CrosshairLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		// 에임 - 아이템 트레이스 충돌 체크
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection * 50'000.0f };
		GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);

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

			// 라인 트레이스에 충돌하면 해당 아이템 위젯 활성화
			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
			}

			if (TraceHitItemLastFrame)
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
				}
			}

			// 마지막으로 위젯이 활성화 된 아이템의 레퍼런스 저장
			TraceHitItemLastFrame = TraceHitItem;
		}
	}
	else if (TraceHitItemLastFrame)
	{
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
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

void ACharacterBase::EquipWeapon(AWeaponBase* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		// 부착할 소켓
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
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
	}
}

void ACharacterBase::SelectButtonPressed()
{
	if (TraceHitItem)
	{
		AWeaponBase* TraceHitWeapon = Cast<AWeaponBase>(TraceHitItem);
		SwapWeapon(TraceHitWeapon);
	}
}

void ACharacterBase::SelectButtonReleased()
{
}

void ACharacterBase::SwapWeapon(AWeaponBase* WeaponToSwap)
{
	DropWeapon();
	EquipWeapon(WeaponToSwap);
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
