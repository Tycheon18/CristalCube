// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_Cube.h"
#include "CC_Tile.h"
#include "CC_Freezable.h"
#include "../CC_CubeWorldManager.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Components/BoxComponent.h"


// Sets default values
ACC_Cube::ACC_Cube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Floor Mesh (시각적)
	FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
	FloorMesh->SetupAttachment(RootComponent);
	FloorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CubeState = ECubeState::Unloaded;
}

// Called when the game starts or when spawned
void ACC_Cube::BeginPlay()
{
	Super::BeginPlay();
	
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

void ACC_Cube::InitializeCube(FIntPoint Coordinate)
{
	CubeCoordinate = Coordinate;
	CubeState = ECubeState::Active;

	// 큐브 위치 설정 (월드 좌표)
	FVector CubeWorldLocation(
		-Coordinate.X * CubeSize,
		Coordinate.Y * CubeSize,
		0.0f
	);
	SetActorLocation(CubeWorldLocation);

	// Floor 크기 설정
	if (FloorMesh)
	{
		FloorMesh->SetWorldScale3D(FVector(CubeSize / 100.0f, CubeSize / 100.0f, 1.0f));
	}

	// 경계 트리거 생성
	CreateBoundaryTriggers();

	UE_LOG(LogTemp, Log, TEXT("[Cube] Initialized at (%d, %d) - Location: %s"),
		Coordinate.X, Coordinate.Y, *CubeWorldLocation.ToString());
}

void ACC_Cube::CreateBoundaryTriggers()
{
	BoundaryTriggers.Empty();
	BoundaryDirectionMap.Empty();

	float HalfSize = CubeSize / 2.0f;
	float TriggerThickness = 100.0f;
	float TriggerHeight = 500.0f;

	struct FBoundaryInfo
	{
		EBoundaryDirection Direction;
		FVector Location;
		FVector Extent;
		FName Name;
		FLinearColor Color;
	};

	TArray<FBoundaryInfo> Boundaries = {
	{ EBoundaryDirection::Up, FVector(HalfSize, 0, 0), FVector(TriggerThickness, HalfSize, TriggerHeight), TEXT("UpBoundary"), FLinearColor::Red },
	{ EBoundaryDirection::Down, FVector(-HalfSize, 0, 0), FVector(TriggerThickness, HalfSize, TriggerHeight), TEXT("DownBoundary"), FLinearColor::Blue },
	{ EBoundaryDirection::Left, FVector(0 , -HalfSize, 0), FVector(HalfSize, TriggerThickness, TriggerHeight), TEXT("LeftBoundary"), FLinearColor::Green },
	{ EBoundaryDirection::Right, FVector(0 , HalfSize, 0), FVector(HalfSize, TriggerThickness, TriggerHeight), TEXT("RightBoundary"), FLinearColor::Yellow }
	};

	for (const FBoundaryInfo& Info : Boundaries)
	{
		UBoxComponent* Trigger = NewObject<UBoxComponent>(this, Info.Name);
		if (Trigger)
		{
			Trigger->RegisterComponent();
			Trigger->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			Trigger->SetRelativeLocation(Info.Location);
			Trigger->SetBoxExtent(Info.Extent);
			Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
			Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
			Trigger->SetGenerateOverlapEvents(true);
			Trigger->ShapeColor = Info.Color.ToFColor(true);

			// Overlap 이벤트 바인딩
			Trigger->OnComponentBeginOverlap.AddDynamic(this, &ACC_Cube::OnBoundaryOverlap);

			BoundaryTriggers.Add(Trigger);
			BoundaryDirectionMap.Add(Trigger, Info.Direction);

			UE_LOG(LogTemp, Log, TEXT("[Cube] Created boundary: %s at %s"),
				*Info.Name.ToString(), *Info.Location.ToString());
		}
	}
}

void ACC_Cube::RegisterActor(AActor* Actor)
{
	if (!Actor || ManagedActors.Contains(Actor))
		return;

	ManagedActors.Add(Actor);

	UE_LOG(LogTemp, Log, TEXT("[Cube %d,%d] Registered actor: %s (Total: %d)"),
		CubeCoordinate.X, CubeCoordinate.Y, *Actor->GetName(), ManagedActors.Num());
}

void ACC_Cube::UnregisterActor(AActor* Actor)
{
	if (!Actor)
		return;

	ManagedActors.Remove(Actor);

	UE_LOG(LogTemp, Log, TEXT("[Cube %d,%d] Unregistered actor: %s (Total: %d)"),
		CubeCoordinate.X, CubeCoordinate.Y, *Actor->GetName(), ManagedActors.Num());
}

bool ACC_Cube::IsActorInCube(AActor* Actor) const
{
	if (!Actor)
		return false;

	FVector ActorLocation = Actor->GetActorLocation();
	FBox CubeBounds = GetCubeBounds();

	return CubeBounds.IsInside(ActorLocation);
}

void ACC_Cube::Freeze()
{
	if (CubeState == ECubeState::Frozen)
		return;

	CubeState = ECubeState::Frozen;

	// 큐브 자체 비활성화
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);

	// 소속 Actor들 Freeze
	for (AActor* Actor : ManagedActors)
	{
		if (!Actor || Actor->IsPendingKillPending())
			continue;

		// 기본 비활성화
		Actor->SetActorHiddenInGame(true);
		Actor->SetActorTickEnabled(false);

		// Freezable 인터페이스 구현 Actor
		if (Actor->GetClass()->ImplementsInterface(UCC_Freezable::StaticClass()))
		{
			ICC_Freezable::Execute_Freeze(Actor);
		}
		else
		{
			// 기본 Freeze 처리
			// Character인 경우
			if (ACharacter* Character = Cast<ACharacter>(Actor))
			{
				// AI 정지
				if (AAIController* AI = Cast<AAIController>(Character->GetController()))
				{
					AI->StopMovement();
					if (AI->BrainComponent)
					{
						AI->BrainComponent->StopLogic("Frozen");
					}
				}

				// 시간 정지
				Character->CustomTimeDilation = 0.0f;

				// 애니메이션 멈춤
				if (USkeletalMeshComponent* Mesh = Character->GetMesh())
				{
					Mesh->bPauseAnims = true;
				}
			}

			// Physics 정지
			TArray<UActorComponent*> Components;
			Actor->GetComponents(UPrimitiveComponent::StaticClass(), Components);
			for (UActorComponent* Comp : Components)
			{
				if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp))
				{
					if (Prim->IsSimulatingPhysics())
					{
						Prim->SetSimulatePhysics(false);
					}
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[Cube %d,%d] FROZEN (%d actors)"),
		CubeCoordinate.X, CubeCoordinate.Y, ManagedActors.Num());
}

void ACC_Cube::Unfreeze()
{
	if (CubeState != ECubeState::Frozen)
		return;

	CubeState = ECubeState::Active;

	// 큐브 자체 활성화
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);

	// 소속 Actor들 Unfreeze
	for (AActor* Actor : ManagedActors)
	{
		if (!Actor || Actor->IsPendingKillPending())
			continue;

		// 기본 활성화
		Actor->SetActorHiddenInGame(false);
		Actor->SetActorTickEnabled(true);

		// Freezable 인터페이스 구현 Actor
		if (Actor->GetClass()->ImplementsInterface(UCC_Freezable::StaticClass()))
		{
			ICC_Freezable::Execute_Unfreeze(Actor);
		}
		else
		{
			// 기본 Unfreeze 처리
			if (ACharacter* Character = Cast<ACharacter>(Actor))
			{
				// AI 재개
				if (AAIController* AI = Cast<AAIController>(Character->GetController()))
				{
					if (AI->BrainComponent)
					{
						AI->BrainComponent->RestartLogic();
					}
				}

				// 시간 재개
				Character->CustomTimeDilation = 1.0f;

				// 애니메이션 재개
				if (USkeletalMeshComponent* Mesh = Character->GetMesh())
				{
					Mesh->bPauseAnims = false;
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[Cube %d,%d] UNFROZEN (%d actors)"),
		CubeCoordinate.X, CubeCoordinate.Y, ManagedActors.Num());
}

void ACC_Cube::OnBoundaryOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("[Cube %d,%d] Boundary overlap detected by %s"),
		CubeCoordinate.X, CubeCoordinate.Y,
		*OtherActor->GetName());

	if (IsFrozen())
		return;

	// 플레이어인지 확인
	ACharacter* Player = Cast<ACharacter>(OtherActor);
	if (!Player || !Player->IsPlayerControlled())
		return;

	UBoxComponent* Trigger = Cast<UBoxComponent>(OverlappedComp);
	if (!Trigger || !BoundaryDirectionMap.Contains(Trigger))
		return;

	EBoundaryDirection Direction = BoundaryDirectionMap[Trigger];

	UE_LOG(LogTemp, Warning, TEXT("[Cube %d,%d] Player crossed boundary: %s"),
		CubeCoordinate.X, CubeCoordinate.Y,
		*UEnum::GetValueAsString(Direction));

	// CubeWorldManager 찾아서 전환 요청
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACC_CubeWorldManager::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		ACC_CubeWorldManager* Manager = Cast<ACC_CubeWorldManager>(FoundActors[0]);
		if (Manager)
		{
			Manager->RequestTransition(Direction);
		}
	}
}

FVector ACC_Cube::GetCubeCenter() const
{
	return GetActorLocation();
}

FBox ACC_Cube::GetCubeBounds() const
{
	FVector Center = GetCubeCenter();
	FVector HalfExtent(CubeSize / 2.0f);
	return FBox(Center - HalfExtent, Center + HalfExtent);
}

void ACC_Cube::DrawDebugInfo()
{
	if (!GetWorld())
		return;

	FBox Bounds = GetCubeBounds();
	FColor Color = (CubeState == ECubeState::Active) ? FColor::Green : FColor::Red;

	DrawDebugBox(GetWorld(), Bounds.GetCenter(), Bounds.GetExtent(),
		Color, false, -1.0f, 0, 50.0f);

	// 좌표 텍스트
	FString CoordText = FString::Printf(TEXT("Cube [%d, %d]"), CubeCoordinate.X, CubeCoordinate.Y);
	DrawDebugString(GetWorld(), GetCubeCenter() + FVector(0, 0, 500),
		CoordText, nullptr, Color, -1.0f, true, 2.0f);
}