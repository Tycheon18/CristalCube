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

// Common Stats that affect multiple weapon types
USTRUCT(BlueprintType)
struct CRISTALCUBE_API FCristalCubeCommonStats
{
    GENERATED_BODY()

public:
    //FCristalCubeCommonStats();

    // Week 3-4 MVP: 정말 필요한 것만
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common|Essential")
    float DamageMultiplier;         // 데미지 배율 (1.0 = 기본, 1.2 = 20% 증가)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common|Essential")
    float AttackSpeedMultiplier;    // 공격속도 배율 (1.0 = 기본, 1.5 = 50% 빠름)

    // Week 5+ 확장용 (틀만 준비)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common|Future")
    float CriticalChance;           // 나중에 사용

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common|Future")
    int32 ProjectileBonus;          // 나중에 사용
};

// Basic Character Stats (non-weapon related)
USTRUCT(BlueprintType)
struct CRISTALCUBE_API FCristalCubeBasicStats
{
    GENERATED_BODY()

public:
    //FCristalCubeBasicStats();
    
    // Week 3-4 MVP: 생존에 필수적인 것만
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic|Essential")
    float MoveSpeedMultiplier;      // 이동속도 배율 (1.0 = 기본)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic|Essential")
    float HealthMultiplier;         // 체력 배율 (1.0 = 기본)

    // Week 5+ 확장용 (틀만 준비)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic|Future")
    float HealthRegeneration;       // 나중에 사용

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic|Future")
    float PickupRange;              // 나중에 사용
};

// Weapon Proficiency Stats (확장용 틀만 준비)
USTRUCT(BlueprintType)
struct CRISTALCUBE_API FCristalCubeWeaponProficiency
{
    GENERATED_BODY()

public:
    //FCristalCubeWeaponProficiency();

    // Week 7-8+ 확장용: 무기별 전문성 (지금은 사용하지 않음)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proficiency|Future")
    float MeleeRangeBonus;          // 나중에 사용

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proficiency|Future")
    float ProjectileSpeedBonus;     // 나중에 사용

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proficiency|Future")
    float CastTimeReduction;        // 나중에 사용
};

// Master Character Stats Container
USTRUCT(BlueprintType)
struct CRISTALCUBE_API FCristalCubeCharacterStats
{
    GENERATED_BODY()

public:
    //FCristalCubeCharacterStats();

    // All stat categories
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    FCristalCubeBasicStats BasicStats;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    FCristalCubeCommonStats CommonStats;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    FCristalCubeWeaponProficiency WeaponProficiency;

    // Weapon Ownership (어떤 무기를 보유했는지)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Inventory")
    bool bHasMeleeWeapon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Inventory")
    bool bHasRangedWeapon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Inventory")
    bool bHasMagicWeapon;
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
