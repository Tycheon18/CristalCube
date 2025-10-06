// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "NiagaraFunctionLibrary.h"
#include "CC_Projectile.h"


// Sets default values
ACC_Weapon::ACC_Weapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Initialize base weapon stats
	BaseStats.WeaponCategory = EWeaponCategory::None;
	BaseStats.BaseDamage = 10.0f;         // 10 damage per hit
	BaseStats.AttackSpeed = 1.0f;         // 1 attack per second

	// Initialize weapon state
	bCanAttack = true;                    // Ready to attack
	LastAttackTime = 0.0f;                // No previous attack
	WeaponOwner = nullptr;                // No owner yet

	// Initialize effects (set in Blueprint or child classes)
	AttackEffect = nullptr;
	AttackSound = nullptr;

}

// Called when the game starts or when spawned
void ACC_Weapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACC_Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACC_Weapon::Attack()
{
	if (!CanAttack())
	{
		return;
	}

	// Update attack state
	LastAttackTime = GetWorld()->GetTimeSeconds();
	bCanAttack = false;

	// Play visual and audio feedback
	PlayAttackEffects();

	// Schedule cooldown reset based on attack speed
	float CooldownDuration = 1.0f / BaseStats.AttackSpeed;
	GetWorld()->GetTimerManager().SetTimer(
		AttackCooldownTimer,
		this,
		&ACC_Weapon::ResetCooldown,
		CooldownDuration,
		false
	);

	// IMPORTANT: Override this function in child classes for specific weapon behavior
	// Example: Spawn projectile, perform melee sweep, etc.
	UE_LOG(LogTemp, Warning, TEXT("Base Weapon Attack - Override in child class!"));

}

bool ACC_Weapon::CanAttack() const
{
	// Implement logic to determine if the weapon can attack
	if (!bCanAttack || !WeaponOwner)
	{
		return false;
	}

	// Verify cooldown has expired
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float CooldownDuration = 1.0f / BaseStats.AttackSpeed;
	return (CurrentTime - LastAttackTime) >= CooldownDuration;
}

void ACC_Weapon::OnEquipped(AActor* NewOwner)
{
	WeaponOwner = NewOwner;

	if (WeaponOwner)
	{
		AttachToActor(WeaponOwner, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		UE_LOG(LogTemp, Log, TEXT("Weapon equipped and attached to: %s"), *WeaponOwner->GetName());
	}
}

void ACC_Weapon::OnUnequipped()
{
	WeaponOwner = nullptr;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	UE_LOG(LogTemp, Log, TEXT("Weapon Uneauipped"));
}

void ACC_Weapon::PerformRangedAttack()
{
	if (!WeaponOwner || !ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("RangedAttack: Missing owner or projectile class"));
		return;
	}

	// Calculate spawn location (75cm in front of owner by default)
	FVector SpawnOffset = FVector(75.0f, 0.0f, 0.0f);
	FVector SpawnLocation = WeaponOwner->GetActorLocation() +
		WeaponOwner->GetActorRotation().RotateVector(SpawnOffset);

	// Get firing direction
	FVector FiringDirection = GetFiringDirection();
	FRotator BaseRotation = FiringDirection.Rotation();

	// Get final projectile count (base + upgrades)
	int32 FinalCount = GetFinalProjectileCount();

	// Calculate spread angle (inverse of accuracy)
	float SpreadAngle = (1.0f - RangedStats.Accuracy) * 30.0f;  // Max 30 degree spread

	// Spawn projectile(s)
	if (FinalCount > 1)
	{
		SpawnMultipleProjectiles(SpawnLocation, BaseRotation, FinalCount);
	}
	else
	{
		// Apply accuracy-based spread even for single projectile
		if (SpreadAngle > 0.0f)
		{
			float RandomYaw = FMath::RandRange(-SpreadAngle, SpreadAngle);
			BaseRotation.Yaw += RandomYaw;
		}
		SpawnProjectile(SpawnLocation, BaseRotation);
	}
}

void ACC_Weapon::PerformMeleeAttack()
{
	UE_LOG(LogTemp, Log, TEXT("Melee Attack - Range: %.1f, Angle: %.1f"),
		MeleeStats.AttackRange, MeleeStats.SwingAngle);
}

void ACC_Weapon::PerformMagicAttack()
{
	UE_LOG(LogTemp, Log, TEXT("Magic Attack - Radius: %.1f, Cast Time: %.1f"),
		MagicStats.EffectRadius, MagicStats.CastTime);
}

void ACC_Weapon::ResetCooldown()
{
	bCanAttack = true;
}

