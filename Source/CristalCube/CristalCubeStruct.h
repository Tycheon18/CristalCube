// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "CristalCubeStruct.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFadeComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCubeTransition, FIntPoint, NewCoordinate);

class UNiagaraSystem;

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

//====================================================================================
// Attack Hit Types
//====================================================================================

UENUM(BlueprintType)
enum class EAttackHitType : uint8
{
	Point       UMETA(DisplayName = "Point"),       // one target
	Sphere      UMETA(DisplayName = "Sphere"),      // circular area 360 degree
    Line        UMETA(DisplayName = "Line"),        // 
    Box         UMETA(DisplayName = "Box"),         // square
    Cone        UMETA(DisplayName = "Cone"),        // 
    Capsule     UMETA(DisplayName = "Capsule")      // 
};

//==============================================================================
// CUBE MANAGEMENT SYSTEM
//==============================================================================

UENUM(BlueprintType)
enum class EBoundaryDirection : uint8
{
    Right UMETA(DisplayName = "Right"),
    Left UMETA(DisplayName = "Left"),
    Up UMETA(DisplayName = "Up"),
    Down UMETA(DisplayName = "Down")
};


UENUM(BlueprintType)
enum class ECubeState : uint8
{
    Active UMETA(DisplayName = "Active"),      // ���� Ȱ�� ť��
    Frozen UMETA(DisplayName = "Frozen"),      // ��Ȱ�� (�ð� ����)
    Unloaded UMETA(DisplayName = "Unloaded")   // �޸𸮿� ����
};

//==============================================================================
// STRUCTS
//==============================================================================


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

// Weapon Target Types
UENUM(BlueprintType)
enum class ETargetingMode : uint8
{
	None       UMETA(DisplayName = "No Targeting"),
    Single     UMETA(DisplayName = "Single Target"),
    Multi      UMETA(DisplayName = "Multi Target"),
    Area       UMETA(DisplayName = "Area(All in Range)"),
    Self       UMETA(DisplayName = "Self")
};

/// <Skill System>
/// 새로 작성된 모듈형 스킬 시스템 구조체, 프로토타입 버전

//==============================================================================
// MODULAR SKILL SYSTEM (Week 9 - Simplified Prototype)
//==============================================================================

// Forward declarations
class UNiagaraSystem;

//------------------------------------------------------------------------------
// Core Types - 스킬의 기본 형태 (3개만)
//------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ESkillCoreType : uint8
{
    None            UMETA(DisplayName = "None"),
    Projectile      UMETA(DisplayName = "Projectile"),     // 투사체 (날아감)
    Instant         UMETA(DisplayName = "Instant"),        // 즉발 (히트스캔)
    Area            UMETA(DisplayName = "Area"),           // 범위 (바닥/공간)
    Beam            UMETA(DisplayName = "Beam")            // 레이저 
};

//------------------------------------------------------------------------------
// Addon Types - 추가 효과 (4개만)
//------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ESkillAddonType : uint8
{
    None            UMETA(DisplayName = "None"),
    Explosion       UMETA(DisplayName = "Explosion"),      // 충돌 시 폭발
    Chain           UMETA(DisplayName = "Chain"),          // 다음 적으로 연쇄
    Penetrate       UMETA(DisplayName = "Penetrate"),      // 관통
    MultiShot       UMETA(DisplayName = "MultiShot")       // 다중 발사
};

//------------------------------------------------------------------------------
// Element Types - 원소 속성
//------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ESkillElementType : uint8
{
    None            UMETA(DisplayName = "None"),
    Physical        UMETA(DisplayName = "Physical"),       // 물리
    Fire            UMETA(DisplayName = "Fire"),           // 화염
    Ice             UMETA(DisplayName = "Ice"),            // 얼음
    Lightning       UMETA(DisplayName = "Lightning"),      // 번개
    Poison          UMETA(DisplayName = "Poison")          // 독
};

//------------------------------------------------------------------------------
// Passive Properties - 수치 강화
//------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FSkillPassiveProperties
{
    GENERATED_BODY()

    // 배율 (곱셈)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive|Multipliers")
    float DamageMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive|Multipliers")
    float SizeMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive|Multipliers")
    float SpeedMultiplier = 1.0f;

    // 가산 (덧셈)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive|Counts")
    int32 PierceCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive|Counts")
    int32 ChainCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive|Counts")
    int32 ProjectileCount = 1;
};

//------------------------------------------------------------------------------
// Skill Definition - 완전한 스킬 정의
//------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FSkillDefinition
{
    GENERATED_BODY()

    // === 기본 정보 ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic")
    FName SkillID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic")
    float BaseDamage = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic")
    float Cooldown = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic")
    float Range = 1000.0f;

    // === Core (필수, 하나만) ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    ESkillCoreType CoreType = ESkillCoreType::Projectile;

    // === Addons (선택, 여러 개) ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Addons")
    TArray<ESkillAddonType> Addons;

    // === Passive (수치) ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive")
    FSkillPassiveProperties Passives;

    // === Element (비주얼/효과) ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    ESkillElementType ElementType = ESkillElementType::Physical;

    // === VFX ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    UNiagaraSystem* CastEffect = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    UNiagaraSystem* HitEffect = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    UNiagaraSystem* ExplosionEffect = nullptr;

    // === Audio ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* CastSound = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* HitSound = nullptr;
};

