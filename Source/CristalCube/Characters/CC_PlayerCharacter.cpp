// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "../CC_LogHelper.h"
#include "../CC_WeaponManagerSubsystem.h"
#include "../CC_SearchingComponent.h"
#include "../Widgets/CC_GameHUD.h"
#include "../Widgets/CC_LevelUpWidget.h"
#include "../CC_TileTrackerComponent.h"
#include "CC_EnemyCharacter.h"


ACC_PlayerCharacter::ACC_PlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character doesn't rotate in direction of movement
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 640.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;


	// Initialize player stats (all start at 1.0 = no bonus)
	PlayerStats = FCristalCubePlayerStats();

	SearchingComponent = CreateDefaultSubobject<UCC_SearchingComponent>(TEXT("SearchingComponent"));

	TileTrackerComponent = CreateDefaultSubobject<UCC_TileTrackerComponent>(TEXT("TileTrackerComponent"));

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 800.0f;           // Camera distance
	CameraBoom->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));  // Top-down angle
	CameraBoom->bDoCollisionTest = false;

	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;  // Camera does not rotate relative to arm

	// Initialize leveling system
	Level = 1;
	Experience = 0.0f;
	BaseExperienceRequirement = 100.f;
	ExperienceScaling = 1.15f;
	ExperienceToNextLevel = BaseExperienceRequirement;

	// Initialize weapon pointers
	CurrentWeapon = nullptr;

	MaxWeapons = 6;
	StartingWeaponClass = nullptr;

}

void ACC_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CC_LOG_PLAYER(Warning, "PlayerCharacter BeginPlay completed - Input handled by PlayerController");
	CC_LOG_PLAYER(Warning, "Camera Distance: %.1f, Angle: %.1f",
		CameraBoom->TargetArmLength,
		CameraBoom->GetRelativeRotation().Pitch);

	// Apply initial stats
	ApplyPlayerStats();

	if (StartingWeaponClass)
	{
		CreateAndEquipWeapon(StartingWeaponClass);
	}
	else
	{
		CC_LOG_PLAYER(Warning, "No StartingWeaponClass set");

		InitializeWeaponSystem();
		EquipStartingWeapons();
	}

	if (GameHUDClass)
	{
		CurrentGameHUD = CreateWidget<UCC_GameHUD>(GetWorld(), GameHUDClass);
		if (CurrentGameHUD)
		{
			CurrentGameHUD->AddToViewport();
			UpdateGameHUD();
			UE_LOG(LogTemp, Log, TEXT("[Player] Game HUD created"));
		}
		
	}

	if (TileTrackerComponent)
	{
		TileTrackerComponent->OnCurrentTileChanged.AddDynamic(
			this,
			&ACC_PlayerCharacter::OnPlayerTileChanged
		);
	}
}

void ACC_PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GameTime += DeltaTime;

	// HUD Update
	static float HUDUpdateTimer = 0.0f;
	HUDUpdateTimer += DeltaTime;
	if (HUDUpdateTimer >= 0.1f)
	{
		UpdateGameHUD();
		HUDUpdateTimer = 0.0f;
	}
}

void ACC_PlayerCharacter::ApplyPlayerStats()
{
	ApplyStats();
}

void ACC_PlayerCharacter::InitializeWeaponSystem()
{
	UGameInstance* GameInstance = GetGameInstance();
	if(!GameInstance)
	{
		CC_LOG_PLAYER(Error, "Cannot initialize Weapon System - GameInstance is null");
		return;
	}

	WeaponManager = GameInstance->GetSubsystem<UCC_WeaponManagerSubsystem>();

	if (!WeaponManager)
	{
		CC_LOG_PLAYER(Error, "Cannot initialize Weapon System - WeaponManagerSubsystem is null");
		return;
	}

	if (WeaponDataTable)
	{
		WeaponManager->LoadWeaponDataTable(WeaponDataTable);
		CC_LOG_PLAYER(Log, "Weapon System Initialized");
	}
	else
	{
		CC_LOG_PLAYER(Warning, "No WeaponDataTable set - cannot load weapon data");
	}
}

