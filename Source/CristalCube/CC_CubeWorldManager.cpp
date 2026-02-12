// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_CubeWorldManager.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Gameplay/CC_Cube.h"

// Sets default values
ACC_CubeWorldManager::ACC_CubeWorldManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACC_CubeWorldManager::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeSystem();
}

// Called every frame
void ACC_CubeWorldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bAutoTest)
	{
		DrawAllCubes();
	}

}

void ACC_CubeWorldManager::InitializeSystem()
{
	UE_LOG(LogTemp, Warning, TEXT("=============================================="));
	UE_LOG(LogTemp, Warning, TEXT("   CUBE WORLD MANAGER - INITIALIZATION"));
	UE_LOG(LogTemp, Warning, TEXT("=============================================="));

	InitializeCubeGrid();
	
	CurrentCubeCoord = FIntPoint(1,1);
	ActiveCube = FindOrSpawnCube(CurrentCubeCoord);

	if (ActiveCube)
	{
		ActiveCube->Unfreeze();
		UE_LOG(LogTemp, Log, TEXT("[Manager] Active cube set to: %d, %d"),
			CurrentCubeCoord.X, CurrentCubeCoord.Y);
	}

	MovePlayerToCube(CurrentCubeCoord);

	UE_LOG(LogTemp, Warning, TEXT("[Manager] System initialized successfully!"));
	UE_LOG(LogTemp, Warning, TEXT("=============================================="));
}

void ACC_CubeWorldManager::InitializeCubeGrid()
{
	CubeGrid.Empty();

	for (int32 X = 0; X < GridSize; X++)
	{
		for (int32 Y = 0; Y < GridSize; Y++)
		{
			FCubeData NewCubeData;
			NewCubeData.Coordinate = FIntPoint(X, Y);
			NewCubeData.State = ECubeState::Unloaded;
			CubeGrid.Add(FIntPoint(X, Y), NewCubeData);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[Manager] Initialized %d cube data entries"), CubeGrid.Num());
}

ACC_Cube* ACC_CubeWorldManager::SpawnCube(FIntPoint Coordinate)
{
	ACC_Cube* ExistingCube = FindCube(Coordinate);
	if (ExistingCube)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Manager] Cube (%d, %d) already exists"),
			Coordinate.X, Coordinate.Y);
		return ExistingCube;
	}

	// ť�� Ŭ���� Ȯ��
	if (!CubeClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[Manager] CubeClass is not set!"));
		return nullptr;
	}

	// Spawn
	FVector SpawnLocation(-Coordinate.X * CubeSize, Coordinate.Y * CubeSize, 0.0f);
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	ACC_Cube* NewCube = GetWorld()->SpawnActor<ACC_Cube>(CubeClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

	if (NewCube)
	{
		NewCube->CubeSize = CubeSize;
		NewCube->InitializeCube(Coordinate);
		
		LoadedCubes.Add(NewCube);

		// ������ ������Ʈ
		if (CubeGrid.Contains(Coordinate))
		{
			CubeGrid[Coordinate].State = ECubeState::Active;
		}

		UE_LOG(LogTemp, Log, TEXT("[Manager] Spawned cube at (%d, %d) - Location: %s"),
			Coordinate.X, Coordinate.Y, *SpawnLocation.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Manager] Failed to spawn cube at (%d, %d)"),
			Coordinate.X, Coordinate.Y);
	}

	return NewCube;
}

void ACC_CubeWorldManager::DespawnCube(FIntPoint Coordinate)
{
	ACC_Cube* Cube = FindCube(Coordinate);
	if (!Cube)
		return;

	LoadedCubes.Remove(Cube);
	Cube->Destroy();

	if (CubeGrid.Contains(Coordinate))
	{
		CubeGrid[Coordinate].State = ECubeState::Unloaded;
	}

	UE_LOG(LogTemp, Log, TEXT("[Manager] Despawned cube at (%d, %d)"), Coordinate.X, Coordinate.Y);
}

ACC_Cube* ACC_CubeWorldManager::FindOrSpawnCube(FIntPoint Coordinate)
{
	ACC_Cube* Cube = FindCube(Coordinate);
	if (Cube)
		return Cube;

	return SpawnCube(Coordinate);
}

ACC_Cube* ACC_CubeWorldManager::FindCube(FIntPoint CubeCoord) const
{
	for (ACC_Cube* Cube : LoadedCubes)
	{
		if (Cube && Cube->CubeCoordinate == CubeCoord)
		{
			return Cube;
		}
	}
	return nullptr;
}

void ACC_CubeWorldManager::RequestTransition(EBoundaryDirection Direction)
{
	if (bIsTransitioning)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Manager] Already transitioning, ignoring request"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("=============================================="));
	UE_LOG(LogTemp, Warning, TEXT("   CUBE TRANSITION STARTED"));
	UE_LOG(LogTemp, Warning, TEXT("   Direction: %s"), *UEnum::GetValueAsString(Direction));
	UE_LOG(LogTemp, Warning, TEXT("=============================================="));

	bIsTransitioning = true;
	LastTransitionDirection = Direction;

	// ���� ť�� ��ǥ ���
	FIntPoint NextCoord = GetNextCubeCoord(CurrentCubeCoord, Direction);

	// ��� ��ȯ (Fade ����)
	PerformTransition(NextCoord);

	bIsTransitioning = false;

	UE_LOG(LogTemp, Warning, TEXT("[Manager] Transition complete!"));
}

