// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_Cube.h"
#include "CC_Tile.h"
#include "Components/BoxComponent.h"

// Sets default values
ACC_Cube::ACC_Cube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// 큐브 벽 초기화 (생성자에서)
	InitializeCubeWalls();

}

// Called when the game starts or when spawned
void ACC_Cube::BeginPlay()
{
	Super::BeginPlay();
	
	GenerateCube();

	if (CubeWall_Right)
	{
		CubeWall_Right->OnComponentBeginOverlap.AddDynamic(this, &ACC_Cube::OnCubeWallHit_Right);
	}
	if (CubeWall_Left)
	{
		CubeWall_Left->OnComponentBeginOverlap.AddDynamic(this, &ACC_Cube::OnCubeWallHit_Left);
	}
	if (CubeWall_Top)
	{
		CubeWall_Top->OnComponentBeginOverlap.AddDynamic(this, &ACC_Cube::OnCubeWallHit_Top);
	}
	if (CubeWall_Bottom)
	{
		CubeWall_Bottom->OnComponentBeginOverlap.AddDynamic(this, &ACC_Cube::OnCubeWallHit_Bottom);
	}

	UE_LOG(LogTemp, Warning, TEXT("=== Cube Initialized ==="));
	UE_LOG(LogTemp, Log, TEXT("Cube Size: %.0f x %.0f"), CubeSize, CubeSize);
	UE_LOG(LogTemp, Log, TEXT("Tiles: %d (3x3)"), CubeTiles.Num());
	UE_LOG(LogTemp, Log, TEXT("Cube Walls: 4 (Right, Left, Top, Bottom)"));
}

