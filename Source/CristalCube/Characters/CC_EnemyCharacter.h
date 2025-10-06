// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CC_Character.h"
#include "../CristalCubeStruct.h"
#include "CC_EnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class CRISTALCUBE_API ACC_EnemyCharacter : public ACC_Character
{
	GENERATED_BODY()

public:
	ACC_EnemyCharacter();

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:

	virtual void Tick(float DeltaTime) override;

protected:

	// Target player reference
	UPROPERTY()
	class ACC_PlayerCharacter* TargetPlayer;

	// Detection range for finding player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|AI")
	float DetectionRange;

	// Should chase player?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|AI")
	bool bChasePlayer;

	// Chase the target player
	void ChasePlayer(float DeltaTime);

	// Find the player in the world
	void FindPlayer();


protected:
	//==========================================================================
// COMBAT
//==========================================================================


	// Contact damage dealt to player on collision
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Combat")
	float ContactDamage;

	// Cooldown between damage ticks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Combat")
	float DamageCooldown;

	// Last time damage was dealt
	UPROPERTY()
	float LastDamageTime;

	// Can deal damage right now?
	bool CanDealDamage() const;

	// Deal contact damage to player
	void DealContactDamage(AActor* OtherActor);

	
protected:
	
	//==========================================================================
// EXPERIENCE DROP
//==========================================================================


	// Experience points dropped when killed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Reward")
	float ExperienceDrop;

	// Override Die() to drop experience
	virtual void Die() override;


protected:

	//==========================================================================
// COLLISION
//==========================================================================


	// Called when enemy overlaps with something
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);


protected:

	//==========================================================================
// ENEMY TYPE (Future expansion)
//==========================================================================


	// Enemy type for future variety
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Type")
	FString EnemyType;

	// Is this a boss enemy?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Type")
	bool bIsBoss;

public:

	//==========================================================================
// AI Manager Interface
//==========================================================================

	UFUNCTION(BlueprintPure, Category = "Enemy|AI")
	float GetDetectionRange() const { return DetectionRange; }

	UFUNCTION(BlueprintCallable, Category = "Enemy|AI")
	void SetChasePlayer(bool bNewChasePlayer) { bChasePlayer = bNewChasePlayer; }

	UFUNCTION(BlueprintPure, Category = "Enemy|AI")
	bool GetChasePlayer() const { return bChasePlayer; }

protected:

	// Attack 
	FCristalCubeEnemyStats EnemyStats;

	// Attack cooldown timer
	FTimerHandle AttackCooldownTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bCanAttack = true;

	void PerformAttack();
	void StartAttackCooldown();
	void ResetAttackCooldown();

public:

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DealDamageToTarget();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void TryAttack(ACC_PlayerCharacter* Target);

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool CanAttack() const { return bCanAttack && !bIsAttacking && IsAlive(); }

	UFUNCTION(BlueprintPure, Category = "Combat")
	float GetAttackCooldownPercent() const;

protected:
	
	// Animation instance reference

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* AttackMontage;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsAttacking = false;

	void PlayAttackAnimation();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

};
