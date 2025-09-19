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
    int32 Weight;                   // ���� Ȯ�� ����ġ
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
    float AttackRange;          // ���� ����

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
    float SwingAngle;           // �ֵθ��� ���� (��)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
    int32 ComboCount;           // ���� ���� Ƚ��

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
    float KnockbackForce;       // �˹� ����
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
    float ProjectileSpeed;      // ����ü �ӵ�

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
    float MaxRange;             // �ִ� ��Ÿ�

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
    int32 ProjectileCount;      // ���� �߻� ����

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
    float Accuracy;             // ��Ȯ�� (0.0~1.0)
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
    float CastTime;             // ��â �ð�

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
    float EffectRadius;         // ���� ȿ�� ����

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
    float Duration;             // ���ӽð� (����/�������)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
    float ManaCost;             // ���� �Ҹ�
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

    // Week 3-4 MVP: ���� �ʿ��� �͸�
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common|Essential")
    float DamageMultiplier;         // ������ ���� (1.0 = �⺻, 1.2 = 20% ����)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common|Essential")
    float AttackSpeedMultiplier;    // ���ݼӵ� ���� (1.0 = �⺻, 1.5 = 50% ����)

    // Week 5+ Ȯ��� (Ʋ�� �غ�)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common|Future")
    float CriticalChance;           // ���߿� ���

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common|Future")
    int32 ProjectileBonus;          // ���߿� ���
};

// Basic Character Stats (non-weapon related)
USTRUCT(BlueprintType)
struct CRISTALCUBE_API FCristalCubeBasicStats
{
    GENERATED_BODY()

public:
    //FCristalCubeBasicStats();
    
    // Week 3-4 MVP: ������ �ʼ����� �͸�
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic|Essential")
    float MoveSpeedMultiplier;      // �̵��ӵ� ���� (1.0 = �⺻)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic|Essential")
    float HealthMultiplier;         // ü�� ���� (1.0 = �⺻)

    // Week 5+ Ȯ��� (Ʋ�� �غ�)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic|Future")
    float HealthRegeneration;       // ���߿� ���

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic|Future")
    float PickupRange;              // ���߿� ���
};

// Weapon Proficiency Stats (Ȯ��� Ʋ�� �غ�)
USTRUCT(BlueprintType)
struct CRISTALCUBE_API FCristalCubeWeaponProficiency
{
    GENERATED_BODY()

public:
    //FCristalCubeWeaponProficiency();

    // Week 7-8+ Ȯ���: ���⺰ ������ (������ ������� ����)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proficiency|Future")
    float MeleeRangeBonus;          // ���߿� ���

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proficiency|Future")
    float ProjectileSpeedBonus;     // ���߿� ���

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proficiency|Future")
    float CastTimeReduction;        // ���߿� ���
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

    // Weapon Ownership (� ���⸦ �����ߴ���)
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