void ACC_PlayerCharacter::EquipStartingWeapons()
{
	if (!WeaponManager)
	{
		CC_LOG_PLAYER(Error, TEXT("WeaponManager Not Initialized!"));
		return;
	}

	TArray<FName> StartingWeaponNames = WeaponManager->GetStartingWeaponNames();

	if (StartingWeaponNames.Num() == 0)
	{
		CC_LOG_PLAYER(Error, TEXT("No Starting weapons available"));
		return;
	}

	TArray<FName> SelectedWeaponNames =
		WeaponManager->GetRandomWeaponNames(StartingWeaponNames, StartingWeaponCount, false);

	for (FName WeaponName : SelectedWeaponNames)
	{
		TSubclassOf<ACC_Weapon> WeaponClass = WeaponManager->GetWeaponClass(WeaponName);

		if (WeaponClass)
		{
			CreateAndEquipWeapon(WeaponClass);

			FWeaponData WeaponData;
			if (WeaponManager->GetWeaponData(WeaponName, WeaponData))
			{
				CC_LOG_PLAYER(Log, TEXT("Equipped: %s"), *WeaponData.WeaponName.ToString());
			}
		}
	}
}

void ACC_PlayerCharacter::EquipWeaponByName(FName WeaponRowName)
{
	if (!WeaponManager)
	{
		CC_LOG_PLAYER(Error, TEXT("WeaponManager not initialized!"));
		return;
	}

	TSubclassOf<ACC_Weapon> WeaponClass = WeaponManager->GetWeaponClass(WeaponRowName);

	if (WeaponClass)
	{
		CreateAndEquipWeapon(WeaponClass);

		FWeaponData WeaponData;
		if (WeaponManager->GetWeaponData(WeaponRowName, WeaponData))
		{
			CC_LOG_PLAYER(Log, TEXT("Equipped weapon: %s"), *WeaponData.WeaponName.ToString());
		}
	}
}

bool ACC_PlayerCharacter::EquipWeapon(ACC_Weapon* Weapon)
{
	// Validation
	if (!Weapon)
	{
		CC_LOG_PLAYER(Warning, "Cannot equip weapon - weapon is null");
		return false;
	}

	if (!CanEquipMoreWeapons())
	{
		CC_LOG_PLAYER(Warning, "Cannot equip more weapons (Max: %d)", MaxWeapons);
		return false;
	}

	if (HasWeapon(Weapon))
	{
		CC_LOG_PLAYER(Warning, "Weapon already equipped");
		return false;
	}

	// Setup weapon
	Weapon->OnEquipped(this);

	// Add to equipped weapons list
	EquippedWeapons.Add(Weapon);

	// Start auto-attack
	StartWeaponAutoAttack(Weapon);

	CC_LOG_PLAYER(Log, "Equipped weapon (Total: %d/%d)",
		EquippedWeapons.Num(), MaxWeapons);

	return true;
}

ACC_Weapon* ACC_PlayerCharacter::CreateAndEquipWeapon(TSubclassOf<ACC_Weapon> WeaponClass)
{
	if (!WeaponClass)
	{
		CC_LOG_PLAYER(Warning, "Cannot create weapon - invalid class");
		return nullptr;
	}

	if (!CanEquipMoreWeapons())
	{
		CC_LOG_PLAYER(Warning, "Cannot equip more weapons (Max: %d)", MaxWeapons);
		return nullptr;
	}

	// Create weapon
	ACC_Weapon* NewWeapon = CreateWeapon(WeaponClass);

	if (!NewWeapon)
	{
		CC_LOG_PLAYER(Error, "Failed to create weapon!");
		return nullptr;
	}

	// Equip it
	if (EquipWeapon(NewWeapon))
	{
		return NewWeapon;
	}
	else
	{
		// Failed to equip, destroy it
		NewWeapon->Destroy();
		return nullptr;
	}
}

