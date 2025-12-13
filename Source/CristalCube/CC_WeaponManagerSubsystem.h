// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CristalCubeStruct.h"
#include "CC_WeaponManagerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class CRISTALCUBE_API UCC_WeaponManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* WeaponDataTable;

public:

	UFUNCTION(BlueprintCallable, Category = "Weapon Manager")
	void LoadWeaponDataTable(UDataTable* DataTable);

	UFUNCTION(BlueprintCallable, Category = "Weapon Manager")
	bool GetWeaponData(FName WeaponRowName, FWeaponData& OutWeaponData);

	UFUNCTION(BlueprintCallable, Category = "Weapon Manager")
	TSubclassOf<class ACC_Weapon> GetWeaponClass(FName WeaponRowName);

	UFUNCTION(BlueprintCallable, Category = "Weapon Manager")
	TArray<FName> GetAllWeaponNames();

	UFUNCTION(BlueprintCallable, Category = "Weapon Manager")
	TArray<FName> GetStartingWeaponNames();

	UFUNCTION(BlueprintCallable, Category = "Weapon Manager")
	FName GetRandomWeaponName(const TArray<FName>& WeaponPool);

	UFUNCTION(BlueprintCallable, Category = "Weapon Manager")
	TArray<FName> GetRandomWeaponNames(
		const TArray<FName>& WeaponPool,
		int32 Count,
		bool bAllowDuplicates = false
	);

	FWeaponData* GetWeaponDataPtr(FName WeaponRowName);

	TArray<FWeaponData*> GetAllWeaponsPtr();

	TArray<FWeaponData*> GetStartingWeaponsPtr();

protected:

	FWeaponData* SelectRandomWeaponByWeight(const TArray<FWeaponData*>& WeaponPool);
};
