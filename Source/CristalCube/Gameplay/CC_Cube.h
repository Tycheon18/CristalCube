// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CC_Cube.generated.h"

UCLASS()
class CRISTALCUBE_API ACC_Cube : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACC_Cube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:


	/** 전체 큐브 크기 (3x3 = 6000) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cube")
	float CubeSize = 6000.0f;

	/** 개별 타일 크기 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cube")
	float TileSize = 2000.0f;

	/** 큐브 높이 (충분히 크게) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cube")
	float CubeHeight = 10000.0f;

	/** 큐브 벽 두께 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cube")
	float WallThickness = 500.0f;

	/** 타일 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cube")
	TSubclassOf<class ACC_Tile> TileClass;

	// ========== 큐브 벽 (외부 경계) ==========

	/** 큐브 벽 - 오른쪽 (X+) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cube Walls")
	class UBoxComponent* CubeWall_Right;

	/** 큐브 벽 - 왼쪽 (X-) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cube Walls")
	class UBoxComponent* CubeWall_Left;

	/** 큐브 벽 - 위쪽 (Y+) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cube Walls")
	class UBoxComponent* CubeWall_Top;

	/** 큐브 벽 - 아래쪽 (Y-) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cube Walls")
	class UBoxComponent* CubeWall_Bottom;

	// ========== 큐브 타일 ==========

	/** 생성된 타일들 (9개) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cube")
	TArray<class ACC_Tile*> CubeTiles;

	/** 현재 플레이어 타일 인덱스 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cube")
	int32 CurrentPlayerTileIndex = 4;

	// ========== 큐브 관리 함수 ==========

	/** 큐브 생성 (3x3 타일 + 외부 벽) */
	UFUNCTION(BlueprintCallable, Category = "Cube")
	void GenerateCube();

	/** 인덱스로 타일 가져오기 (0-8) */
	UFUNCTION(BlueprintCallable, Category = "Cube")
	ACC_Tile* GetTileAt(int32 Index) const;

	/** 위치로 타일 인덱스 가져오기 */
	UFUNCTION(BlueprintCallable, Category = "Cube")
	int32 GetTileIndexAtPosition(const FVector& Position) const;

	/** 플레이어 타일 기준 활성화 */
	UFUNCTION(BlueprintCallable, Category = "Cube")
	void ActivateTilesAroundPlayer(int32 PlayerTileIndex);

	/** 특정 타일만 활성화 */
	UFUNCTION(BlueprintCallable, Category = "Cube")
	void ActivateOnlyTile(int32 TileIndex);

	/** 모든 타일 활성화 */
	UFUNCTION(BlueprintCallable, Category = "Cube")
	void ActivateAllTiles();

	/** 모든 타일 비활성화 */
	UFUNCTION(BlueprintCallable, Category = "Cube")
	void DeactivateAllTiles();

	/** 오른쪽 벽 충돌 → 왼쪽으로 순환 */
	UFUNCTION()
	void OnCubeWallHit_Right(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	/** 왼쪽 벽 충돌 → 오른쪽으로 순환 */
	UFUNCTION()
	void OnCubeWallHit_Left(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	/** 위쪽 벽 충돌 → 아래로 순환 */
	UFUNCTION()
	void OnCubeWallHit_Top(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	/** 아래쪽 벽 충돌 → 위로 순환 */
	UFUNCTION()
	void OnCubeWallHit_Bottom(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

protected:
	/** 인덱스 → 행/열 변환 */
	void IndexToRowCol(int32 Index, int32& OutRow, int32& OutCol) const;

	/** 행/열 → 인덱스 변환 */
	int32 RowColToIndex(int32 Row, int32 Col) const;

	/** 큐브 벽 초기화 */
	void InitializeCubeWalls();

};
