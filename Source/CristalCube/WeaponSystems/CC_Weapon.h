// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../CristalCubeStruct.h"
#include "CC_Weapon.generated.h"

UCLASS()
class CRISTALCUBE_API ACC_Weapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACC_Weapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	//==============================================================================
// WEAPON PROPERTIES
//==============================================================================

	// Base weapon data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	FCristalCubeWeaponStats BaseStats;

	// Current state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon State")
	bool bCanAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon State")
	float LastAttackTime;

	// Owner reference
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon State")
	AActor* WeaponOwner;

	//==============================================================================
	// VISUAL/AUDIO COMPONENTS
	//==============================================================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UNiagaraSystem* AttackEffect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class USoundBase* AttackSound;

public:
	//==============================================================================
	// WEAPON INTERFACE
	//==============================================================================

	// Main attack function (to be overridden by child classes)
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Attack();

	// Check if weapon can attack
	UFUNCTION(BlueprintPure, Category = "Weapon")
	virtual bool CanAttack() const;

	// Weapon equip/unequip
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void OnEquipped(AActor* NewOwner);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void OnUnequipped();

	// Get weapon stats
	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetBaseDamage() const { return BaseStats.BaseDamage; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetAttackSpeed() const { return BaseStats.AttackSpeed; }


protected:
	//==============================================================================
	// INTERNAL FUNCTIONS
	//==============================================================================

	// Reset attack cooldown
	UFUNCTION()
	void ResetCooldown();

	// Play attack effects
	virtual void PlayAttackEffects();

	// Calculate final damage (base damage + owner stats)
	virtual float CalculateFinalDamage() const;
};
