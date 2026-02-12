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

    /** �׸��� ũ�� (3x3) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cube|Grid")
    int32 GridSize = 3;

    /** ť�� ũ�� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cube|Grid")
    float CubeSize = 500.0f;

    /** ť�� Ŭ���� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cube|Grid")
    TSubclassOf<class ACC_Cube> CubeClass;

    /** ���� ť�� ��ǥ */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cube|State")
    FIntPoint CurrentCubeCoord;

    /** ��ȯ ������ ���� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cube|State")
    bool bIsTransitioning = false;

    // ========================================
    // Data Structures
    // ========================================

    /** ť�� ������ �׸��� (9��) */
    UPROPERTY()
    TMap<FIntPoint, FCubeData> CubeGrid;

    /** ���� Active ť�� */
    UPROPERTY()
    ACC_Cube* ActiveCube;

    /** Spawn�� ť��� */
    UPROPERTY()
    TArray<ACC_Cube*> LoadedCubes;

    // ========================================
    // Initialization
    // ========================================

    /** �ý��� �ʱ�ȭ */
    UFUNCTION(BlueprintCallable, Category = "Cube")
    void InitializeSystem();

    /** ť�� �׸��� ������ ���� */
    void InitializeCubeGrid();

    // ========================================
    // Cube Management
    // ========================================

    /** ť�� Spawn */
    UFUNCTION(BlueprintCallable, Category = "Cube")
    ACC_Cube* SpawnCube(FIntPoint Coordinate);

    /** ť�� Despawn (�޸� �����, ���߿� ����) */
    UFUNCTION(BlueprintCallable, Category = "Cube")
    void DespawnCube(FIntPoint Coordinate);

    /** ť�� ã�� �Ǵ� Spawn */
    UFUNCTION(BlueprintCallable, Category = "Cube")
    ACC_Cube* FindOrSpawnCube(FIntPoint Coordinate);

    /** ť�� ã�� */
    UFUNCTION(BlueprintCallable, Category = "Cube")
    ACC_Cube* FindCube(FIntPoint Coordinate) const;

    // ========================================
    // Transition System
    // ========================================

    /** ť�� ��ȯ ��û */
    UFUNCTION(BlueprintCallable, Category = "Cube|Transition")
    void RequestTransition(EBoundaryDirection Direction);

    /** ���� ť�� ��ǥ ��� (��ȯ ����) */
    UFUNCTION(BlueprintCallable, Category = "Cube|Transition")
    FIntPoint GetNextCubeCoord(FIntPoint Current, EBoundaryDirection Direction) const;

    /** ��ȯ ���� (���� �� ȣ��) */
    void PerformTransition(FIntPoint NextCoord);

    /** �÷��̾� ��ġ ��� (��ȯ ��) */
    FVector CalculatePlayerPositionInCube(ACC_Cube* TargetCube, EBoundaryDirection FromDirection) const;

    // ========================================
    // Player Management
    // ========================================

    /** �÷��̾� ���� �������� */
    UFUNCTION(BlueprintCallable, Category = "Cube")
    ACharacter* GetPlayerCharacter() const;

    /** �÷��̾ ť��� �̵� */
    UFUNCTION(BlueprintCallable, Category = "Cube")
    void MovePlayerToCube(FIntPoint Coordinate);

    // ========================================
    // Events
    // ========================================

    /** ť�� ��ȯ �Ϸ� �̺�Ʈ */
    UPROPERTY(BlueprintAssignable, Category = "Cube|Events")
    FOnCubeTransition OnCubeTransition;

    // ========================================
    // Debug
    // ========================================

    /** ����� ���� ��� */
    UFUNCTION(BlueprintCallable, Category = "Cube|Debug")
    void PrintDebugInfo();

    /** ��� ť�� Draw */
    UFUNCTION(BlueprintCallable, Category = "Cube|Debug")
    void DrawAllCubes();

    /** �ڵ� �׽�Ʈ ��� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cube|Debug")
    bool bAutoTest = false;

protected:

    /** ���� ��ȯ ���� (�׽�Ʈ��) */
    EBoundaryDirection LastTransitionDirection;
};
