// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CC_TileSystemTester.generated.h"

UCLASS()
class CRISTALCUBE_API ACC_TileSystemTester : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACC_TileSystemTester();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	// Test code is moved to the 'cube system tester' for better organization.

//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Settings")
//	bool bAutoRunTests = true;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Settings")
//	float TestStartDelay = 2.0f;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Settings")
//	int32 NumTestEnemies = 20;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Settings")
//	TSubclassOf<class ACC_TestActor> TestActorClass;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Settings")
//	int32 NumPerformanceTestActors = 100;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Test Results")
//	int32 TestsPassed = 0;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Test Results")
//	int32 TestsFailed = 0;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Test Results")
//	TArray<FString> TestLog;
//
//public:
//
//	UFUNCTION(BlueprintCallable, Category = "Testing")
//	void RunAllTests();
//
//	UFUNCTION(BlueprintCallable, Category = "Testing")
//	bool Test_TileSystemInitialization();
//
//	UFUNCTION(BlueprintCallable, Category = "Testing")
//	bool Test_TileGeneration();
//
//	UFUNCTION(BlueprintCallable, Category = "Testing")
//	bool Test_ActorRegistration();
//
//	UFUNCTION(BlueprintCallable, Category = "Testing")
//	bool Test_TileActivation();
//
//	UFUNCTION(BlueprintCallable, Category = "Testing")
//	bool Test_PlayerMovementSimulation();
//
//	UFUNCTION(BlueprintCallable, Category = "Testing")
//	bool Test_ActorDeactivation();
//
//	UFUNCTION(BlueprintCallable, Category = "Testing")
//	bool Test_PerformanceMeasurement();
//
//	UFUNCTION(BlueprintCallable, Category = "Testing")
//	bool Test_WrapAroundSystem();
//
//	UFUNCTION(BlueprintCallable, Category = "Testing")
//	void PrintTestReport();
//
//	UFUNCTION(BlueprintCallable, Category = "Testing")
//	void CleanupTestActors();
//
//
//protected:
//
//	class ACC_TileManager* TileManager;
//	TArray<ACC_TestActor*> SpawnedTestActors;
//
//	void LogTest(const FString& TestName, bool bPassed, const FString& Message);
//	ACC_TestActor* SpawnTestActorAtTile(int32 TileIndex);

};