void ACC_PlayerCharacter::UnequipWeapon(ACC_Weapon* Weapon)
{
	if (!Weapon)
	{
		CC_LOG_PLAYER(Warning, "Cannot unequip - weapon is null");
		return;
	}

	if (!HasWeapon(Weapon))
	{
		CC_LOG_PLAYER(Warning, "Weapon is not equipped");
		return;
	}

	// Stop auto-attack
	StopWeaponAutoAttack(Weapon);

	// Remove from list
	EquippedWeapons.Remove(Weapon);

	// Cleanup weapon
	Weapon->OnUnequipped();
	Weapon->Destroy();

	CC_LOG_PLAYER(Log, "Unequipped weapon (Remaining: %d)", EquippedWeapons.Num());

}

void ACC_PlayerCharacter::UnequipAllWeapons()
{
	// Create copy of array to iterate (since we're modifying the original)
	TArray<ACC_Weapon*> WeaponsCopy = EquippedWeapons;

	for (ACC_Weapon* Weapon : WeaponsCopy)
	{
		UnequipWeapon(Weapon);
	}

	CC_LOG_PLAYER(Log, "Unequipped all weapons");
}

void ACC_PlayerCharacter::SwitchWeapon()
{

}

void ACC_PlayerCharacter::PerformAttack()
{
	if (CurrentWeapon && CurrentWeapon->CanAttack())
	{
		CurrentWeapon->Attack();
	}
}

ACC_Weapon* ACC_PlayerCharacter::FindWeaponByClass(TSubclassOf<ACC_Weapon> WeaponClass) const
{
	if (!WeaponClass)
	{
		return nullptr;
	}

	for (ACC_Weapon* Weapon : EquippedWeapons)
	{
		if (Weapon && Weapon->GetClass() == WeaponClass)
		{
			return Weapon;
		}
	}

	return nullptr;
}

ACC_Weapon* ACC_PlayerCharacter::CreateWeapon(TSubclassOf<ACC_Weapon> WeaponClass)
{
	if (!WeaponClass)
	{
		return nullptr;
	}

	// Spawn parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn weapon
	ACC_Weapon* NewWeapon = GetWorld()->SpawnActor<ACC_Weapon>(
		WeaponClass,
		GetActorLocation(),
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (NewWeapon)
	{
		CC_LOG_PLAYER(VeryVerbose, "Created weapon from class");
	}
	else
	{
		CC_LOG_PLAYER(Error, "Failed to spawn weapon!");
	}

	return NewWeapon;
}

void ACC_PlayerCharacter::StartWeaponAutoAttack(ACC_Weapon* Weapon)
{
	if (!Weapon)
	{
		CC_LOG_PLAYER(Warning, "Cannot start auto-attack - weapon is null");
		return;
	}

	// Get attack speed
	float AttackSpeed = Weapon->GetAttackSpeed();
	if (AttackSpeed <= 0.0f)
	{
		CC_LOG_PLAYER(Warning, "Invalid attack speed: %.2f", AttackSpeed);
		return;
	}

	// Calculate interval
	float AttackInterval = 1.0f / AttackSpeed;

	// Create timer handle
	FTimerHandle AttackTimer;

	// Set up repeating timer
	GetWorld()->GetTimerManager().SetTimer(
		AttackTimer,
		[Weapon]()
		{
			if (IsValid(Weapon))
			{
				Weapon->Attack();
			}
		},
		AttackInterval,
		true  // Repeating
	);

	// Store timer handle
	WeaponAttackTimers.Add(Weapon, AttackTimer);

	CC_LOG_PLAYER(Log, "Started auto-attack (Interval: %.2fs, Rate: %.1f/s)",
		AttackInterval, AttackSpeed);
}

void ACC_PlayerCharacter::StopWeaponAutoAttack(ACC_Weapon* Weapon)
{
	if (!Weapon)
	{
		return;
	}

	// Find and clear timer
	FTimerHandle* TimerHandle = WeaponAttackTimers.Find(Weapon);
	if (TimerHandle && TimerHandle->IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(*TimerHandle);
		WeaponAttackTimers.Remove(Weapon);

		CC_LOG_PLAYER(VeryVerbose, "Stopped auto-attack for weapon");
	}
}

void ACC_PlayerCharacter::LevelUp()
{
	Level++;
	Experience -= ExperienceToNextLevel;

	// Increase XP requirement for next level (simple scaling / Request to change to refined variables in the future)
	ExperienceToNextLevel *= ExperienceScaling;

	// Heal to full on level up
	Heal(MaxHealth);

	UE_LOG(LogTemp, Log, TEXT("Level Up! Now level %d"), Level);

	UpdateGameHUD();
	ShowLevelUpUI();
}

void ACC_PlayerCharacter::OnPlayerTileChanged(int32 OldTileIndex, int32 NewTileIndex, FVector NewPosition)
{
	UE_LOG(LogTemp, Log, TEXT("[Player] Tile changed: %d -> %d"), OldTileIndex, NewTileIndex);
}

void ACC_PlayerCharacter::UpdateGameHUD()
{
	if (!CurrentGameHUD) return;

	CurrentGameHUD->UpdateHealth(CurrentHealth, MaxHealth);
	CurrentGameHUD->UpdateExp(Experience, ExperienceToNextLevel, Level);
	CurrentGameHUD->UpdateTimer(GameTime);

}

float ACC_PlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHealth = FMath::Max(0.0f, CurrentHealth - ActualDamage);

	UpdateGameHUD();

	return ActualDamage;
}

