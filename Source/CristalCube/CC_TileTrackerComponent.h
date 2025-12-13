// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CC_TileTrackerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnCurrentTileChanged,
	int32, OldTileIndex,
	int32, NewTileIndex,
	FVector, NewPosition
);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRISTALCUBE_API UCC_TileTrackerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCC_TileTrackerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Tracker")
	float CheckInterval = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Tracker")
	bool bEnableWrapAround = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Tracker")
	bool bAutoTeleport = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile Tracker")
	int32 CurrentTileIndex = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile Tracker")
	int32 PreviousTileIndex = -1;

	UPROPERTY(BlueprintAssignable, Category = "Tile Tracker")
	FOnCurrentTileChanged OnCurrentTileChanged;

	UFUNCTION(BlueprintCallable, Category = "Tile Tracker")
	int32 GetCurrentTileIndex() const { return CurrentTileIndex; }

	UFUNCTION(BlueprintCallable, Category = "Tile Tracker")
	void StartTracking();

	UFUNCTION(BlueprintCallable, Category = "Tile Tracker")
	void StopTracking();

	UFUNCTION(BlueprintCallable, Category = "Tile Tracker")
	void ForceUpdateTile();

protected:

	UPROPERTY()
	class ACC_TileManager* TileManager;

	FTimerHandle TileCheckTimer;
	
	void CheckTileBoundary();

	void HandleTileTransition(int32 NewTileIndex);
};
