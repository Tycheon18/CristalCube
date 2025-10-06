// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_EnemyCharacter.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "TimerManager.h"
#include "CC_PlayerCharacter.h"
#include "../CC_LogHelper.h"
#include "../CC_AIManager.h"
#include "../CC_EnemyAIController.h"

ACC_EnemyCharacter::ACC_EnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Enemy defaults
	MaxHealth = 50.0f;
	CurrentHealth = MaxHealth;
	MoveSpeed = 300.0f;  // Slower than player

	if(UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
		Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		Capsule->SetGenerateOverlapEvents(true);

		Capsule->OnComponentBeginOverlap.AddDynamic(this, &ACC_EnemyCharacter::OnOverlapBegin);

	}

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

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Enemy type
	EnemyType = TEXT("Basic");
	bIsBoss = false;
}

void ACC_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!GetController())
	{
		CC_LOG_ENEMY(Warning, TEXT("No controller! Spawning AI Controller..."));
	}

	// Add "Enemy" tag for weapon auto-aim
	Tags.AddUnique(FName("Enemy"));

	// Find player
	FindPlayer();

	// Setup overlap events for contact damage
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->OnComponentBeginOverlap.AddDynamic(this, &ACC_EnemyCharacter::OnOverlapBegin);
	}

	CC_LOG_ENEMY(Warning, TEXT("Spawned enemy: %s (HP: %.0f, Damage: %.0f)"),
		*EnemyType, MaxHealth, ContactDamage);


	if (UCC_AIManager* AIManager = UCC_AIManager::Get(this))
	{
		AIManager->RegisterEnemy(this);
		UE_LOG(LogTemp, Log, TEXT("Enemy registered with AI Manager: %s"), *GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AI Manager not found, using fallback AI for: %s"), *GetName());
		// Fallback: 기존 방식으로 동작 (AI Manager 없어도 게임 진행 가능)
	}

	if (USkeletalMeshComponent* SkeletalMesh = GetMesh())
	{
		if (UAnimInstance* AnimInstance = SkeletalMesh->GetAnimInstance())
		{
			AnimInstance->OnMontageEnded.AddDynamic(
				this, &ACC_EnemyCharacter::OnAttackMontageEnded
			);


			UE_LOG(LogTemp, Warning, TEXT("[ENEMY] %s - Montage ended event bound"),
				*GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[ENEMY] %s - No AnimInstance found!"),
				*GetName());
		}
	}
}

void ACC_EnemyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unregister from AI Manager
	if (UCC_AIManager* AIManager = UCC_AIManager::Get(this))
	{
		AIManager->UnregisterEnemy(this);
	}

	Super::EndPlay(EndPlayReason);
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
	if (UCC_AIManager* AIManager = UCC_AIManager::Get(this))
	{
		AIManager->UnregisterEnemy(this);
	}

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
	//DealContactDamage(OtherActor);

	UE_LOG(LogTemp, Warning, TEXT("[ENEMY] %s - OnOverlapBegin with: %s"),
		*GetName(), *OtherActor->GetName());

	if(ACC_PlayerCharacter* Player = Cast<ACC_PlayerCharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("[ENEMY] %s - Detected PLAYER overlap!"),
			*GetName());

		TargetPlayer = Player;


		UE_LOG(LogTemp, Warning, TEXT("[ENEMY] %s - Calling TryAttack..."),
			*GetName());


		TryAttack(Player);


	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[ENEMY] %s - Overlap with non-player: %s"),
			*GetName(), *OtherActor->GetClass()->GetName());
	}
}

void ACC_EnemyCharacter::PerformAttack()
{
	bIsAttacking = true;
	bCanAttack = false;

	// Stop movement while attacking
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
	}

	// Ply attack animation if available
	if (AttackMontage && GetMesh())
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(AttackMontage, 1.0f);

			UE_LOG(LogTemp, Log, TEXT("[ENEMY] %s started attack animation"), *GetName());
		}
	}
	else
	{
		// if no animation, immediately deal damage and reset
		DealDamageToTarget();
		bIsAttacking = false;
		StartAttackCooldown();
	}
}

void ACC_EnemyCharacter::StartAttackCooldown()
{
	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("[ENEMY] Cannot start cooldown - no valid world"));
		return;
	}

	// Start cooldown timer
	GetWorld()->GetTimerManager().SetTimer(
		AttackCooldownTimer,
		this,
		&ACC_EnemyCharacter::ResetAttackCooldown,
		EnemyStats.AttackCooldown,
		false  // Loop = false (Only once)
	);

	UE_LOG(LogTemp, Log, TEXT("[ENEMY] %s cooldown started (%.1fs)"),
		*GetName(), EnemyStats.AttackCooldown);
}

void ACC_EnemyCharacter::ResetAttackCooldown()
{
	bCanAttack = true;

	UE_LOG(LogTemp, Log, TEXT("[ENEMY] %s attack ready!"), *GetName());

	// if in range, try to attack again
	if (TargetPlayer)
	{
		float Distance = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
		if (Distance <= EnemyStats.AttackRange)
		{
			TryAttack(TargetPlayer);
		}
	}
}

void ACC_EnemyCharacter::DealDamageToTarget()
{
	// AnimNofity "AttackHit" from animation
	if (!TargetPlayer || !IsAlive())
	{
		return;
	}

	if (ACC_PlayerCharacter* Player = Cast<ACC_PlayerCharacter>(TargetPlayer))
	{
		if(Player->IsAlive())
		{
			Player->TakeDamage(EnemyStats.AttackDamage, FDamageEvent(), nullptr, this);

			UE_LOG(LogTemp, Warning, TEXT("[ENEMY] %s dealt %.1f damage to player"),
				*GetName(), EnemyStats.AttackDamage);
		}
	}

}

void ACC_EnemyCharacter::TryAttack(ACC_PlayerCharacter* Target)
{
	if (!CanAttack() || !Target)
	{
		return;
	}

	// Check range
	float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
	if (Distance > EnemyStats.AttackRange)
	{
		return;
	}

	// Try to attack
	TargetPlayer = Target;
	PerformAttack();
}

float ACC_EnemyCharacter::GetAttackCooldownPercent() const
{
	if (bCanAttack)
	{
		return 1.0f;
	}

	if (!GetWorld())
	{
		return 0.0f;
	}

	float RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(AttackCooldownTimer);

	if (RemainingTime <= 0.0f)
	{
		return 1.0f;
	}

	return 1.0f - (RemainingTime / EnemyStats.AttackCooldown);
}

void ACC_EnemyCharacter::PlayAttackAnimation()
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(AttackMontage, 1.0f);
	}
}

void ACC_EnemyCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontage)
	{
		bIsAttacking = false;

		// Start cooldown Timer
		GetWorld()->GetTimerManager().SetTimer(
			AttackCooldownTimer,
			this,
			&ACC_EnemyCharacter::ResetAttackCooldown,
			EnemyStats.AttackCooldown,
			false
		);

		UE_LOG(LogTemp, Log, TEXT("[ENEMY] %s attack ended, cooldown started (%.1fs)"),
			*GetName(), EnemyStats.AttackCooldown);
	}

}
