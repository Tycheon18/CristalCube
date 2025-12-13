// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CC_SearchingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRISTALCUBE_API UCC_SearchingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCC_SearchingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	class ACC_EnemyManager* EnemyManager;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	UFUNCTION(BlueprintCallable, Category = "Searching|Single")
	AActor* FindNearestEnemy(FVector SearchOrigin, float SearchRadius = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Searching|Single")
	AActor* FindNearestEnemyInDirection(
		FVector SearchOrigin, 
		FVector Direction, 
		float MaxAngle = 45.0f, 
		float SearchRadius = 1000.0f
	);

	UFUNCTION(BlueprintCallable, Category = "Searching|Multiple")
	TArray<AActor*> FindRandomEnemies(
		FVector SearchOrigin,
		float SearchRadius,
		int32 Count
	);

	UFUNCTION(BlueprintCallable, Category = "Searching|Multiple")
	TArray<AActor*> FindNearestEnemies(
		FVector SearchOrigin,
		float SearchRadius,
		int32 Count
	);

	UFUNCTION(BlueprintCallable, Category = "Searching|Area")
	TArray<AActor*> GetEnemiesInSphere(FVector Center, float Radius);

	UFUNCTION(BlueprintCallable, Category = "Searching|Area")
	TArray<AActor*> GetEnemiesInCone(
		FVector Origin,
		FVector Direction,
		float Range,
		float Angle
	);

	UFUNCTION(BlueprintCallable, Category = "Searching|Area")
	TArray<AActor*> GetEnemiesInBox(
		FVector Center,
		FVector HalfExtents,
		FRotator Rotation = FRotator::ZeroRotator
	);

	UFUNCTION(BlueprintCallable, Category = "Searching|Utility")
	TArray<FRotator> CalculateSpreadRotations(
		FRotator BaseRotation,
		int32 Count,
		float SpreadAngle
	);

	UFUNCTION(BlueprintCallable, Category = "Searching|Utility")
	TArray<FVector> CalculateCirclePositions(
		FVector Center,
		float Radius,
		int32 Count,
		float StartAngle = 0.0f
	);

public:

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bEnableDebugVisualization = false;

	void DebugDrawSphere(FVector Center, float Radius, FColor Color, float Duration = 1.0f);
	void DebugDrawCone(FVector Origin, FVector Direction, float Range, float Angle, FColor Color, float Duration = 1.0f);
};
