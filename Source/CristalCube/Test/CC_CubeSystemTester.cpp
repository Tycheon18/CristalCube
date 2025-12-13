// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_CubeSystemTester.h"
#include "CC_TestActor.h"
#include "Kismet/GameplayStatics.h"
#include "../Gameplay/CC_Tile.h"
#include "../Gameplay/CC_Cube.h"

// Sets default values
ACC_CubeSystemTester::ACC_CubeSystemTester()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACC_CubeSystemTester::BeginPlay()
{
	Super::BeginPlay();
	
	// Cube 찾기
	TArray<AActor*> FoundCubes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACC_Cube::StaticClass(), FoundCubes);

	if (FoundCubes.Num() > 0)
	{
		Cube = Cast<ACC_Cube>(FoundCubes[0]);

		//if (bAutoRunTests && Cube)
		//{
		//	GetWorld()->GetTimerManager().SetTimer(
		//		TestStartTimerHandle,
		//		this,
		//		&ACC_CubeSystemTester::RunAllTests,
		//		TestStartDelay,
		//		false
		//	);
		//}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[CubeTester] No Cube found in level!"));
	}
}

// Called every frame
void ACC_CubeSystemTester::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACC_CubeSystemTester::RunAllTests()
{
	UE_LOG(LogTemp, Warning, TEXT("=============================================="));
	UE_LOG(LogTemp, Warning, TEXT("   CUBE SYSTEM TEST START"));
	UE_LOG(LogTemp, Warning, TEXT("=============================================="));

	TestsPassed = 0;
	TestsFailed = 0;
	TestLog.Empty();

	// 8가지 테스트 실행
	Test_CubeInitialization();
	Test_TileGeneration();
	Test_ActorRegistration();
	Test_TileActivation();
	Test_PlayerMovementSimulation();
	Test_TestActorDeactivation();
	Test_PerformanceMeasurement();
	Test_CubeWallWrapAround();

	// 리포트 출력
	PrintTestReport();
}

void ACC_CubeSystemTester::PrintTestReport()
{
	float SuccessRate = (TestsPassed + TestsFailed) > 0
		? (float)TestsPassed / (TestsPassed + TestsFailed) * 100.0f
		: 0.0f;

	UE_LOG(LogTemp, Warning, TEXT("=============================================="));
	UE_LOG(LogTemp, Warning, TEXT("   CUBE SYSTEM TEST REPORT"));
	UE_LOG(LogTemp, Warning, TEXT("=============================================="));
	UE_LOG(LogTemp, Warning, TEXT("Total Tests: %d"), TestsPassed + TestsFailed);
	UE_LOG(LogTemp, Warning, TEXT("Passed: %d"), TestsPassed);
	UE_LOG(LogTemp, Warning, TEXT("Failed: %d"), TestsFailed);
	UE_LOG(LogTemp, Warning, TEXT("Success Rate: %.1f%%"), SuccessRate);
	UE_LOG(LogTemp, Warning, TEXT("=============================================="));
	UE_LOG(LogTemp, Warning, TEXT("Detailed Results:"));

	for (const FString& Log : TestLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Log);
	}

	UE_LOG(LogTemp, Warning, TEXT("=============================================="));

	if (TestsFailed == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("   ALL TESTS PASSED!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("   SOME TESTS FAILED!"));
	}

	UE_LOG(LogTemp, Warning, TEXT("=============================================="));
}

// ========== 테스트 1: Cube 초기화 ==========
bool ACC_CubeSystemTester::Test_CubeInitialization()
{
	FString TestName = TEXT("Cube System Initialization");

	if (!Cube)
	{
		LogTest(TestName, false, TEXT("Cube not found"));
		return false;
	}

	if (Cube->CubeTiles.Num() != 9)
	{
		LogTest(TestName, false, FString::Printf(TEXT("Expected 9 tiles, found %d"), Cube->CubeTiles.Num()));
		return false;
	}

	LogTest(TestName, true, TEXT("Cube initialized with 9 tiles"));
	return true;
}

