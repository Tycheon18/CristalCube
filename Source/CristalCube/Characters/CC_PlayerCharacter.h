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

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;


protected:

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


	// Current active weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class ACC_Weapon* CurrentWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Weapons")
	TArray<ACC_Weapon*> EquippedWeapons;

	/** Starting weapon class (equipped on BeginPlay) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Weapons")
	TSubclassOf<ACC_Weapon> StartingWeaponClass;

	/** Maximum number of weapons player can equip */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Weapons")
	int32 MaxWeapons;

	UPROPERTY()
	TMap<ACC_Weapon*, FTimerHandle> WeaponAttackTimers;

public:

	// Weapon Management
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool EquipWeapon(ACC_Weapon* Weapon);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	ACC_Weapon* CreateAndEquipWeapon(TSubclassOf<ACC_Weapon> WeaponClass);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void UnequipWeapon(ACC_Weapon* Weapon);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void UnequipAllWeapons();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SwitchWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PerformAttack();

	// Getters
	UFUNCTION(BlueprintPure, Category = "Weapon")
	ACC_Weapon* GetCurrentWeapon() const { return CurrentWeapon; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	ACC_Weapon* FindWeaponByClass(TSubclassOf<ACC_Weapon> WeaponClass) const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	ACC_Weapon* CreateWeapon(TSubclassOf<ACC_Weapon> WeaponClass);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartWeaponAutoAttack(ACC_Weapon* Weapon);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StopWeaponAutoAttack(ACC_Weapon* Weapon);

	/** Get all equipped weapons */
	UFUNCTION(BlueprintPure, Category = "Player|Weapons")
	TArray<ACC_Weapon*> GetEquippedWeapons() const { return EquippedWeapons; }

	/** Check if can equip more weapons */
	UFUNCTION(BlueprintPure, Category = "Player|Weapons")
	bool CanEquipMoreWeapons() const { return EquippedWeapons.Num() < MaxWeapons; }

	/** Get weapon count */
	UFUNCTION(BlueprintPure, Category = "Player|Weapons")
	int32 GetEquippedWeaponCount() const { return EquippedWeapons.Num(); }

	/** Check if specific weapon is equipped */
	UFUNCTION(BlueprintPure, Category = "Player|Weapons")
	bool HasWeapon(ACC_Weapon* Weapon) const { return EquippedWeapons.Contains(Weapon); }

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

	/** Base experience for level 2 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Experience")
	float BaseExperienceRequirement;

	/** Experience scaling per level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Experience")
	float ExperienceScaling;

public:

	//==============================================================================
	// PUBLIC FUNCTIONS
	//==============================================================================

	// Add experience (called when enemy dies)
	UFUNCTION(BlueprintCallable, Category = "Level")
	void AddExperience(float ExpAmount);

	UFUNCTION(BlueprintCallable, Category = "Level")
	void LevelUp();

	UFUNCTION(BlueprintCallable, Category = "Level")
	int32 GetPlayerLevel() const { return Level; }

	UFUNCTION(BlueprintCallable, Category = "Level")
	float GetExperience() const { return Experience; }

	UFUNCTION(BlueprintPure, Category = "Level")
	float GetExperiencePercentage() const;

	/** Returns CameraBoom subobject */
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject */
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }


	//==============================================================================
	// OVERRIDE FROM BASE CHARACTER
	//==============================================================================

protected:
	// Override ApplyStats to include player-specific stat calculations

	virtual void ApplyStats() override;
};
