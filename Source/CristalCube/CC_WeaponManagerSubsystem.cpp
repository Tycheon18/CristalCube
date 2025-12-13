// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_WeaponManagerSubsystem.h"
#include "CristalCubeStruct.h"

void UCC_WeaponManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("[WeaponManagerSubsystem] Initialized"));
}

void UCC_WeaponManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Log, TEXT("[WeaponManagerSubsystem] Deinitialized"));
}

void UCC_WeaponManagerSubsystem::LoadWeaponDataTable(UDataTable* DataTable)
{
    if (!DataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("[WeaponManagerSubsystem] Invalid DataTable!"));
        return;
    }

    WeaponDataTable = DataTable;
    UE_LOG(LogTemp, Log, TEXT("[WeaponManagerSubsystem] Loaded Weapon DataTable: %s"), *DataTable->GetName());
}

bool UCC_WeaponManagerSubsystem::GetWeaponData(FName WeaponRowName, FWeaponData& OutWeaponData)
{
    FWeaponData* Data = GetWeaponDataPtr(WeaponRowName);
    if (Data)
    {
        OutWeaponData = *Data;  
        return true;
    }
    return false;
}

TSubclassOf<ACC_Weapon> UCC_WeaponManagerSubsystem::GetWeaponClass(FName WeaponRowName)
{
    FWeaponData* Data = GetWeaponDataPtr(WeaponRowName);
    return Data ? Data->WeaponClass : nullptr;
}

TArray<FName> UCC_WeaponManagerSubsystem::GetAllWeaponNames()
{
    TArray<FName> WeaponNames;

    if (!WeaponDataTable) return WeaponNames;

    WeaponNames = WeaponDataTable->GetRowNames();
    return WeaponNames;
}

TArray<FName> UCC_WeaponManagerSubsystem::GetStartingWeaponNames()
{
    TArray<FName> StartingNames;
    TArray<FWeaponData*> AllWeapons = GetAllWeaponsPtr();

    for (FWeaponData* Weapon : AllWeapons)
    {
        if (Weapon && Weapon->bIsStartingWeapon)
        {
            TArray<FName> RowNames = WeaponDataTable->GetRowNames();
            for (FName RowName : RowNames)
            {
                FWeaponData* RowData = GetWeaponDataPtr(RowName);
                if (RowData == Weapon)
                {
                    StartingNames.Add(RowName);
                    break;
                }
            }
        }
    }

    return StartingNames;
}

FName UCC_WeaponManagerSubsystem::GetRandomWeaponName(const TArray<FName>& WeaponPool)
{
    if (WeaponPool.Num() == 0) return NAME_None;

    TArray<FWeaponData*> WeaponDataArray;
    for (FName WeaponName : WeaponPool)
    {
        FWeaponData* Data = GetWeaponDataPtr(WeaponName);
        if (Data) WeaponDataArray.Add(Data);
    }

    FWeaponData* Selected = SelectRandomWeaponByWeight(WeaponDataArray);

    if (Selected)
    {
        for (FName RowName : WeaponPool)
        {
            if (GetWeaponDataPtr(RowName) == Selected)
            {
                return RowName;
            }
        }
    }

    return NAME_None;
}

TArray<FName> UCC_WeaponManagerSubsystem::GetRandomWeaponNames(
    const TArray<FName>& WeaponPool,
    int32 Count,
    bool bAllowDuplicates)
{
    TArray<FName> SelectedNames;
    TArray<FName> RemainingPool = WeaponPool;

    Count = FMath::Min(Count, bAllowDuplicates ? Count : WeaponPool.Num());

    for (int32 i = 0; i < Count; i++)
    {
        if (RemainingPool.Num() == 0) break;

        FName Selected = GetRandomWeaponName(RemainingPool);
        if (Selected != NAME_None)
        {
            SelectedNames.Add(Selected);

            if (!bAllowDuplicates)
            {
                RemainingPool.Remove(Selected);
            }
        }
    }

    return SelectedNames;
}

FWeaponData* UCC_WeaponManagerSubsystem::GetWeaponDataPtr(FName WeaponRowName)
{
    if (!WeaponDataTable) return nullptr;

    static const FString ContextString(TEXT("GetWeaponDataPtr"));
    return WeaponDataTable->FindRow<FWeaponData>(WeaponRowName, ContextString);
}

TArray<FWeaponData*> UCC_WeaponManagerSubsystem::GetAllWeaponsPtr()
{
    TArray<FWeaponData*> AllWeapons;
    if (!WeaponDataTable) return AllWeapons;

    WeaponDataTable->GetAllRows(TEXT("GetAllWeaponsPtr"), AllWeapons);
    return AllWeapons;
}

TArray<FWeaponData*> UCC_WeaponManagerSubsystem::GetStartingWeaponsPtr()
{
    TArray<FWeaponData*> StartingWeapons;
    TArray<FWeaponData*> AllWeapons = GetAllWeaponsPtr();

    for (FWeaponData* Weapon : AllWeapons)
    {
        if (Weapon && Weapon->bIsStartingWeapon)
        {
            StartingWeapons.Add(Weapon);
        }
    }

    return StartingWeapons;
}

FWeaponData* UCC_WeaponManagerSubsystem::SelectRandomWeaponByWeight(
    const TArray<FWeaponData*>& WeaponPool)
{
    if (WeaponPool.Num() == 0) return nullptr;

    float TotalWeight = 0.0f;
    for (FWeaponData* Weapon : WeaponPool)
    {
        if (Weapon) TotalWeight += Weapon->DropWeight;
    }

    float RandomValue = FMath::FRandRange(0.0f, TotalWeight);
    float CurrentWeight = 0.0f;

    for (FWeaponData* Weapon : WeaponPool)
    {
        if (Weapon)
        {
            CurrentWeight += Weapon->DropWeight;
            if (RandomValue <= CurrentWeight)
            {
                return Weapon;
            }
        }
    }

    return WeaponPool[0]; 
}