// ========== 테스트 2: 타일 생성 ==========
bool ACC_CubeSystemTester::Test_TileGeneration()
{
	FString TestName = TEXT("Tile Generation & Layout");

	for (int32 i = 0; i < 9; ++i)
	{
		ACC_Tile* Tile = Cube->GetTileAt(i);
		if (!Tile)
		{
			LogTest(TestName, false, FString::Printf(TEXT("Tile %d not found"), i));
			return false;
		}

		int32 TileIndex = Tile->GetTileIndex();

		if (TileIndex != i)
		{
			LogTest(TestName, false, FString::Printf(TEXT("Tile %d has wrong index: %d"), i, TileIndex));
			return false;
		}
	}

	LogTest(TestName, true, TEXT("All 9 tiles generated correctly"));
	return true;
}

// ========== 테스트 3: Actor 등록/해제 ==========
bool ACC_CubeSystemTester::Test_ActorRegistration()
{
	FString TestName = TEXT("Actor Registration/Unregistration");

	if (!TestActorClass)
	{
		LogTest(TestName, false, TEXT("TestActorClass not set!"));
		UE_LOG(LogTemp, Warning, TEXT("TestActorClass not Set!"));
		return false;
	}

	ACC_TestActor* TestActor = SpawnTestActorAtTile(4);
	if (!TestActor)
	{
		LogTest(TestName, false, TEXT("Failed to spawn test actor"));
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn test actor"));
		return false;
	}

	FPlatformProcess::Sleep(0.5f);

	ACC_Tile* Tile4 = Cube->GetTileAt(4);
	TArray<AActor*> ActorsInTile = Tile4->GetActorsInTile();
	if (!ActorsInTile.Contains(TestActor))
	{
		LogTest(TestName, false, FString::Printf(TEXT("TestActor not registered in Tile 4 (ActorsInTile: %d)"),
			ActorsInTile.Num()));
		UE_LOG(LogTemp, Warning, TEXT("TestActor not registered in Tile 4 (ActorsInTile: %d)"),
			ActorsInTile.Num());
		TestActor->Destroy();
		return false;
	}

	int32 CountBefore = Tile4->GetActorsInTile().Num();
	TestActor->Destroy();
	FPlatformProcess::Sleep(0.5f);
	int32 CountAfter = Tile4->GetActorsInTile().Num();

	if (CountAfter != CountBefore - 1)
	{
		LogTest(TestName, false, FString::Printf(TEXT("Actor count mismatch (Before: %d, After: %d)"),
			CountBefore, CountAfter));
		UE_LOG(LogTemp, Warning, TEXT("Actor count mismatch (Before: %d, After: %d)"), CountBefore, CountAfter);
		return false;
	}

	LogTest(TestName, true, TEXT("Actor registration/unregistration works correctly"));
	UE_LOG(LogTemp, Warning, TEXT("Actor registration/unregistration works correctly"));
	return true;
}

// ========== 테스트 4: 타일 활성화/비활성화 ==========
bool ACC_CubeSystemTester::Test_TileActivation()
{
	FString TestName = TEXT("Tile Activation/Deactivation");

	Cube->ActivateAllTiles();

	for (int32 i = 0; i < 9; ++i)
	{
		ACC_Tile* Tile = Cube->GetTileAt(i);
		if (!Tile || !Tile->IsTileActive())
		{
			LogTest(TestName, false, TEXT("ActivateAllTiles failed"));
			return false;
		}
	}

	Cube->ActivateOnlyTile(4);

	ACC_Tile* Tile4 = Cube->GetTileAt(4);
	if (!Tile4 || !Tile4->IsTileActive())
	{
		LogTest(TestName, false, TEXT("Tile 4 should be active"));
		return false;
	}

	for (int32 i = 0; i < 9; ++i)
	{
		if (i == 4) continue;

		ACC_Tile* Tile = Cube->GetTileAt(i);
		if (Tile && Tile->IsTileActive())
		{
			LogTest(TestName, false, FString::Printf(TEXT("Tile %d should be inactive"), i));
			return false;
		}
	}

	LogTest(TestName, true, TEXT("Tile activation/deactivation works correctly"));
	return true;
}