// Called every frame
void ACC_Cube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACC_Cube::GenerateCube()
{
	if (!TileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Cube: TileClass not set!"));
		return;
	}

	// 기존 타일 제거
	for (ACC_Tile* Tile : CubeTiles)
	{
		if (Tile)
		{
			Tile->Destroy();
		}
	}
	CubeTiles.Empty();

	// 3x3 타일 생성
	for (int32 Row = 0; Row < 3; ++Row)
	{
		for (int32 Col = 0; Col < 3; ++Col)
		{
			int32 Index = Row * 3 + Col;

			// 타일 위치 계산
			float X = (Col - 1) * TileSize;  // -2000, 0, 2000
			float Y = (Row - 1) * TileSize;  // -2000, 0, 2000
			FVector TileCenter = GetActorLocation() + FVector(X, Y, 0);

			// 타일 생성
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			ACC_Tile* NewTile = GetWorld()->SpawnActor<ACC_Tile>(
				TileClass,
				TileCenter,
				FRotator::ZeroRotator,
				SpawnParams
			);

			if (NewTile)
			{
				NewTile->InitializeTile(Index, TileCenter, TileSize);
				CubeTiles.Add(NewTile);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Cube: Generated 9 tiles in 3x3 grid"));
}

ACC_Tile* ACC_Cube::GetTileAt(int32 Index) const
{
	if (Index >= 0 && Index < CubeTiles.Num())
	{
		return CubeTiles[Index];
	}
	return nullptr;
}

int32 ACC_Cube::GetTileIndexAtPosition(const FVector& Position) const
{
	for (int32 i = 0; i < CubeTiles.Num(); ++i)
	{
		if (CubeTiles[i] && CubeTiles[i]->IsPositionInTile(Position))
		{
			return i;
		}
	}
	return -1;
}

void ACC_Cube::ActivateTilesAroundPlayer(int32 PlayerTileIndex)
{
	// 현재는 플레이어 타일만 활성화
	ActivateOnlyTile(PlayerTileIndex);
	CurrentPlayerTileIndex = PlayerTileIndex;
}

void ACC_Cube::ActivateOnlyTile(int32 TileIndex)
{
	for (int32 i = 0; i < CubeTiles.Num(); ++i)
	{
		if (CubeTiles[i])
		{
			if (i == TileIndex)
			{
				CubeTiles[i]->ActivateTile();
			}
			else
			{
				CubeTiles[i]->DeactivateTile();
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Cube: Activated tile %d only"), TileIndex);
}

void ACC_Cube::ActivateAllTiles()
{
	for (ACC_Tile* Tile : CubeTiles)
	{
		if (Tile)
		{
			Tile->ActivateTile();
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Cube: All tiles activated"));
}

void ACC_Cube::DeactivateAllTiles()
{
	for (ACC_Tile* Tile : CubeTiles)
	{
		if (Tile)
		{
			Tile->DeactivateTile();
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Cube: All tiles deactivated"));
}

// ========== 큐브 벽 충돌 이벤트 ==========

void ACC_Cube::OnCubeWallHit_Right(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	// 오른쪽 벽 → 왼쪽으로 순환
	FVector CurrentPos = OtherActor->GetActorLocation();
	FVector NewPos = CurrentPos - FVector(CubeSize, 0, 0);
	OtherActor->SetActorLocation(NewPos);

	UE_LOG(LogTemp, Warning, TEXT("[CubeWall] %s: Right → Left (X: %.1f → %.1f)"),
		*OtherActor->GetName(), CurrentPos.X, NewPos.X);
}

void ACC_Cube::OnCubeWallHit_Left(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	// 왼쪽 벽 → 오른쪽으로 순환
	FVector CurrentPos = OtherActor->GetActorLocation();
	FVector NewPos = CurrentPos + FVector(CubeSize, 0, 0);
	OtherActor->SetActorLocation(NewPos);

	UE_LOG(LogTemp, Warning, TEXT("[CubeWall] %s: Left → Right (X: %.1f → %.1f)"),
		*OtherActor->GetName(), CurrentPos.X, NewPos.X);
}

void ACC_Cube::OnCubeWallHit_Top(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	// 위쪽 벽 → 아래로 순환
	FVector CurrentPos = OtherActor->GetActorLocation();
	FVector NewPos = CurrentPos - FVector(0, CubeSize, 0);
	OtherActor->SetActorLocation(NewPos);

	UE_LOG(LogTemp, Warning, TEXT("[CubeWall] %s: Top → Bottom (Y: %.1f → %.1f)"),
		*OtherActor->GetName(), CurrentPos.Y, NewPos.Y);
}

void ACC_Cube::OnCubeWallHit_Bottom(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	// 아래쪽 벽 → 위로 순환
	FVector CurrentPos = OtherActor->GetActorLocation();
	FVector NewPos = CurrentPos + FVector(0, CubeSize, 0);
	OtherActor->SetActorLocation(NewPos);

	UE_LOG(LogTemp, Warning, TEXT("[CubeWall] %s: Bottom → Top (Y: %.1f → %.1f)"),
		*OtherActor->GetName(), CurrentPos.Y, NewPos.Y);
}

void ACC_Cube::IndexToRowCol(int32 Index, int32& OutRow, int32& OutCol) const
{
	OutRow = Index / 3;
	OutCol = Index % 3;
}

int32 ACC_Cube::RowColToIndex(int32 Row, int32 Col) const
{
	return Row * 3 + Col;
}

void ACC_Cube::InitializeCubeWalls()
{
	float HalfCube = CubeSize / 2.0f;
	float HalfHeight = CubeHeight / 2.0f;
	float HalfThickness = WallThickness / 2.0f;

	// 오른쪽 벽 (X+) - 빨강
	CubeWall_Right = CreateDefaultSubobject<UBoxComponent>(TEXT("CubeWall_Right"));
	CubeWall_Right->SetupAttachment(RootComponent);
	CubeWall_Right->SetRelativeLocation(FVector(HalfCube + HalfThickness, 0, HalfHeight));
	CubeWall_Right->SetBoxExtent(FVector(HalfThickness, HalfCube, HalfHeight));
	CubeWall_Right->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CubeWall_Right->SetCollisionResponseToAllChannels(ECR_Ignore);
	CubeWall_Right->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CubeWall_Right->SetGenerateOverlapEvents(true);
	CubeWall_Right->ShapeColor = FColor::Red;

	// 왼쪽 벽 (X-) - 파랑
	CubeWall_Left = CreateDefaultSubobject<UBoxComponent>(TEXT("CubeWall_Left"));
	CubeWall_Left->SetupAttachment(RootComponent);
	CubeWall_Left->SetRelativeLocation(FVector(-HalfCube - HalfThickness, 0, HalfHeight));
	CubeWall_Left->SetBoxExtent(FVector(HalfThickness, HalfCube, HalfHeight));
	CubeWall_Left->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CubeWall_Left->SetCollisionResponseToAllChannels(ECR_Ignore);
	CubeWall_Left->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CubeWall_Left->SetGenerateOverlapEvents(true);
	CubeWall_Left->ShapeColor = FColor::Blue;

	// 위쪽 벽 (Y+) - 초록
	CubeWall_Top = CreateDefaultSubobject<UBoxComponent>(TEXT("CubeWall_Top"));
	CubeWall_Top->SetupAttachment(RootComponent);
	CubeWall_Top->SetRelativeLocation(FVector(0, HalfCube + HalfThickness, HalfHeight));
	CubeWall_Top->SetBoxExtent(FVector(HalfCube, HalfThickness, HalfHeight));
	CubeWall_Top->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CubeWall_Top->SetCollisionResponseToAllChannels(ECR_Ignore);
	CubeWall_Top->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CubeWall_Top->SetGenerateOverlapEvents(true);
	CubeWall_Top->ShapeColor = FColor::Green;

	// 아래쪽 벽 (Y-) - 노랑
	CubeWall_Bottom = CreateDefaultSubobject<UBoxComponent>(TEXT("CubeWall_Bottom"));
	CubeWall_Bottom->SetupAttachment(RootComponent);
	CubeWall_Bottom->SetRelativeLocation(FVector(0, -HalfCube - HalfThickness, HalfHeight));
	CubeWall_Bottom->SetBoxExtent(FVector(HalfCube, HalfThickness, HalfHeight));
	CubeWall_Bottom->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CubeWall_Bottom->SetCollisionResponseToAllChannels(ECR_Ignore);
	CubeWall_Bottom->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CubeWall_Bottom->SetGenerateOverlapEvents(true);
	CubeWall_Bottom->ShapeColor = FColor::Yellow;
}