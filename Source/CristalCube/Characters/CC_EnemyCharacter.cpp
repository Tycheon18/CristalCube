// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "CC_PlayerCharacter.h"

ACC_EnemyCharacter::ACC_EnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Enemy defaults
	MaxHealth = 50.0f;
	CurrentHealth = MaxHealth;
	MoveSpeed = 300.0f;  // Slower than player

	// AI settings
	DetectionRange = 2000.0f;  // 20 meters
	bChasePlayer = true;
	TargetPlayer = nullptr;

	// Combat settings
	ContactDamage = 10.0f;     // 10 damage per hit
	DamageCooldown = 1.0f;     // 1 second between hits
	LastDamageTime = 0.0f;

	// Reward settings
	ExperienceDrop = 10.0f;    // Give 10 XP when killed

	// Enemy type
	EnemyType = TEXT("Basic");
	bIsBoss = false;
}

void ACC_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add "Enemy" tag for weapon auto-aim
	Tags.AddUnique(FName("Enemy"));

	// Find player
	FindPlayer();

	// Setup overlap events for contact damage
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->OnComponentBeginOverlap.AddDynamic(this, &ACC_EnemyCharacter::OnOverlapBegin);
	}

	UE_LOG(LogTemp, Log, TEXT("Enemy spawned: %s (HP: %.0f, Damage: %.0f)"),
		*EnemyType, MaxHealth, ContactDamage);
}

void ACC_EnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Chase player if enabled and alive
	if (bChasePlayer && IsAlive() && TargetPlayer)
	{
		ChasePlayer(DeltaTime);
	}
}

void ACC_EnemyCharacter::ChasePlayer(float DeltaTime)
{
	if (!TargetPlayer)
	{
		// Try to find player again
		FindPlayer();
		return;
	}

	// Check if player is within detection range
	float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());

	if (DistanceToPlayer > DetectionRange)
	{
		// Player too far, don't chase
		return;
	}

	// Calculate direction to player
	FVector DirectionToPlayer = (TargetPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	// Move towards player
	AddMovementInput(DirectionToPlayer, 1.0f);

	// Rotate to face player
	FRotator LookAtRotation = DirectionToPlayer.Rotation();
	LookAtRotation.Pitch = 0.0f;  // Keep on ground
	LookAtRotation.Roll = 0.0f;

	SetActorRotation(FMath::RInterpTo(GetActorRotation(), LookAtRotation, DeltaTime, 5.0f));

}

void ACC_EnemyCharacter::FindPlayer()
{
	// Find player character in the world
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	TargetPlayer = Cast<ACC_PlayerCharacter>(PlayerPawn);

	if (TargetPlayer)
	{
		UE_LOG(LogTemp, Log, TEXT("Enemy found player: %s"), *TargetPlayer->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy could not find player"));
	}
}

bool ACC_EnemyCharacter::CanDealDamage() const
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	return (CurrentTime - LastDamageTime) >= DamageCooldown;
}

void ACC_EnemyCharacter::DealContactDamage(AActor* OtherActor)
{
	if (!OtherActor || !CanDealDamage())
	{
		return;
	}

	// Check if it's the player
	ACC_PlayerCharacter* Player = Cast<ACC_PlayerCharacter>(OtherActor);
	if (!Player)
	{
		return;
	}

	// Deal damage to player
	UGameplayStatics::ApplyDamage(
		Player,
		ContactDamage,
		GetController(),
		this,
		UDamageType::StaticClass()
	);

	// Update cooldown
	LastDamageTime = GetWorld()->GetTimeSeconds();

	UE_LOG(LogTemp, Log, TEXT("Enemy dealt %.0f contact damage to player"), ContactDamage);

}

void ACC_EnemyCharacter::Die()
{
	// Drop experience for player
	if (TargetPlayer && ExperienceDrop > 0.0f)
	{
		TargetPlayer->AddExperience(ExperienceDrop);
		UE_LOG(LogTemp, Log, TEXT("Enemy dropped %.0f experience"), ExperienceDrop);
	}

	// Call base class Die() to handle death animation, etc.
	Super::Die();

	// TODO: Spawn death effect, drop items, etc.

	// Destroy enemy after short delay
	SetLifeSpan(1.0f);
}

void ACC_EnemyCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DealContactDamage(OtherActor);
}