// ========== 테스트 5: Player 이동 시뮬레이션 ==========
bool ACC_CubeSystemTester::Test_PlayerMovementSimulation()
{
	FString TestName = TEXT("Player Movement Simulation");

	int32 TestPath[] = { 4, 5, 8, 7, 4 };

	for (int32 i = 0; i < 5; ++i)
	{
		int32 TileIndex = TestPath[i];
		Cube->ActivateTilesAroundPlayer(TileIndex);

		ACC_Tile* ActiveTile = Cube->GetTileAt(TileIndex);
		if (!ActiveTile || !ActiveTile->IsTileActive())
		{
			LogTest(TestName, false, FString::Printf(TEXT("Tile %d should be active"), TileIndex));
			return false;
		}

		for (int32 j = 0; j < 9; ++j)
		{
			if (j == TileIndex) continue;

			ACC_Tile* OtherTile = Cube->GetTileAt(j);
			if (OtherTile && OtherTile->IsTileActive())
			{
				LogTest(TestName, false, FString::Printf(TEXT("Tile %d should be inactive"), j));
				return false;
			}
		}
	}

	LogTest(TestName, true, TEXT("Player movement simulation successful"));
	return true;
}

// ========== 테스트 6: TestActor 비활성화 ==========
bool ACC_CubeSystemTester::Test_TestActorDeactivation()
{
	FString TestName = TEXT("TestActor Deactivation on Tile Switch");

	if (!TestActorClass)
	{
		LogTest(TestName, false, TEXT("TestActorClass not set!"));
		return false;
	}

	TArray<ACC_TestActor*> TestActors;
	for (int32 i = 0; i < 5; ++i)
	{
		ACC_TestActor* TestActor = SpawnTestActorAtTile(4);
		if (TestActor)
		{
			TestActors.Add(TestActor);
		}
	}

	FPlatformProcess::Sleep(0.5f);

	ACC_Tile* Tile4 = Cube->GetTileAt(4);
	TArray<AActor*> ActorsInTile = Tile4->GetActorsInTile();
	if (ActorsInTile.Num() != 5)
	{
		LogTest(TestName, false, FString::Printf(TEXT("Expected 5 testactors in Tile 4, found %d"),
			ActorsInTile.Num()));
		CleanupTestActors();
		return false;
	}

	Cube->ActivateTilesAroundPlayer(4);
	FPlatformProcess::Sleep(0.2f);

	for (ACC_TestActor* TestActor : TestActors)
	{
		if (!TestActor || !TestActor->IsActorTickEnabled())
		{
			LogTest(TestName, false, TEXT("TestActor should be active in Tile 4"));
			CleanupTestActors();
			return false;
		}
	}

	Cube->ActivateTilesAroundPlayer(5);
	FPlatformProcess::Sleep(0.2f);

	int32 ActiveCount = 0;
	for (ACC_TestActor* TestActor : TestActors)
	{
		if (TestActor && TestActor->IsActorTickEnabled())
		{
			ActiveCount++;
		}
	}

	if (ActiveCount > 0)
	{
		LogTest(TestName, false, FString::Printf(TEXT("Expected 0 active testactors, found %d"), ActiveCount));
		CleanupTestActors();
		return false;
	}

	Cube->ActivateTilesAroundPlayer(4);
	FPlatformProcess::Sleep(0.2f);

	ActiveCount = 0;
	for (ACC_TestActor* TestActor : TestActors)
	{
		if (TestActor && TestActor->IsActorTickEnabled())
		{
			ActiveCount++;
		}
	}

	if (ActiveCount != 5)
	{
		LogTest(TestName, false, FString::Printf(TEXT("Expected 5 active testactors, found %d"), ActiveCount));
		CleanupTestActors();
		return false;
	}

	for (ACC_TestActor* TestActor : TestActors)
	{
		if (TestActor)
		{
			TestActor->Destroy();
		}
	}

	LogTest(TestName, true, TEXT("TestActor deactivation system works correctly"));
	return true;
}

