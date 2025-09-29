// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "CristalCubeStruct.generated.h"


//==============================================================================
// UPGRADE SYSTEM (Simplified DataTable Approach)
//==============================================================================

// Simple Upgrade Types
UENUM(BlueprintType)
enum class EUpgradeType : uint8
{
    None            UMETA(DisplayName = "None"),
    Damage          UMETA(DisplayName = "Damage"),
    AttackSpeed     UMETA(DisplayName = "Attack Speed"),
    MoveSpeed       UMETA(DisplayName = "Move Speed"),
    Health          UMETA(DisplayName = "Health"),

};


// DataTable Row Structure
USTRUCT(BlueprintType)
struct CRISTALCUBE_API FCristalCubeUpgradeDataRow : public FTableRowBase
{
    GENERATED_BODY()

public:
    //FCristalCubeUpgradeDataRow();

    // Basic Info (for UI)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
    FString DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
    FString Description;

    // Upgrade Logic
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
    EUpgradeType UpgradeType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
    float Value;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Requirements")
    int32 Weight;                   // 출현 확률 가중치
};

//==============================================================================
// WEAPON SYSTEM STRUCTS
//==============================================================================

// Weapon Categories
UENUM(BlueprintType)
enum class EWeaponCategory : uint8
{
    None        UMETA(DisplayName = "None"),
    Melee       UMETA(DisplayName = "Melee"),        
    Ranged      UMETA(DisplayName = "Ranged"),       
    Magic       UMETA(DisplayName = "Magic")         
};



// Base Weapon Data
USTRUCT(BlueprintType)
struct CRISTALCUBE_API FCristalCubeWeaponStats
{
    GENERATED_BODY()

public:
    //FCristalCubeWeaponStats();

    // Basic Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Basic")
    EWeaponCategory WeaponCategory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Basic")
    FString WeaponName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Basic")
    float BaseDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Basic")
    float AttackSpeed;

    FCristalCubeWeaponStats()
        : WeaponCategory(EWeaponCategory::None)
        , BaseDamage(10.0f)
        , AttackSpeed(1.0f)
    {
        // Default weapon stats
        // - BaseDamage: 10 damage per hit
        // - AttackSpeed: 1 attack per second
    }
};

// Melee Weapon Specific Stats
USTRUCT(BlueprintType)
struct CRISTALCUBE_API FCristalCubeMeleeStats
{
    GENERATED_BODY()

public:
    //FCristalCubeMeleeStats();

    // Melee-specific stats (3-4 key stats)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
    float AttackRange;          // 공격 범위

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
    float SwingAngle;           // 휘두르기 각도 (도)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
    int32 ComboCount;           // 연속 공격 횟수

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
    float KnockbackForce;       // 넉백 강도
};

// Ranged Weapon Specific Stats  
USTRUCT(BlueprintType)
struct CRISTALCUBE_API FCristalCubeRangedStats
{
    GENERATED_BODY()

public:
    //FCristalCubeRangedStats();

    // Ranged-specific stats (3-4 key stats)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
    float ProjectileSpeed;      // 투사체 속도

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
    float MaxRange;             // 최대 사거리

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
    int32 ProjectileCount;      // 동시 발사 개수

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
    float Accuracy;             // 정확도 (0.0~1.0)
};

// Magic Weapon Specific Stats
USTRUCT(BlueprintType)
struct CRISTALCUBE_API FCristalCubeMagicStats
{
    GENERATED_BODY()

public:
    //FCristalCubeMagicStats();

    // Magic-specific stats (3-4 key stats)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
    float CastTime;             // 영창 시간

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
    float EffectRadius;         // 마법 효과 범위

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
    float Duration;             // 지속시간 (버프/디버프용)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
    float ManaCost;             // 마나 소모량
};

//==============================================================================
// CHARACTER STATS SYSTEM
//==============================================================================

// Basic Stats - Essential for Week 3-4 MVP
USTRUCT(BlueprintType)
struct CRISTALCUBE_API FCristalCubeBasicStats
{
    GENERATED_BODY()

