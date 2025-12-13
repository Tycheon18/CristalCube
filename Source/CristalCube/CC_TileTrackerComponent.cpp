// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_TileTrackerComponent.h"
#include "CC_TileManager.h"
#include "Gameplay/CC_Tile.h"
#include "TimerManager.h"

// Sets default values for this component's properties
UCC_TileTrackerComponent::UCC_TileTrackerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UCC_TileTrackerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	TileManager = ACC_TileManager::Get(GetWorld());

	if (!TileManager)
	{
		UE_LOG(LogTemp, Error, TEXT("[TileTracker] TileManager not found!"));
		return;
	}

	AActor* Owner = GetOwner();
	if (Owner)
	{
		CurrentTileIndex = TileManager->GetTileIndexAtPosition(Owner->GetActorLocation());
		PreviousTileIndex = CurrentTileIndex;

		if (CurrentTileIndex != -1)
		{
			ACC_Tile* InitialTile = TileManager->GetTileByIndex(CurrentTileIndex);
			if (InitialTile)
			{
				InitialTile->RegisterActor(Owner);
			}
		}

		UE_LOG(LogTemp, Log, TEXT("[TileTracker] %s started at Tile %d"),
			*Owner->GetName(), CurrentTileIndex);
	}

	StartTracking();
}

void UCC_TileTrackerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	StopTracking();

	if (TileManager && CurrentTileIndex != -1)
	{
		ACC_Tile* CurrentTile = TileManager->GetTileByIndex(CurrentTileIndex);
		if (CurrentTile && GetOwner())
		{
			CurrentTile->UnregisterActor(GetOwner());
		}
	}
}

// Called every frame
void UCC_TileTrackerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCC_TileTrackerComponent::StartTracking()
{
	if (!TileManager) return;

	if (GetWorld()->GetTimerManager().IsTimerActive(TileCheckTimer))
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(
		TileCheckTimer,
		this,
		&UCC_TileTrackerComponent::CheckTileBoundary,
		CheckInterval,
		true
	);

	UE_LOG(LogTemp, Log, TEXT("[TileTracker] %s tracking started"),
		*GetOwner()->GetName());
}

void UCC_TileTrackerComponent::StopTracking()
{
	if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(TileCheckTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(TileCheckTimer);

		UE_LOG(LogTemp, Log, TEXT("[TileTracker] %s tracking stopped"),
			*GetOwner()->GetName());
	}
}

void UCC_TileTrackerComponent::ForceUpdateTile()
{
	CheckTileBoundary();
}

void UCC_TileTrackerComponent::CheckTileBoundary()
{
	if (!TileManager) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	//UE_LOG(LogTemp, Log, TEXT("[TileTracker] CheckTileBoundary is Called"));

	FVector CurrentPosition = Owner->GetActorLocation();
	int32 NewTileIndex = TileManager->GetTileIndexAtPosition(CurrentPosition);

	if (NewTileIndex != -1 && NewTileIndex != CurrentTileIndex)
	{
		HandleTileTransition(NewTileIndex);
	}

	TileManager->UpdatePlayerPosition(CurrentPosition);
}

void UCC_TileTrackerComponent::HandleTileTransition(int32 NewTileIndex)
{
	if (!TileManager) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	UE_LOG(LogTemp, Log, TEXT("[TileTracker] %s: Tile %d -> %d"),
		*Owner->GetName(), CurrentTileIndex, NewTileIndex);

	if (CurrentTileIndex != -1)
	{
		ACC_Tile* OldTile = TileManager->GetTileByIndex(CurrentTileIndex);
		if (OldTile)
		{
			OldTile->UnregisterActor(Owner);
		}
	}

	ACC_Tile* NewTile = TileManager->GetTileByIndex(NewTileIndex);
	if (NewTile)
	{
		NewTile->RegisterActor(Owner);
	}

	FVector CurrentPosition = Owner->GetActorLocation();
	FVector NewPosition = CurrentPosition;

	if (bEnableWrapAround)
	{
		NewPosition = TileManager->HandleWrapAround(
			CurrentPosition,
			CurrentTileIndex,
			NewTileIndex
		);

		if (bAutoTeleport && !NewPosition.Equals(CurrentPosition, 1.0f))
		{
			Owner->SetActorLocation(NewPosition);

			UE_LOG(LogTemp, Warning, TEXT("[TileTracker] %s teleported to: %s"),
				*Owner->GetName(), *NewPosition.ToString());
		}
	}

	PreviousTileIndex = CurrentTileIndex;
	CurrentTileIndex = NewTileIndex;

	OnCurrentTileChanged.Broadcast(PreviousTileIndex, CurrentTileIndex, NewPosition);
}