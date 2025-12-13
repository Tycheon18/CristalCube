// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_SearchingComponent.h"
#include "CC_EnemyManager.h"

// Sets default values for this component's properties
UCC_SearchingComponent::UCC_SearchingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bEnableDebugVisualization = false;
	// ...
}


// Called when the game starts
void UCC_SearchingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	EnemyManager = ACC_EnemyManager::Get(this);
	if (!EnemyManager)
	{
		UE_LOG(LogTemp, Error, TEXT("[TargetingComponent] Failed to get EnemyManager!"));
	}


}


// Called every frame
void UCC_SearchingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

AActor* UCC_SearchingComponent::FindNearestEnemy(FVector SearchOrigin, float SearchRadius)
{
	if (!EnemyManager) return nullptr;

	AActor* NearestEnemy = EnemyManager->GetNearestEnemy(SearchOrigin, SearchRadius);

	if (bEnableDebugVisualization && NearestEnemy)
	{
		DrawDebugLine(
			GetWorld(),
			SearchOrigin,
			NearestEnemy->GetActorLocation(),
			FColor::Green,
			false,
			0.5f,
			0,
			2.0f
		);
	}

	return NearestEnemy;
}

AActor* UCC_SearchingComponent::FindNearestEnemyInDirection(FVector SearchOrigin, FVector Direction, float MaxAngle, float SearchRadius)
{
	if (!EnemyManager) return nullptr;

	TArray<AActor*> AllEnemies = EnemyManager->GetEnemiesInRadius(SearchOrigin, SearchRadius);
	Direction.Normalize();

	AActor* BestEnemy = nullptr;
	float BestDistance = FLT_MAX;

	for (AActor* Enemy : AllEnemies)
	{
		if (!Enemy) continue;

		FVector ToEnemy = Enemy->GetActorLocation() - SearchOrigin;
		float Distance = ToEnemy.Size();
		ToEnemy.Normalize();

		float DotProduct = FVector::DotProduct(Direction, ToEnemy);
		float AngleInDegrees = FMath::RadiansToDegrees(FMath::Acos(DotProduct));

		if (AngleInDegrees <= MaxAngle && Distance < BestDistance)
		{
			BestEnemy = Enemy;
			BestDistance = Distance;
		}
	}

	return BestEnemy;
}

TArray<AActor*> UCC_SearchingComponent::FindRandomEnemies(FVector SearchOrigin, float SearchRadius, int32 Count)
{
	TArray<AActor*> SelectedEnemies;
	if (!EnemyManager || Count <= 0) return SelectedEnemies;

	TArray<AActor*> PotentialTargets = EnemyManager->GetEnemiesInRadius(SearchOrigin, SearchRadius);
	int32 SelectCount = FMath::Min(Count, PotentialTargets.Num());

	for (int32 i = 0; i < SelectCount; i++)
	{
		if (PotentialTargets.Num() == 0) break;

		int32 RandomIndex = FMath::RandRange(0, PotentialTargets.Num() - 1);
		SelectedEnemies.Add(PotentialTargets[RandomIndex]);
		PotentialTargets.RemoveAt(RandomIndex);
	}

	return SelectedEnemies;
}

TArray<AActor*> UCC_SearchingComponent::FindNearestEnemies(FVector SearchOrigin, float SearchRadius, int32 Count)
{
	TArray<AActor*> NearestEnemies;
	if (!EnemyManager || Count <= 0) return NearestEnemies;

	TArray<AActor*> AllEnemies = EnemyManager->GetEnemiesInRadius(SearchOrigin, SearchRadius);

	AllEnemies.Sort([SearchOrigin](const AActor& A, const AActor& B)
		{
			float DistA = FVector::DistSquared(SearchOrigin, A.GetActorLocation());
			float DistB = FVector::DistSquared(SearchOrigin, B.GetActorLocation());
			return DistA < DistB;
		});

	int32 SelectCount = FMath::Min(Count, AllEnemies.Num());
	for (int32 i = 0; i < SelectCount; i++)
	{
		NearestEnemies.Add(AllEnemies[i]);
	}

	return NearestEnemies;
}

TArray<AActor*> UCC_SearchingComponent::GetEnemiesInSphere(FVector Center, float Radius)
{
	if (!EnemyManager) return TArray<AActor*>();

	TArray<AActor*> Enemies = EnemyManager->GetEnemiesInRadius(Center, Radius);

	if (bEnableDebugVisualization)
	{
		DebugDrawSphere(Center, Radius, FColor::Blue, 0.5f);
	}

	return Enemies;
}