void ACC_Weapon::PlayAttackEffects()
{
	// Play Niagara particle effect at weapon location
	if (AttackEffect && WeaponOwner)
	{
		FVector EffectLocation = WeaponOwner->GetActorLocation();
		FRotator EffectRotation = WeaponOwner->GetActorRotation();

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			AttackEffect,
			EffectLocation,
			EffectRotation
		);
	}

	// Play attack sound at weapon location
	if (AttackSound && WeaponOwner)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			AttackSound,
			WeaponOwner->GetActorLocation()
		);
	}
}

float ACC_Weapon::CalculateFinalDamage() const
{
	float FinalDamage = BaseStats.BaseDamage;

	return FinalDamage;
}

int32 ACC_Weapon::GetFinalProjectileCount() const
{
	int32 FinalCount = RangedStats.ProjectileCount;

	return FinalCount;
}

void ACC_Weapon::SpawnProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if (!GetWorld() || !ProjectileClass)
	{
		return;
	}

	// Spawn parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = WeaponOwner;
	SpawnParams.Instigator = Cast<APawn>(WeaponOwner);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn projectile
	ACC_Projectile* Projectile = GetWorld()->SpawnActor<ACC_Projectile>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	if (Projectile)
	{
		// Initialize projectile with weapon's damage and speed
		float FinalDamage = CalculateFinalDamage();
		Projectile->InitializeProjectile(FinalDamage, RangedStats.ProjectileSpeed);
		Projectile->SetProjectileOwner(WeaponOwner);

		UE_LOG(LogTemp, Log, TEXT("Spawned projectile: Damage=%.1f, Speed=%.1f"),
			FinalDamage, RangedStats.ProjectileSpeed);
	}
}

void ACC_Weapon::SpawnMultipleProjectiles(const FVector& SpawnLocation, const FRotator& BaseRotation, int32 Count)
{
	// Calculate spread angle based on accuracy
	float SpreadAngle = (1.0f - RangedStats.Accuracy) * 30.0f;  // Max 30 degree spread

	// Calculate spread rotations for all projectiles
	TArray<FRotator> SpreadRotations = CalculateSpreadRotations(BaseRotation, Count, SpreadAngle);

	// Spawn each projectile
	for (const FRotator& Rotation : SpreadRotations)
	{
		SpawnProjectile(SpawnLocation, Rotation);
	}
}

FVector ACC_Weapon::GetFiringDirection() const
{
	if (!WeaponOwner)
	{
		return FVector::ForwardVector;
	}

	// If auto-aim is enabled, find nearest enemy
	if (bAutoAim)
	{
		AActor* NearestEnemy = FindNearestEnemy();
		if (NearestEnemy)
		{
			// Aim towards nearest enemy
			FVector Direction = (NearestEnemy->GetActorLocation() - WeaponOwner->GetActorLocation()).GetSafeNormal();
			return Direction;
		}
	}

	// Default: fire in the direction owner is facing
	return WeaponOwner->GetActorForwardVector();
}

AActor* ACC_Weapon::FindNearestEnemy() const
{
	if (!WeaponOwner || !GetWorld())
	{
		return nullptr;
	}

	// Find all actors with tag "Enemy" within radius
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), FoundActors);

	AActor* NearestEnemy = nullptr;
	float NearestDistance = AutoAimRadius;

	FVector OwnerLocation = WeaponOwner->GetActorLocation();

	// Find the closest enemy
	for (AActor* Actor : FoundActors)
	{
		if (!Actor || Actor == WeaponOwner)
		{
			continue;
		}

		float Distance = FVector::Dist(OwnerLocation, Actor->GetActorLocation());

		if (Distance < NearestDistance)
		{
			NearestDistance = Distance;
			NearestEnemy = Actor;
		}
	}

	// Debug visualization (optional, disabled in shipping builds)
#if !UE_BUILD_SHIPPING
	if (NearestEnemy)
	{
		DrawDebugLine(
			GetWorld(),
			OwnerLocation,
			NearestEnemy->GetActorLocation(),
			FColor::Red,
			false,
			0.1f,
			0,
			2.0f
		);
	}
#endif

	return NearestEnemy;
}

TArray<FRotator> ACC_Weapon::CalculateSpreadRotations(const FRotator& BaseRotation, int32 Count, float Spread) const
{
	TArray<FRotator> Rotations;

	if (Count <= 0)
	{
		return Rotations;
	}

	if (Count == 1)
	{
		// Single projectile - no spread
		Rotations.Add(BaseRotation);
		return Rotations;
	}

	// Calculate spread for multiple projectiles
	float HalfSpread = Spread * 0.5f;
	float AngleStep = (Count > 1) ? Spread / (Count - 1) : 0.0f;

	for (int32 i = 0; i < Count; i++)
	{
		// Calculate angle offset from center
		float AngleOffset = -HalfSpread + (AngleStep * i);

		// Apply yaw rotation (horizontal spread)
		FRotator SpreadRotation = BaseRotation;
		SpreadRotation.Yaw += AngleOffset;

		Rotations.Add(SpreadRotation);
	}

	return Rotations;
}