FIntPoint ACC_CubeWorldManager::GetNextCubeCoord(FIntPoint Current, EBoundaryDirection Direction) const
{
	FIntPoint Next = Current;

	switch (Direction)
	{
	case EBoundaryDirection::Right:
		Next.Y = (Current.Y + 1) % GridSize;
		break;

	case EBoundaryDirection::Left:
		Next.Y = (Current.Y - 1 + GridSize) % GridSize;
		break;

	case EBoundaryDirection::Up:
		Next.X = (Current.X - 1 + GridSize) % GridSize;
		break;

	case EBoundaryDirection::Down:
		Next.X = (Current.X + 1) % GridSize;
		break;
	}

	UE_LOG(LogTemp, Log, TEXT("[Manager] Next cube coord: (%d, %d) -> (%d, %d)"),
		Current.X, Current.Y, Next.X, Next.Y);

	return Next;
}

void ACC_CubeWorldManager::PerformTransition(FIntPoint NextCoord)
{
	UE_LOG(LogTemp, Log, TEXT("[Manager] Performing transition to (%d, %d)"), NextCoord.X, NextCoord.Y);

	// 1. ���� ť�� Freeze
	if (ActiveCube)
	{
		ActiveCube->Freeze();
	}

	// 2. �� ť�� Spawn �Ǵ� ã��
	ACC_Cube* NextCube = FindOrSpawnCube(NextCoord);
	if (!NextCube)
	{
		UE_LOG(LogTemp, Error, TEXT("[Manager] Failed to get next cube!"));
		return;
	}

	// 3. �� ť�� Unfreeze
	NextCube->Unfreeze();

	// 4. �÷��̾� �̵�
	FVector NewPlayerPos = CalculatePlayerPositionInCube(NextCube, LastTransitionDirection);
	ACharacter* Player = GetPlayerCharacter();
	if (Player)
	{
		Player->SetActorLocation(NewPlayerPos);
		UE_LOG(LogTemp, Log, TEXT("[Manager] Player moved to: %s"), *NewPlayerPos.ToString());
	}

	// 5. Active ť�� ��ü
	ActiveCube = NextCube;
	CurrentCubeCoord = NextCoord;

	// 6. �̺�Ʈ �߻�
	OnCubeTransition.Broadcast(NextCoord);

	UE_LOG(LogTemp, Log, TEXT("[Manager] Transition performed successfully"));
}

FVector ACC_CubeWorldManager::CalculatePlayerPositionInCube(ACC_Cube* TargetCube, EBoundaryDirection FromDirection) const
{
	if (!TargetCube)
		return FVector::ZeroVector;

	FVector CubeCenter = TargetCube->GetCubeCenter();
	float Offset = (CubeSize / 2.0f) - 500.0f; // ��迡�� �ణ ����

	// �ݴ������� ����
	switch (FromDirection)
	{
	case EBoundaryDirection::Right:
		return CubeCenter + FVector(0, Offset, 200); // ���ʿ��� ����

	case EBoundaryDirection::Left:
		return CubeCenter + FVector(0, -Offset, 200); // �����ʿ��� ����

	case EBoundaryDirection::Up:
		return CubeCenter + FVector(Offset, 0, 200); // �Ʒ����� ����

	case EBoundaryDirection::Down:
		return CubeCenter + FVector(-Offset, 0, 200); // ������ ����
	}

	return CubeCenter;
}

ACharacter* ACC_CubeWorldManager::GetPlayerCharacter() const
{
	return UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void ACC_CubeWorldManager::MovePlayerToCube(FIntPoint Coordinate)
{
	ACC_Cube* Cube = FindCube(Coordinate);
	if (!Cube)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Manager] Cannot move player - cube not found at (%d, %d)"),
			Coordinate.X, Coordinate.Y);
		return;
	}

	ACharacter* Player = GetPlayerCharacter();

	if (Player)
	{
		FVector CubeCenter = Cube->GetCubeCenter();
		Player->SetActorLocation(CubeCenter + FVector(0, 0, 300));

		UE_LOG(LogTemp, Log, TEXT("[Manager] Moved player to cube (%d, %d) at %s"),
			Coordinate.X, Coordinate.Y, *CubeCenter.ToString());
	}
}

void ACC_CubeWorldManager::PrintDebugInfo()
{
	UE_LOG(LogTemp, Warning, TEXT("=============================================="));
	UE_LOG(LogTemp, Warning, TEXT("   CUBE WORLD DEBUG INFO"));
	UE_LOG(LogTemp, Warning, TEXT("=============================================="));
	UE_LOG(LogTemp, Warning, TEXT("Grid Size: %d x %d"), GridSize, GridSize);
	UE_LOG(LogTemp, Warning, TEXT("Current Cube: (%d, %d)"), CurrentCubeCoord.X, CurrentCubeCoord.Y);
	UE_LOG(LogTemp, Warning, TEXT("Loaded Cubes: %d"), LoadedCubes.Num());
	UE_LOG(LogTemp, Warning, TEXT("Transitioning: %s"), bIsTransitioning ? TEXT("Yes") : TEXT("No"));

	if (ActiveCube)
	{
		UE_LOG(LogTemp, Warning, TEXT("Active Cube Actors: %d"), ActiveCube->ManagedActors.Num());
	}

	UE_LOG(LogTemp, Warning, TEXT("=============================================="));
}

void ACC_CubeWorldManager::DrawAllCubes()
{
	UE_LOG(LogTemp, Log, TEXT("[Manager] Drawing debug info for all loaded cubes..."));

	for (ACC_Cube* Cube : LoadedCubes)
	{
		if (Cube)
		{
			UE_LOG(LogTemp, Log, TEXT("[Manager] Drawing debug info for cube (%d, %d)"), Cube->CubeCoordinate.X, Cube->CubeCoordinate.Y);
			Cube->DrawDebugInfo();
		}
	}
}