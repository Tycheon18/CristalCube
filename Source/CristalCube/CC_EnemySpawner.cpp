// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_EnemySpawner.h"
#include "Characters/CC_EnemyCharacter.h"
#include "Characters/CC_PlayerCharacter.h"
#include "CC_LogHelper.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACC_EnemySpawner::ACC_EnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Default spawn settings
    SpawnInterval = 3.0f;
    CurrentSpawnInterval = SpawnInterval;
    EnemiesPerSpawn = 3;
    MinSpawnDistance = 800.0f;
    MaxSpawnDistance = 1200.0f;
    MaxEnemies = 50;

    // Difficulty scaling
    bIncreaseSpawnRate = true;
    SpawnIntervalDecreasePerMinute = 0.1f;
    MinSpawnInterval = 0.5f;

    // State
    bAutoStart = true;
    bIsSpawning = false;
    GameTime = 0.0f;
    CachedPlayer = nullptr;

}

// Called when the game starts or when spawned
void ACC_EnemySpawner::BeginPlay()
{
    Super::BeginPlay();

    FindPlayer();

    if (!EnemyClass)
    {
        CC_LOG_SPAWNER(Error, TEXT("No enemy class set! Spawner disabled."));
        return;
    }

    CC_LOG_SPAWNER(Warning, TEXT("EnemySpawner initialized (Interval: %.1fs, Enemies/Spawn: %d, Max: %d)"),
        SpawnInterval, EnemiesPerSpawn, MaxEnemies);

    if (bAutoStart)
    {
        StartSpawning();
    }
	
}

// Called every frame
void ACC_EnemySpawner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    GameTime += DeltaTime;

    // Update spawn interval based on difficulty scaling
    if (bIncreaseSpawnRate && bIsSpawning)
    {
        UpdateSpawnInterval();
    }

    // Periodically clean up dead enemies
    static float CleanupTimer = 0.0f;
    CleanupTimer += DeltaTime;

    if (CleanupTimer >= 5.0f)  // Every 5 seconds
    {
        CleanupDeadEnemies();
        CleanupTimer = 0.0f;
    }
}

void ACC_EnemySpawner::StartSpawning()
{
    if (bIsSpawning)
    {
        CC_LOG_SPAWNER(Warning, TEXT("Already spawning!"));
        return;
    }

    if (!EnemyClass)
    {
        CC_LOG_SPAWNER(Error, TEXT("Cannot start spawning - no enemy class set!"));
        return;
    }

    bIsSpawning = true;
    CurrentSpawnInterval = SpawnInterval;

    // Set up repeating timer
    GetWorld()->GetTimerManager().SetTimer(
        SpawnTimerHandle,
        this,
        &ACC_EnemySpawner::SpawnEnemies,
        CurrentSpawnInterval,
        true  // Repeating
    );

    CC_LOG_SPAWNER(Log, TEXT("Started spawning enemies"));

    // Spawn immediately
    SpawnEnemies();
}

void ACC_EnemySpawner::StopSpawning()
{
    if (!bIsSpawning)
    {
        return;
    }

    bIsSpawning = false;
    GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);

    CC_LOG_SPAWNER(Log, TEXT("Stopped spawning enemies"));
}

void ACC_EnemySpawner::SpawnEnemies()
{
    if (!CanSpawnMore())
    {
        CC_LOG_SPAWNER(VeryVerbose, TEXT("Max enemies reached (%d/%d), skipping spawn"),
            GetAliveEnemyCount(), MaxEnemies);
        return;
    }

    if (!CachedPlayer)
    {
        FindPlayer();
        if (!CachedPlayer)
        {
            CC_LOG_SPAWNER(Warning, TEXT("No player found, cannot spawn enemies"));
            return;
        }
    }

    // Calculate how many enemies we can spawn
    int32 EnemiesToSpawn = FMath::Min(EnemiesPerSpawn, MaxEnemies - GetAliveEnemyCount());

    int32 SuccessfulSpawns = 0;

    for (int32 i = 0; i < EnemiesToSpawn; ++i)
    {
        FVector SpawnLocation = GetRandomSpawnLocation();

        ACC_EnemyCharacter* NewEnemy = SpawnSingleEnemy(SpawnLocation);

        if (NewEnemy)
        {
            SpawnedEnemies.Add(NewEnemy);
            SuccessfulSpawns++;
        }
    }

    if (SuccessfulSpawns > 0)
    {
        CC_LOG_SPAWNER(Log, TEXT("Spawned %d enemies (Total: %d/%d, Interval: %.2fs)"),
            SuccessfulSpawns, GetAliveEnemyCount(), MaxEnemies, CurrentSpawnInterval);
    }
}

