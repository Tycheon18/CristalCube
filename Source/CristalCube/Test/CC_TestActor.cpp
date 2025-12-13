// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_TestActor.h"
#include "../CC_TileTrackerComponent.h"
#include "TimerManager.h"


// Sets default values
ACC_TestActor::ACC_TestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TileTrackerComponent = CreateDefaultSubobject<UCC_TileTrackerComponent>(TEXT("TileTrackerComponent"));

	TileTrackerComponent->bEnableWrapAround = false;
	TileTrackerComponent->bAutoTeleport = false;
	TileTrackerComponent->CheckInterval = 0.2f;
}

// Called when the game starts or when spawned
void ACC_TestActor::BeginPlay()
{
	Super::BeginPlay();
	
	ChangeDirection();

	GetWorld()->GetTimerManager().SetTimer(
		DirectionChangeTimer,
		this,
		&ACC_TestActor::ChangeDirection,
		DirectionChangeInterval,
		true
	);

	if (TileTrackerComponent)
	{
		TileTrackerComponent->OnCurrentTileChanged.AddDynamic(this, &ACC_TestActor::OnTileChanged);
	}

	if (bEnableDebugLog)
	{
		UE_LOG(LogTemp, Log, TEXT("[TestActor] %s spawned at %s"),
			*GetName(), *GetActorLocation().ToString());
	}
}

// Called every frame
void ACC_TestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickCount++;

	FVector NewLocation = GetActorLocation() + (CurrentDirection * MoveSpeed * DeltaTime);
	SetActorLocation(NewLocation);

	if (bEnableDebugLog && TickCount % 600 == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[TestActor] %s still ticking (Count: %d, Tile: %d)"),
			*GetName(), TickCount, TileTrackerComponent->GetCurrentTileIndex());
	}
}

void ACC_TestActor::ChangeDirection()
{
	float Angle = FMath::FRandRange(0.0f, 360.0f);
	CurrentDirection = FVector(
		FMath::Cos(FMath::DegreesToRadians(Angle)),
		FMath::Sin(FMath::DegreesToRadians(Angle)),
		0.0f
	);

	if (bEnableDebugLog)
	{
		UE_LOG(LogTemp, Log, TEXT("[TestActor] %s changed direction to %s"),
			*GetName(), *CurrentDirection.ToString());
	}
}

void ACC_TestActor::SetActorTickEnabled(bool bEnabled)
{
	Super::SetActorTickEnabled(bEnabled);

	if (bEnableDebugLog)
	{
		if (bEnabled)
		{
			LastActiveTime = GetWorld()->GetTimeSeconds();
			UE_LOG(LogTemp, Warning, TEXT("[TestActor] %s ACTIVATED (Tile: %d)"),
				*GetName(), TileTrackerComponent->GetCurrentTileIndex());
		}
		else
		{
			float InactiveTime = GetWorld()->GetTimeSeconds() - LastActiveTime;
			UE_LOG(LogTemp, Warning, TEXT("[TestActor] %s DEACTIVATED (Active time: %.1fs, Tile: %d)"),
				*GetName(), InactiveTime, TileTrackerComponent->GetCurrentTileIndex());
		}
	}

	if (MeshComponent)
	{
		FLinearColor Color = bEnabled ? FLinearColor::Red : FLinearColor::Gray;
		MeshComponent->SetVectorParameterValueOnMaterials(FName("Color"), FVector(Color));
	}
}

void ACC_TestActor::OnTileChanged(int32 OldTileIndex, int32 NewTileIndex, FVector NewPosition)
{
	if (bEnableDebugLog)
	{
		UE_LOG(LogTemp, Log, TEXT("[TestActor] %s moved from Tile %d to Tile %d"),
			*GetName(), OldTileIndex, NewTileIndex);
	}
}

