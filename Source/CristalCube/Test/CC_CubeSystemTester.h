// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CC_CubeSystemTester.generated.h"

UCLASS()
class CRISTALCUBE_API ACC_CubeSystemTester : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACC_CubeSystemTester();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// ========== 설정 ==========

	/** 자동 테스트 실행 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Testing")
	bool bAutoRunTests = true;

	/** 테스트 시작 딜레이 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Testing")
	float TestStartDelay = 2.0f;

	/** 테스트용 적 수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Testing")
	int32 NumTestEnemies = 20;

	/** 성능 테스트용 적 수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Testing")
	int32 NumPerformanceTestActors = 100;

	/** 테스트 Enemy 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Testing")
	TSubclassOf<class ACC_TestActor> TestActorClass;

	// ========== 상태 ==========

	/** Cube 참조 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Testing")
	class ACC_Cube* Cube;

	/** 테스트 결과 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Testing")
	int32 TestsPassed = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Testing")
	int32 TestsFailed = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Testing")
	TArray<FString> TestLog;

	// ========== 테스트 함수 ==========

	/** 모든 테스트 실행 */
	UFUNCTION(BlueprintCallable, Category = "Testing")
	void RunAllTests();

	/** 테스트 리포트 출력 */
	UFUNCTION(BlueprintCallable, Category = "Testing")
	void PrintTestReport();

	// 개별 테스트
	UFUNCTION(BlueprintCallable, Category = "Testing")
	bool Test_CubeInitialization();

	UFUNCTION(BlueprintCallable, Category = "Testing")
	bool Test_TileGeneration();

	UFUNCTION(BlueprintCallable, Category = "Testing")
	bool Test_ActorRegistration();
	
	UFUNCTION(BlueprintCallable, Category = "Testing")
	bool Test_TileActivation();
	
	UFUNCTION(BlueprintCallable, Category = "Testing")
	bool Test_PlayerMovementSimulation();

	UFUNCTION(BlueprintCallable, Category = "Testing")
	bool Test_TestActorDeactivation();
	
	UFUNCTION(BlueprintCallable, Category = "Testing")
	bool Test_PerformanceMeasurement();

	UFUNCTION(BlueprintCallable, Category = "Testing")
	bool Test_CubeWallWrapAround();

protected:
	/** 테스트 로그 */
	void LogTest(const FString& TestName, bool bPassed, const FString& Message = TEXT(""));

	/** 테스트 액터 생성 */
	ACC_TestActor* SpawnTestActorAtTile(int32 TileIndex);

	/** 정리 */
	void CleanupTestActors();

	FTimerHandle TestStartTimerHandle;
	TArray<class ACC_TestActor*> SpawnedTestActors;
};