//------------------------------------------------------------------------------
// Skill Execution Context - 런타임 데이터
//------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FSkillExecutionContext
{
    GENERATED_BODY()

    UPROPERTY()
    AActor* Caster = nullptr;

    UPROPERTY()
    FVector StartLocation = FVector::ZeroVector;

    UPROPERTY()
    FVector TargetLocation = FVector::ZeroVector;

    UPROPERTY()
    FVector Direction = FVector::ForwardVector;

    // 런타임 추적
    UPROPERTY()
    TArray<AActor*> HitActors;          // 이미 맞은 적들 (관통/연쇄용)

    UPROPERTY()
    int32 CurrentChainCount = 0;

    UPROPERTY()
    int32 CurrentPierceCount = 0;

    UPROPERTY()
    float CurrentDamage = 0.0f;
};

//------------------------------------------------------------------------------
// DataTable Row - 스킬 라이브러리
//------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FSkillTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSkillDefinition SkillData;

    // UI 전용
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    UTexture2D* Icon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    FText Description;

    // 드롭/획득 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Acquisition")
    float DropWeight = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Acquisition")
    bool bIsStartingSkill = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Acquisition")
    int32 UnlockLevel = 1;
};
/// </Skill System>

// Bast Weapon Data
USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic Info")
    FText WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic Info")
	FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic Info")
	UTexture2D* Icon;
    
    UPROPERTY(Editanywhere, BlueprintReadOnly)
	TSubclassOf<class ACC_Weapon> WeaponClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 MaxLevel;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bIsStartingWeapon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float DropWeight;

    FWeaponData()
        : WeaponName(FText::FromString("Unknown Weapon"))
        , Description(FText::FromString("No description"))
        , Icon(nullptr)
        , WeaponClass(nullptr)
        , MaxLevel(5)
        , bIsStartingWeapon(true)
        , DropWeight(1.0f)
    {
    }
};

// Base Weapon Stats Data
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

USTRUCT(BlueprintType)
struct CRISTALCUBE_API FCristalCubeEnemyStats
{
    GENERATED_BODY()

	// Enemy stats (simplified for MVP)

public:

    UPROPERTY(EditAnywhere, BlueprintREadWrite, Category = "Stats")
    float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintREadWrite, Category = "Stats")
    float AttackCooldown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackRange; 

    FCristalCubeEnemyStats()
    {
        AttackDamage = 10.0f;
        AttackCooldown = 1.0f;
        AttackRange = 200.0f;
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

//==============================================================================
// Attack Hit Data
//==============================================================================

USTRUCT(BlueprintType)
struct FAttackHitData
{
    GENERATED_BODY()

    // 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    EAttackHitType HitType = EAttackHitType::Line;

    // 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (ClampMin = "0"))
    float Range = 200.0f;

    // 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (ClampMin = "0"))
    float Width = 100.0f;

    // 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (ClampMin = "0"))
    float Height = 25.0f;

    // 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (ClampMin = "0"))
    float Thickness = 50.0f;

    // 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (ClampMin = "0", ClampMax = "360"))
    float Angle = 90.0f;

    // 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (ClampMin = "0"))
    float Radius = 50.0f;

    //
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    bool bCheckFrontOnly = false;

    // 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    bool bPenetrate = false;

    FAttackHitData()
        : HitType(EAttackHitType::Sphere)
        , Range(200.0f)
        , Width(100.0f)
        , Height(25.0f)
        , Thickness(50.0f)
        , Angle(90.0f)
        , Radius(50.0f)
        , bCheckFrontOnly(false)
        , bPenetrate(false)
    {
    }
};

//==============================================================================
// Actor Save Data
//==============================================================================

USTRUCT(BlueprintType)
struct FActorSaveData
{
    GENERATED_BODY()

    UPROPERTY()
    TSubclassOf<AActor> ActorClass;

    UPROPERTY()
    FTransform Transform;

    UPROPERTY()
    TMap<FString, FString> ActorState;
};

//==============================================================================
// Cube Data
//==============================================================================

USTRUCT(BlueprintType)
struct FCubeData
{
    GENERATED_BODY()

    UPROPERTY()
    FIntPoint Coordinate = FIntPoint(0, 0);

    UPROPERTY()
    TArray<FActorSaveData> SavedActors;

    UPROPERTY()
    ECubeState State = ECubeState::Unloaded;

    UPROPERTY()
    bool bCleared = false;

    UPROPERTY()
    int32 CubeType = 0; // ���� Ȯ���
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
