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

	// Character�� ȸ���� Controller�� ������ ���� ����
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	// ĳ���� �����Ʈ
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 340.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 600.0f;

	// SpringArm �ʱ�ȭ
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 180.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SocketOffset = FVector(0.0f, 50.0f, 100.0f);

	// Camera �ʱ�ȭ
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;	// ī�޶� ȸ���� ���Ƴ��´�.

	// ȸ����
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;

	// ���ذ� ���/�̻�� Fov
	bAiming = false;
	CameraDefaultFov = 0.0f; // BeginPlay���� �� ����
	CameraZoomedFov = 35.0f;
	CameraCurrentFov = 0.0f;
	ZoomInterpSpeed = 20.0f;

	// ���ذ� ���/�̻�� ����
	HipTurnRate = 90.0f;
	HipLookUpRate = 90.0f;
	AimingTurnRate = 20.0f;
	AImingLookUpRate = 20.0f;

	// ���콺 ����
	MouseHipTurnRate = 1.0f;
	MouseHipLookUpRate = 1.0f;
	MouseAimingTurnRate = 0.2f;
	MouseAimingLookUpRate = 0.2f;

	// ���ڼ�
	CrosshairSpreadMultiplier = 0.0f;
	CrosshairVelocityFactor = 0.0f;
	CrosshairInAirFactor = 0.0f;
	CrosshairAimFactor = 0.0f;
	CrosshairShootingFactor = 0.0f;
	bFiringBullet = false;
	ShootTimeDuration = 0.05f;

	// �ڵ� �߻�
	bFireButtonPressed = false;
	bShouldFire = true;
	AutomaticFireRate = 0.1f;

	// ������ Ʈ���̽�
	bShouldTraceForItems = false;
	OverlappedItemCount = 0;

	// �Ѿ�
	Starting9mmAmmo = 85;
	StartingARAmmo = 120;

	// ����
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

	// �⺻ ���� ���� �� ���Ͽ� ����
	EquipWeapon(SpawnDefaultWeapon());

	// �Ѿ� �ʱ�ȭ
	InitializeAmmoMap();
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �� ����
	CameraInterpZoom(DeltaTime);

	// ��/���� ���� ����
	SetLookRates();

	// ���ڼ� ���� ���� ���
	CalculateCrosshairSpread(DeltaTime);

	// Overlap �� ������ Ȯ��, �ش� ������ Ʈ���̽�
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

		// �߻� �� ���ڼ� �ݵ� ȿ��
		StartCrosshairBulletFire();
	}
}

bool ACharacterBase::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
{
	FHitResult CrosshairHitResult;

	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);

	// ���� Ʈ���̽� �浹 �� �ش� ��ġ ����
	if (bCrosshairHit)
	{
		OutBeamLocation = CrosshairHitResult.Location;
	}

	// ���� Ʈ���̽��� �浹���� �ʾ��� �� �ι�° �浹 �˻�(�ѱ� - ��ǥ��)
	FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ OutBeamLocation - MuzzleSocketLocation };
	const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f };

	GetWorld()->LineTraceSingleByChannel(
		WeaponTraceHit,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility);

	// �浹ü�� �ѱ��� ��ǥ ���� ���̿� �ִٸ�
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
	// ī�޶� Fov ����
	if (bAiming)
	{
		// �� Fov ����
		CameraCurrentFov = FMath::FInterpTo(CameraCurrentFov, CameraZoomedFov, DeltaTime, ZoomInterpSpeed);
	}
	else
	{
		// �� ���� Fov ����
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
	// ������ ���
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

		// �ѱ� ����Ʈ
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		// �Ѿ� �浹 Ȯ��
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
	// �� �ݵ� �ִϸ��̼� ��Ÿ�� ���
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

	// ź�� Ÿ�� ��Ī�� ź���� �ִ���?? �߰� ����
	// �������� �߰� ���� ��� ���⵵ �پ��ϰ� �߰� ����
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
	// ������Ʈ AmmoMap �߰� ����
	CombatState = ECombatState::ECS_Unoccupied;
}

void ACharacterBase::CalculateCrosshairSpread(float DeltaTime)
{
	// ���ڼ� ����
	FVector2D WalkSpeedRange{ 0.0f, GetMovementComponent()->GetMaxSpeed() };
	FVector2D VelocityMultiplierRange{ 0.0f, 1.0f };
	FVector Velocity{ GetVelocity() };

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	// ���߿����� ���ڼ� ���� ���� ���(�������� �� �� ������)
	if (GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	// �������� ���ڼ� ���� ���� ���
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.0f, DeltaTime, 30.0f);
	}

	// ���� �� ���ڼ� ���� ���� ���
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

	// �Ѿ��� ������ �߻�, ������ ������
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
	// ���� ����Ʈ ������
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// ���忡���� ���ڼ� ��ġ, ����
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
		// ���� - ������ Ʈ���̽� �浹 üũ
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

			// ���� Ʈ���̽��� �浹�ϸ� �ش� ������ ���� Ȱ��ȭ
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

			// ���������� ������ Ȱ��ȭ �� �������� ���۷��� ����
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
		// ���� ����
		return GetWorld()->SpawnActor<AWeaponBase>(DefaultWeaponClass);
	}

	return nullptr;
}

void ACharacterBase::EquipWeapon(AWeaponBase* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		// ������ ����
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