ACC_EnemyCharacter* ACC_EnemySpawner::SpawnSingleEnemy(const FVector& Location)
{
    if (!EnemyClass)
    {
        return nullptr;
    }

    FRotator SpawnRotation = FRotator::ZeroRotator;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    ACC_EnemyCharacter* NewEnemy = GetWorld()->SpawnActor<ACC_EnemyCharacter>(
        EnemyClass,
        Location,
        SpawnRotation,
        SpawnParams
    );

    if (NewEnemy)
    {
        CC_LOG_SPAWNER(VeryVerbose, TEXT("Spawned enemy at (%.0f, %.0f, %.0f)"),
            Location.X, Location.Y, Location.Z);
    }
    else
    {
        CC_LOG_SPAWNER(Warning, TEXT("Failed to spawn enemy at location"));
    }

    return NewEnemy;
}

FVector ACC_EnemySpawner::GetRandomSpawnLocation() const
{
    if (!CachedPlayer)
    {
        CC_LOG_SPAWNER(Warning, TEXT("No player for spawn location, using world origin"));
        return FVector::ZeroVector;
    }

    FVector PlayerLocation = CachedPlayer->GetActorLocation();

    // Random angle (360 degrees)
    float Angle = FMath::FRandRange(0.0f, 360.0f);
    float RadAngle = FMath::DegreesToRadians(Angle);

    // Random distance between min and max
    float Distance = FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

    // Calculate offset from player
    FVector Offset;
    Offset.X = FMath::Cos(RadAngle) * Distance;
    Offset.Y = FMath::Sin(RadAngle) * Distance;
    Offset.Z = 0.0f;  // Keep on ground level

    return PlayerLocation + Offset;
}

void ACC_EnemySpawner::CleanupDeadEnemies()
{
    int32 RemovedCount = 0;

    // Remove null and dead enemies from tracking array
    for (int32 i = SpawnedEnemies.Num() - 1; i >= 0; --i)
    {
        if (!SpawnedEnemies[i] || !SpawnedEnemies[i]->IsAlive())
        {
            SpawnedEnemies.RemoveAt(i);
            RemovedCount++;
        }
    }

    if (RemovedCount > 0)
    {
        CC_LOG_SPAWNER(VeryVerbose, TEXT("Cleaned up %d dead enemies (Alive: %d)"),
            RemovedCount, GetAliveEnemyCount());
    }
}

void ACC_EnemySpawner::UpdateSpawnInterval()
{
    if (!bIncreaseSpawnRate)
    {
        return;
    }

    // Calculate new interval based on game time
    float MinutesPlayed = GameTime / 60.0f;
    float IntervalDecrease = MinutesPlayed * SpawnIntervalDecreasePerMinute;

    float NewInterval = FMath::Max(SpawnInterval - IntervalDecrease, MinSpawnInterval);

    // Only update if changed significantly
    if (FMath::Abs(NewInterval - CurrentSpawnInterval) > 0.05f)
    {
        CurrentSpawnInterval = NewInterval;

        // Restart timer with new interval
        if (bIsSpawning)
        {
            GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
            GetWorld()->GetTimerManager().SetTimer(
                SpawnTimerHandle,
                this,
                &ACC_EnemySpawner::SpawnEnemies,
                CurrentSpawnInterval,
                true
            );

            CC_LOG_SPAWNER(VeryVerbose, TEXT("Spawn interval updated to %.2fs (%.1f min elapsed)"),
                CurrentSpawnInterval, MinutesPlayed);
        }
    }
}

int32 ACC_EnemySpawner::GetAliveEnemyCount() const
{
    int32 Count = 0;

    for (ACC_EnemyCharacter* Enemy : SpawnedEnemies)
    {
        if (Enemy && Enemy->IsAlive())
        {
            Count++;
        }
    }

    return Count;
}

bool ACC_EnemySpawner::CanSpawnMore() const
{
    return GetAliveEnemyCount() < MaxEnemies;
}

void ACC_EnemySpawner::FindPlayer()
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    CachedPlayer = Cast<ACC_PlayerCharacter>(PlayerPawn);

    if (CachedPlayer)
    {
        CC_LOG_SPAWNER(Log, TEXT("Found player: %s"), *CachedPlayer->GetName());
    }
    else
    {
        CC_LOG_SPAWNER(Warning, TEXT("Could not find player"));
    }
}
