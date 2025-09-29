// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"


ACC_PlayerCharacter::ACC_PlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize player stats (all start at 1.0 = no bonus)
	PlayerStats = FCristalCubePlayerStats();

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 800.0f;           // Camera distance
	CameraBoom->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));  // Top-down angle
	CameraBoom->bDoCollisionTest = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;  // Camera does not rotate relative to arm

	// Initialize leveling system
	Level = 1;
	Experience = 0.0f;
	ExperienceToNextLevel = 100.0f;

	// Initialize weapon pointers
	PrimaryWeapon = nullptr;
	SecondaryWeapon = nullptr;
	CurrentWeapon = nullptr;
}

void ACC_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Apply initial stats
	ApplyPlayerStats();
}

void ACC_PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACC_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Primary Attack
		EnhancedInputComponent->BindAction(PrimaryAttackAction, ETriggerEvent::Triggered, this, &ACC_PlayerCharacter::PrimaryAttack);

		// Secondary Attack  
		EnhancedInputComponent->BindAction(SecondaryAttackAction, ETriggerEvent::Triggered, this, &ACC_PlayerCharacter::SecondaryAttack);
	}
}

void ACC_PlayerCharacter::PrimaryAttack()
{
}

void ACC_PlayerCharacter::SecondaryAttack()
{
}

void ACC_PlayerCharacter::ApplyPlayerStats()
{
	ApplyStats();
}

void ACC_PlayerCharacter::EquipWeapon(ACC_Weapon* NewWeapon, bool bIsPrimary)
{
	if (!NewWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to equip null weapon"));
		return;
	}

	// Determine which slot to use
	ACC_Weapon** TargetSlot = bIsPrimary ? &PrimaryWeapon : &SecondaryWeapon;

	// Remove old weapon from slot if it exists
	if (*TargetSlot)
	{
		(*TargetSlot)->OnUnequipped();
		(*TargetSlot)->Destroy();
	}

	// Equip new weapon
	*TargetSlot = NewWeapon;
	NewWeapon->OnEquipped(this);

	// Set as current weapon if no current weapon or if equipping to primary slot
	if (!CurrentWeapon || bIsPrimary)
	{
		CurrentWeapon = NewWeapon;
	}

	UE_LOG(LogTemp, Log, TEXT("Equipped weapon to %s slot"), bIsPrimary ? TEXT("Primary") : TEXT("Secondary"));

}

void ACC_PlayerCharacter::SwitchWeapon()
{
	// Switch between primary and secondary weapons
	if (PrimaryWeapon && SecondaryWeapon)
	{
		CurrentWeapon = (CurrentWeapon == PrimaryWeapon) ? SecondaryWeapon : PrimaryWeapon;
		UE_LOG(LogTemp, Log, TEXT("Switched to %s weapon"),
			(CurrentWeapon == PrimaryWeapon) ? TEXT("Primary") : TEXT("Secondary"));
	}
}

void ACC_PlayerCharacter::PerformAttack()
{
	if (CurrentWeapon && CurrentWeapon->CanAttack())
	{
		CurrentWeapon->Attack();
	}
}

void ACC_PlayerCharacter::LevelUp()
{
	Level++;
	Experience -= ExperienceToNextLevel;

	// Increase XP requirement for next level (simple scaling / Request to change to refined variables in the future)
	ExperienceToNextLevel *= 1.2f;

	// Heal to full on level up
	Heal(MaxHealth);

	UE_LOG(LogTemp, Log, TEXT("Level Up! Now level %d"), Level);
}

void ACC_PlayerCharacter::AddExperience(float ExpAmount)
{
	Experience += ExpAmount;

	// Check for level up
	while (Experience >= ExperienceToNextLevel)
	{
		LevelUp();
	}
}

float ACC_PlayerCharacter::GetExperiencePercentage() const
{
	if (ExperienceToNextLevel <= 0.0f)
	{
		return 0.0f;
	}
	return Experience / ExperienceToNextLevel;
}

void ACC_PlayerCharacter::ApplyStats()
{
	Super::ApplyStats();

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		float FinalMoveSpeed = MoveSpeed * PlayerStats.BasicStats.MoveSpeedMultiplier;
		Movement->MaxWalkSpeed = FinalMoveSpeed;
	}

	// Apply health multiplier
	float FinalMaxHealth = MaxHealth * PlayerStats.BasicStats.HealthMultiplier;

	// If health was at max, keep it at max after stat change
	if (FMath::IsNearlyEqual(CurrentHealth, MaxHealth))
	{
		CurrentHealth = FinalMaxHealth;
	}
	else
	{
		// Otherwise, adjust current health proportionally
		float HealthPercentage = CurrentHealth / MaxHealth;
		CurrentHealth = FinalMaxHealth * HealthPercentage;
	}

	MaxHealth = FinalMaxHealth;

	UE_LOG(LogTemp, Log, TEXT("Stats Applied - Health: %.0f/%.0f, Speed: %.0f"),
		CurrentHealth, MaxHealth, GetCharacterMovement()->MaxWalkSpeed);
}
