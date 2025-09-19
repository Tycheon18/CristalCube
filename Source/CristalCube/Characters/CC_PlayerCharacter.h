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

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
	// WEAPON SYSTEM
	//==============================================================================

	// Current equipped weapons
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class ACC_Weapon* PrimaryWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class ACC_Weapon* SecondaryWeapon;

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
	FCristalCubeCharacterStats PlayerStats;

	// Apply stats to character (called when stats change)
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void ApplyStats();

	//==============================================================================
	// LEVELING SYSTEM
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

	// Getters
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FCristalCubeCharacterStats GetPlayerStats() const { return PlayerStats; }

	UFUNCTION(BlueprintCallable, Category = "Level")
	int32 GetPlayerLevel() const { return Level; }

	UFUNCTION(BlueprintCallable, Category = "Level")
	float GetExperience() const { return Experience; }

};
