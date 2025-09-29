// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CC_Character.h"
#include "../CristalCubeStruct.h"
#include "../WeaponSystems/CC_Weapon.h"
#include "CC_PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class CRISTALCUBE_API ACC_PlayerCharacter : public ACC_Character
{
	GENERATED_BODY()
	
public:
	ACC_PlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;


protected:
	//==============================================================================
	// INPUT SYSTEM
	//==============================================================================

	// Enhanced Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* PrimaryAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* SecondaryAttackAction;

	// Input Mapping Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* DefaultMappingContext;


	//==============================================================================
	// ATTACK FUNCTIONS
	//==============================================================================

	// Called for attack input
	void PrimaryAttack();
	void SecondaryAttack();

	//==============================================================================
	// CHARACTER STATS SYSTEM
	//==============================================================================

	// Player's stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FCristalCubePlayerStats PlayerStats;

	// Apply stats to character (called when stats change)
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void ApplyPlayerStats();

	// Getters
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FCristalCubePlayerStats GetPlayerStats() const { return PlayerStats; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetFinalDamageMultiplier() const { return PlayerStats.BasicStats.DamageMultiplier; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetFinalAttackSpeedMultiplier() const { return PlayerStats.BasicStats.AttackSpeedMultiplier; }

	//==============================================================================
// WEAPON SYSTEM
//==============================================================================

// Current equipped weapons
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class ACC_Weapon* PrimaryWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	ACC_Weapon* SecondaryWeapon;

	// Current active weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	ACC_Weapon* CurrentWeapon;

public:

	// Weapon Management
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon(ACC_Weapon* NewWeapon, bool bIsPrimary = true);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SwitchWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PerformAttack();

	// Getters
	UFUNCTION(BlueprintPure, Category = "Weapon")
	ACC_Weapon* GetCurrentWeapon() const { return CurrentWeapon; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool HasWeapon() const { return CurrentWeapon != nullptr; }

protected:

	//==============================================================================
	// LEVEL & EXPERIENCE SYSTEM
	//==============================================================================
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 Level;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	float Experience;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	float ExperienceToNextLevel;

	void LevelUp();


public:
	//==============================================================================
	// PUBLIC FUNCTIONS
	//==============================================================================

	// Add experience (called when enemy dies)
	UFUNCTION(BlueprintCallable, Category = "Level")
	void AddExperience(float ExpAmount);

	UFUNCTION(BlueprintCallable, Category = "Level")
	int32 GetPlayerLevel() const { return Level; }

	UFUNCTION(BlueprintCallable, Category = "Level")
	float GetExperience() const { return Experience; }

	UFUNCTION(BlueprintPure, Category = "Level")
	float GetExperiencePercentage() const;

	//==============================================================================
	// OVERRIDE FROM BASE CHARACTER
	//==============================================================================

protected:
	// Override ApplyStats to include player-specific stat calculations

	virtual void ApplyStats() override;
};
