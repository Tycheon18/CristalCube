// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"


ACC_PlayerCharacter::ACC_PlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerStats.BasicStats.MoveSpeedMultiplier = 1.0f;
	PlayerStats.BasicStats.HealthMultiplier = 1.0f;
	PlayerStats.CommonStats.DamageMultiplier = 1.0f;
	PlayerStats.CommonStats.AttackSpeedMultiplier = 1.0f;

	// Initialize leveling system
	Level = 1;
	Experience = 0.0f;
	ExperienceToNextLevel = 100.0f;

	// Initialize weapon pointers
	PrimaryWeapon = nullptr;
	SecondaryWeapon = nullptr;
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
	ApplyStats();
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

void ACC_PlayerCharacter::ApplyStats()
{
}

void ACC_PlayerCharacter::LevelUp()
{
}

void ACC_PlayerCharacter::AddExperience(float ExpAmount)
{
}
