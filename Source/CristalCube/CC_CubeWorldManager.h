// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CristalCubeStruct.h"
#include "CC_CubeWorldManager.generated.h"

UCLASS()
class CRISTALCUBE_API ACC_CubeWorldManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACC_CubeWorldManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    // ========================================
    // Properties
    // ========================================

    /** 그리드 크기 (3x3) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cube|Grid")
    int32 GridSize = 3;

    /** 큐브 크기 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cube|Grid")
    float CubeSize = 500.0f;

    /** 큐브 클래스 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cube|Grid")
    TSubclassOf<class ACC_Cube> CubeClass;

    /** 현재 큐브 좌표 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cube|State")
    FIntPoint CurrentCubeCoord;

    /** 전환 중인지 여부 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cube|State")
    bool bIsTransitioning = false;

    // ========================================
    // Data Structures
    // ========================================

    /** 큐브 데이터 그리드 (9개) */
    UPROPERTY()
    TMap<FIntPoint, FCubeData> CubeGrid;

    /** 현재 Active 큐브 */
    UPROPERTY()
    ACC_Cube* ActiveCube;

    /** Spawn된 큐브들 */
    UPROPERTY()
    TArray<ACC_Cube*> LoadedCubes;

    // ========================================
    // Initialization
    // ========================================

    /** 시스템 초기화 */
    UFUNCTION(BlueprintCallable, Category = "Cube")
    void InitializeSystem();

    /** 큐브 그리드 데이터 생성 */
    void InitializeCubeGrid();

    // ========================================
    // Cube Management
    // ========================================

    /** 큐브 Spawn */
    UFUNCTION(BlueprintCallable, Category = "Cube")
    ACC_Cube* SpawnCube(FIntPoint Coordinate);

    /** 큐브 Despawn (메모리 절약용, 나중에 구현) */
    UFUNCTION(BlueprintCallable, Category = "Cube")
    void DespawnCube(FIntPoint Coordinate);

    /** 큐브 찾기 또는 Spawn */
    UFUNCTION(BlueprintCallable, Category = "Cube")
    ACC_Cube* FindOrSpawnCube(FIntPoint Coordinate);

    /** 큐브 찾기 */
    UFUNCTION(BlueprintCallable, Category = "Cube")
    ACC_Cube* FindCube(FIntPoint Coordinate) const;

    // ========================================
    // Transition System
    // ========================================

    /** 큐브 전환 요청 */
    UFUNCTION(BlueprintCallable, Category = "Cube|Transition")
    void RequestTransition(EBoundaryDirection Direction);

    /** 다음 큐브 좌표 계산 (순환 고려) */
    UFUNCTION(BlueprintCallable, Category = "Cube|Transition")
    FIntPoint GetNextCubeCoord(FIntPoint Current, EBoundaryDirection Direction) const;

    /** 전환 실행 (암전 중 호출) */
    void PerformTransition(FIntPoint NextCoord);

    /** 플레이어 위치 계산 (전환 시) */
    FVector CalculatePlayerPositionInCube(ACC_Cube* TargetCube, EBoundaryDirection FromDirection) const;

    // ========================================
    // Player Management
    // ========================================

    /** 플레이어 참조 가져오기 */
    UFUNCTION(BlueprintCallable, Category = "Cube")
    ACharacter* GetPlayerCharacter() const;

    /** 플레이어를 큐브로 이동 */
    UFUNCTION(BlueprintCallable, Category = "Cube")
    void MovePlayerToCube(FIntPoint Coordinate);

    // ========================================
    // Events
    // ========================================

    /** 큐브 전환 완료 이벤트 */
    UPROPERTY(BlueprintAssignable, Category = "Cube|Events")
    FOnCubeTransition OnCubeTransition;

    // ========================================
    // Debug
    // ========================================

    /** 디버그 정보 출력 */
    UFUNCTION(BlueprintCallable, Category = "Cube|Debug")
    void PrintDebugInfo();

    /** 모든 큐브 Draw */
    UFUNCTION(BlueprintCallable, Category = "Cube|Debug")
    void DrawAllCubes();

    /** 자동 테스트 모드 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cube|Debug")
    bool bAutoTest = false;

protected:

    /** 이전 전환 방향 (테스트용) */
    EBoundaryDirection LastTransitionDirection;
};
