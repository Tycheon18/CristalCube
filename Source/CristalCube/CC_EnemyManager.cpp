// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_EnemyManager.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

ACC_EnemyManager* ACC_EnemyManager::Instance = nullptr;

// Sets default values
ACC_EnemyManager::ACC_EnemyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = CacheUpdateInterval;
}

// Called when the game starts or when spawned
void ACC_EnemyManager::BeginPlay()
{
	Super::BeginPlay();

	Instance = this;

	UE_LOG(LogTemp, Log, TEXT("[ENEMY MANAGER] Initialized"));
	
}

// Called every frame
void ACC_EnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeSinceLastUpdate += DeltaTime;

	if (TimeSinceLastUpdate >= CacheUpdateInterval)
	{
		UpdateNearestEnemyCache();
		TimeSinceLastUpdate = 0.0f;
	}

}

ACC_EnemyManager* ACC_EnemyManager::Get(const UObject* WorldContextObject)
{
    if (!Instance && WorldContextObject)
    {
        UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
        if (World)
        {
            // Try to find existing instance
            for (TActorIterator<ACC_EnemyManager> It(World); It; ++It)
            {
                Instance = *It;
                break;
            }

            // Create if not found
            if (!Instance)
            {
                FActorSpawnParameters SpawnParams;
                SpawnParams.Name = FName("EnemyManager");
                Instance = World->SpawnActor<ACC_EnemyManager>(ACC_EnemyManager::StaticClass(), SpawnParams);
                UE_LOG(LogTemp, Warning, TEXT("[ENEMY MANAGER] Auto-created instance"));
            }
        }
    }

    return Instance;
}

void ACC_EnemyManager::RegisterEnemy(AActor* Enemy)
{
    if (!Enemy || ActiveEnemies.Contains(Enemy))
    {
        return;
    }

    ActiveEnemies.Add(Enemy);

    UE_LOG(LogTemp, VeryVerbose, TEXT("[ENEMY MANAGER] Registered enemy (Total: %d)"), ActiveEnemies.Num());

}

void ACC_EnemyManager::UnregisterEnemy(AActor* Enemy)
{
    if (!Enemy)
    {
        return;
    }

    ActiveEnemies.Remove(Enemy);
    NearestEnemyCache.Remove(Enemy);

    UE_LOG(LogTemp, VeryVerbose, TEXT("[ENEMY MANAGER] Unregistered enemy (Total: %d)"), ActiveEnemies.Num());

}

AActor* ACC_EnemyManager::GetNearestEnemy(const FVector& Location, float MaxRadius)
{
    if (ActiveEnemies.Num() == 0)
    {
        return nullptr;
    }

    AActor* NearestEnemy = nullptr;
    float NearestDistSq = MaxRadius * MaxRadius;

    for (AActor* Enemy : ActiveEnemies)
    {
        if (!Enemy || !IsValid(Enemy))
        {
            continue;
        }

        float DistSq = FVector::DistSquared(Location, Enemy->GetActorLocation());

        if (DistSq < NearestDistSq)
        {
            NearestDistSq = DistSq;
            NearestEnemy = Enemy;
        }
    }

    return NearestEnemy;
}

TArray<AActor*> ACC_EnemyManager::GetEnemiesInRadius(const FVector& Location, float Radius)
{
    TArray<AActor*> Result;
    float RadiusSq = Radius * Radius;

    for (AActor* Enemy : ActiveEnemies)
    {
        if (!Enemy || !IsValid(Enemy))
        {
            continue;
        }

        float DistSq = FVector::DistSquared(Location, Enemy->GetActorLocation());

        if (DistSq <= RadiusSq)
        {
            Result.Add(Enemy);
        }
    }

    return Result;
}

void ACC_EnemyManager::UpdateNearestEnemyCache()
{
    // Remove invalid enemies
    ActiveEnemies.RemoveAll([](AActor* Enemy) {
        return !IsValid(Enemy);
        });
}
