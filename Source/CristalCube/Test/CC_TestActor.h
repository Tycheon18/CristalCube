// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CC_TestActor.generated.h"

UCLASS()
class CRISTALCUBE_API ACC_TestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACC_TestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCC_TileTrackerComponent* TileTrackerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DirectionChangeInterval = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableDebugLog = true;

	FVector CurrentDirection;
	int32 TickCount = 0;
	float LastActiveTime = 0.0f;

public:

	UFUNCTION()
	void ChangeDirection();

	virtual void SetActorTickEnabled(bool bEnabled) override;

	UFUNCTION()
	void OnTileChanged(int32 OldTileIndex, int32 NewTileIndex, FVector NewPosition);

protected:

	FTimerHandle DirectionChangeTimer;
};