TArray<AActor*> UCC_SearchingComponent::GetEnemiesInCone(FVector Origin, FVector Direction, float Range, float Angle)
{
	TArray<AActor*> EnemiesInCone;
	if (!EnemyManager) return EnemiesInCone;

	Direction.Normalize();
	TArray<AActor*> AllEnemies = EnemyManager->GetEnemiesInRadius(Origin, Range);

	float HalfAngle = Angle * 0.5f;

	for (AActor* Enemy : AllEnemies)
	{
		if (!Enemy) continue;

		FVector ToEnemy = Enemy->GetActorLocation() - Origin;
		float Distance = ToEnemy.Size();

		if (Distance > Range) continue;

		ToEnemy.Normalize();
		float DotProduct = FVector::DotProduct(Direction, ToEnemy);
		float AngleInDegrees = FMath::RadiansToDegrees(FMath::Acos(DotProduct));

		if (AngleInDegrees <= HalfAngle)
		{
			EnemiesInCone.Add(Enemy);
		}
	}

	if (bEnableDebugVisualization)
	{
		DebugDrawCone(Origin, Direction, Range, Angle, FColor::Red, 0.5f);
	}

	return EnemiesInCone;
}

TArray<AActor*> UCC_SearchingComponent::GetEnemiesInBox(FVector Center, FVector HalfExtents, FRotator Rotation)
{
	TArray<AActor*> EnemiesInBox;
	if (!EnemyManager) return EnemiesInBox;

	float MaxExtent = FMath::Max3(HalfExtents.X, HalfExtents.Y, HalfExtents.Z);
	TArray<AActor*> AllEnemies = EnemyManager->GetEnemiesInRadius(Center, MaxExtent * 1.5f);

	FTransform BoxTransform(Rotation, Center);

	for (AActor* Enemy : AllEnemies)
	{
		if (!Enemy) continue;

		FVector LocalPos = BoxTransform.InverseTransformPosition(Enemy->GetActorLocation());

		// 박스 체크
		if (FMath::Abs(LocalPos.X) <= HalfExtents.X &&
			FMath::Abs(LocalPos.Y) <= HalfExtents.Y &&
			FMath::Abs(LocalPos.Z) <= HalfExtents.Z)
		{
			EnemiesInBox.Add(Enemy);
		}
	}

	if (bEnableDebugVisualization)
	{
		DrawDebugBox(
			GetWorld(),
			Center,
			HalfExtents,
			Rotation.Quaternion(),
			FColor::Purple,
			false,
			0.5f,
			0,
			2.0f
		);
	}

	return EnemiesInBox;
}

TArray<FRotator> UCC_SearchingComponent::CalculateSpreadRotations(FRotator BaseRotation, int32 Count, float SpreadAngle)
{
	TArray<FRotator> Rotations;
	if (Count <= 0) return Rotations;

	if (Count == 1)
	{
		Rotations.Add(BaseRotation);
		return Rotations;
	}

	float HalfSpread = SpreadAngle * 0.5f;
	float AngleStep = SpreadAngle / (Count - 1);

	for (int32 i = 0; i < Count; i++)
	{
		float AngleOffset = -HalfSpread + (AngleStep * i);
		FRotator SpreadRotation = BaseRotation;
		SpreadRotation.Yaw += AngleOffset;
		Rotations.Add(SpreadRotation);
	}

	return Rotations;
}

TArray<FVector> UCC_SearchingComponent::CalculateCirclePositions(FVector Center, float Radius, int32 Count, float StartAngle)
{
	TArray<FVector> Positions;
	if (Count <= 0) return Positions;

	float AngleStep = 360.0f / Count;

	for (int32 i = 0; i < Count; i++)
	{
		float Angle = StartAngle + (AngleStep * i);
		float Radians = FMath::DegreesToRadians(Angle);

		FVector Offset(
			FMath::Cos(Radians) * Radius,
			FMath::Sin(Radians) * Radius,
			0.0f
		);

		Positions.Add(Center + Offset);
	}

	return Positions;
}

void UCC_SearchingComponent::DebugDrawSphere(FVector Center, float Radius, FColor Color, float Duration)
{
#if !UE_BUILD_SHIPPING
	if (bEnableDebugVisualization)
	{
		DrawDebugSphere(GetWorld(), Center, Radius, 12, Color, false, Duration, 0, 2.0f);
	}
#endif
}

void UCC_SearchingComponent::DebugDrawCone(FVector Origin, FVector Direction, float Range, float Angle, FColor Color, float Duration)
{
#if !UE_BUILD_SHIPPING
	if (!bEnableDebugVisualization) return;

	Direction.Normalize();
	FVector EndPoint = Origin + Direction * Range;

	DrawDebugLine(GetWorld(), Origin, EndPoint, Color, false, Duration, 0, 2.0f);

	int32 Segments = 16;
	float HalfAngle = Angle * 0.5f;

	for (int32 i = 0; i <= Segments; i++)
	{
		float CurrentAngle = (360.0f / Segments) * i;
		FRotator Rotation = Direction.Rotation();
		Rotation.Yaw += CurrentAngle;

		FVector ConeDirection = Rotation.Vector();
		FVector ConeEnd = Origin + ConeDirection * Range;

		DrawDebugLine(GetWorld(), Origin, ConeEnd, Color.WithAlpha(128), false, Duration, 0, 1.0f);
	}
#endif
}