void ACC_PlayerCharacter::AddExperience(float ExpAmount)
{
	Experience += ExpAmount;

	UpdateGameHUD();

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

void ACC_PlayerCharacter::ShowLevelUpUI()
{
	if (!LevelUpUIClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[Player] LevelUpUIClass not set!"));
		return;
	}

	if (CurrentLevelUpUI)
	{
		return;
	}

	CurrentLevelUpUI = CreateWidget<UCC_LevelUpWidget>(GetWorld(), LevelUpUIClass);

	if (!CurrentLevelUpUI)
	{
		UE_LOG(LogTemp, Error, TEXT("[Player] Failed to create LevelUpUI"));
		return;
	}

	if(WeaponManager)
	{
		TArray<FName> Options = WeaponManager->GetRandomWeaponNames(WeaponManager->GetAllWeaponNames(), 3);
		CurrentLevelUpUI->SetWeaponChoices(Options);

		CurrentLevelUpUI->OnWeaponSelected.AddDynamic(this, &ACC_PlayerCharacter::OnWeaponSelected);
	}

	CurrentLevelUpUI->AddToViewport(10);

	UGameplayStatics::SetGamePaused(GetWorld(), true);

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
	}

	UE_LOG(LogTemp, Log, TEXT("[Player] Level Up UI shown"));
	//if (LevelUpWidgetClass)
	//{
	//	UUserWidget* Widget = CreateWidget<UUserWidget>(
	//		GetWorld(),
	//		LevelUpWidgetClass
	//	);
	//	Widget->AddToViewport();
	//}

}

void ACC_PlayerCharacter::HideLevelUpUI()
{
	if (CurrentLevelUpUI)
	{
		CurrentLevelUpUI->RemoveFromParent();
		CurrentLevelUpUI = nullptr;
	}

	UGameplayStatics::SetGamePaused(GetWorld(), false);

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}

	UE_LOG(LogTemp, Log, TEXT("[Player] Level Up UI hidden"));
}

void ACC_PlayerCharacter::OnWeaponSelected(FName WeaponName)
{
	UE_LOG(LogTemp, Warning, TEXT("[Player] Weapon Selected: %s"), *WeaponName.ToString());

	if (WeaponManager)
	{
		FWeaponData* WeaponData = WeaponManager->GetWeaponDataPtr(WeaponName);
		if (WeaponData && WeaponData->WeaponClass)
		{
			CreateAndEquipWeapon(WeaponData->WeaponClass);
		}
	}

	HideLevelUpUI();
}

void ACC_PlayerCharacter::ApplyWeaponUpgrade(FName WeaponID)
{
	UCC_WeaponManagerSubsystem* WeaponMgr = GetGameInstance()
		->GetSubsystem<UCC_WeaponManagerSubsystem>();

	FWeaponData* Data = WeaponMgr->GetWeaponDataPtr(WeaponID);

	if (Data && Data->WeaponClass)
	{
		CreateAndEquipWeapon(Data->WeaponClass);
	}

	UGameplayStatics::SetGamePaused(GetWorld(), false);
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