// ========== 테스트 7: 성능 측정 ==========
bool ACC_CubeSystemTester::Test_PerformanceMeasurement()
{
	FString TestName = TEXT("Performance Measurement");

	TArray<ACC_TestActor*> PerformanceTestActors;
	for (int32 i = 0; i < NumPerformanceTestActors; ++i)
	{
		int32 TileIndex = i % 9;
		ACC_TestActor* TestActor = SpawnTestActorAtTile(TileIndex);
		if (TestActor)
		{
			PerformanceTestActors.Add(TestActor);
		}
	}

	FPlatformProcess::Sleep(1.0f);

	int32 TotalRegistered = 0;
	for (int32 i = 0; i < 9; ++i)
	{
		ACC_Tile* Tile = Cube->GetTileAt(i);
		if (Tile)
		{
			TotalRegistered += Tile->GetActorsInTile().Num();
		}
	}

	if (TotalRegistered != NumPerformanceTestActors)
	{
		LogTest(TestName, false, FString::Printf(TEXT("Expected %d registered TestActors, found %d"),
			NumPerformanceTestActors, TotalRegistered));

		for (ACC_TestActor* TestActor : PerformanceTestActors)
		{
			if (TestActor) TestActor->Destroy();
		}
		return false;
	}

	Cube->ActivateTilesAroundPlayer(4);
	FPlatformProcess::Sleep(0.3f);

	int32 ActiveTestActors = 0;
	for (ACC_TestActor* TestActor : PerformanceTestActors)
	{
		if (TestActor && TestActor->IsActorTickEnabled())
		{
			ActiveTestActors++;
		}
	}

	ACC_Tile* Tile4 = Cube->GetTileAt(4);
	int32 ExpectedActive = Tile4 ? Tile4->GetActorsInTile().Num() : 0;

	if (ActiveTestActors != ExpectedActive)
	{
		LogTest(TestName, false, FString::Printf(TEXT("Expected %d active TestActors (Tile 4), found %d"),
			ExpectedActive, ActiveTestActors));

		for (ACC_TestActor* TestActor : PerformanceTestActors)
		{
			if (TestActor) TestActor->Destroy();
		}
		return false;
	}

	float ReductionPercent = (1.0f - (float)ActiveTestActors / NumPerformanceTestActors) * 100.0f;
	FString Message = FString::Printf(TEXT("%d/%d TestActors active (%.1f%% reduction)"),
		ActiveTestActors, NumPerformanceTestActors, ReductionPercent);

	for (ACC_TestActor* TestActor : PerformanceTestActors)
	{
		if (TestActor)
		{
			TestActor->Destroy();
		}
	}

	LogTest(TestName, true, Message);
	return true;
}

// ========== 테스트 8: CubeWall 순환 이동 ==========
bool ACC_CubeSystemTester::Test_CubeWallWrapAround()
{
	FString TestName = TEXT("CubeWall Wrap Around System");

	// CubeWall 존재 확인
	if (!Cube->CubeWall_Right || !Cube->CubeWall_Left ||
		!Cube->CubeWall_Top || !Cube->CubeWall_Bottom)
	{
		LogTest(TestName, false, TEXT("CubeWalls not found"));
		return false;
	}

	// 참고: 실제 순환 이동은 플레이 중에 CubeWall이 자동 처리
	// 여기서는 CubeWall 존재만 확인
	LogTest(TestName, true, TEXT("CubeWalls ready for wrap around (4 walls detected)"));
	return true;
}

void ACC_CubeSystemTester::LogTest(const FString& TestName, bool bPassed, const FString& Message)
{
	if (bPassed)
	{
		TestsPassed++;
		FString Log = FString::Printf(TEXT("[PASS] %s%s"), *TestName,
			Message.IsEmpty() ? TEXT("") : *FString::Printf(TEXT(": %s"), *Message));
		TestLog.Add(Log);
	}
	else
	{
		TestsFailed++;
		FString Log = FString::Printf(TEXT("[FAIL] %s%s"), *TestName,
			Message.IsEmpty() ? TEXT("") : *FString::Printf(TEXT(": %s"), *Message));
		TestLog.Add(Log);
	}
}

ACC_TestActor* ACC_CubeSystemTester::SpawnTestActorAtTile(int32 TileIndex)
{
	if (!Cube || !TestActorClass) return nullptr;

	ACC_Tile* Tile = Cube->GetTileAt(TileIndex);
	if (!Tile) return nullptr;

	FVector SpawnPos = Tile->GetTileCenter() + FVector(0, 0, 100);

	ACC_TestActor* TestActor = GetWorld()->SpawnActor<ACC_TestActor>(
		TestActorClass,
		SpawnPos,
		FRotator::ZeroRotator
	);

	if (TestActor)
	{
		SpawnedTestActors.Add(TestActor);
	}

	return TestActor;
}

void ACC_CubeSystemTester::CleanupTestActors()
{
	for (ACC_TestActor* TestActor : SpawnedTestActors)
	{
		if (TestActor)
		{
			TestActor->Destroy();
		}
	}
	SpawnedTestActors.Empty();
}