    // Essential stats for MVP (Week 3-4)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Essential")
    float DamageMultiplier;          // 1.0 = base, 1.2 = +20% damage

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Essential")
    float AttackSpeedMultiplier;     // 1.0 = base, 1.5 = +50% attack speed

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Essential")
    float MoveSpeedMultiplier;       // 1.0 = base, 1.3 = +30% move speed

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Essential")
    float HealthMultiplier;          // 1.0 = base, 1.5 = +50% health

    FCristalCubeBasicStats()
        : DamageMultiplier(1.0f)
        , AttackSpeedMultiplier(1.0f)
        , MoveSpeedMultiplier(1.0f)
        , HealthMultiplier(1.0f)
    {
    }
};

// Advanced Stats - For future expansion (Week 5+)
USTRUCT(BlueprintType)
struct CRISTALCUBE_API FCristalCubeAdvancedStats
{
    GENERATED_BODY()

    // Advanced combat mechanics (not used in MVP)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Future")
    float CriticalChance;            // Critical hit chance (0.0~1.0)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Future")
    float CriticalDamageMultiplier;  // Critical damage multiplier

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Future")
    float AreaOfEffectMultiplier;    // AoE size multiplier

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Future")
    int32 ProjectileCountBonus;      // Extra projectiles

    FCristalCubeAdvancedStats()
        : CriticalChance(0.0f)
        , CriticalDamageMultiplier(1.5f)
        , AreaOfEffectMultiplier(1.0f)
        , ProjectileCountBonus(0)
    {
    }
};

// Defense Stats - For future expansion (Week 5+)
USTRUCT(BlueprintType)
struct CRISTALCUBE_API FCristalCubeDefenseStats
{
    GENERATED_BODY()

    // Defensive mechanics (not used in MVP)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Future")
    float DamageReduction;           // Damage reduction (0.0~1.0)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Future")
    float Evasion;                   // Evasion chance (0.0~1.0)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Future")
    float HealthRegeneration;        // HP regen per second

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Future")
    float Shield;                    // Shield amount

    FCristalCubeDefenseStats()
        : DamageReduction(0.0f)
        , Evasion(0.0f)
        , HealthRegeneration(0.0f)
        , Shield(0.0f)
    {
    }
};

// Utility Stats - For future expansion (Week 5+)
USTRUCT(BlueprintType)
struct CRISTALCUBE_API FCristalCubeUtilityStats
{
    GENERATED_BODY()

    // Utility mechanics (not used in MVP)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Future")
    float ExperienceMultiplier;      // XP gain multiplier

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Future")
    float PickupRange;               // Pickup range multiplier

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Future")
    float CooldownReduction;         // Cooldown reduction (0.0~1.0)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Future")
    float LuckBonus;                 // Luck bonus

    FCristalCubeUtilityStats()
        : ExperienceMultiplier(1.0f)
        , PickupRange(1.0f)
        , CooldownReduction(0.0f)
        , LuckBonus(0.0f)
    {
    }
};

// Player Stats - Main container combining all stat categories
USTRUCT(BlueprintType)
struct CRISTALCUBE_API FCristalCubePlayerStats
{
    GENERATED_BODY()

    // Essential stats (Week 3-4)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    FCristalCubeBasicStats BasicStats;

    // Future expansion stats (Week 5+)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    FCristalCubeAdvancedStats AdvancedStats;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    FCristalCubeDefenseStats DefenseStats;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    FCristalCubeUtilityStats UtilityStats;

    FCristalCubePlayerStats()
        : BasicStats()
        , AdvancedStats()
        , DefenseStats()
        , UtilityStats()
    {
    }
};

//==============================================================================
// CRYSTAL COLLECTION SYSTEM
//==============================================================================

// Crystal Collection Data
USTRUCT(BlueprintType)
struct CRISTALCUBE_API FCristalCubeCollectionData
{
    GENERATED_BODY()

public:
    //FCristalCubeCollectionData();

    // Collection Stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collection|Basic")
    int32 TotalCristalsCollected;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collection|Basic")
    float CollectionRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collection|Basic")
    bool bAutoCollect;

    // Experience System
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collection|Experience")
    float ExperienceMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collection|Experience")
    float BonusExperienceChance;

    // Magnetic Effect
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collection|Magnetic")
    bool bMagneticCollection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collection|Magnetic")
    float MagneticStrength;
};

UCLASS()
class CRISTALCUBE_API ACristalCubeStruct : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACristalCubeStruct();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
