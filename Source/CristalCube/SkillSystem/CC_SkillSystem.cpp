// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_SkillSystem.h"
#include "../CC_LogHelper.h"
#include "../WeaponSystems/CC_Projectile.h"
#include "../Characters/CC_Character.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "DrawDebugHelpers.h"


// Sets default values for this component's properties
UCC_SkillSystem::UCC_SkillSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;  // 필요할 때만 Tick

	// ...
}


// Called when the game starts
void UCC_SkillSystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	UE_LOG(LogTemp, Log, TEXT("SkillSystem initialized for %s"), *GetOwner()->GetName());
}


// Called every frame
void UCC_SkillSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

//==============================================================================
// MAIN INTERFACE
//==============================================================================

void UCC_SkillSystem::ExecuteSkill(const FSkillDefinition& Skill, FVector TargetLocation)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecuteSkill: No owner!"));
		return;
	}

	// 컨텍스트 초기화
	FSkillExecutionContext Context;
	Context.Caster = Owner;
	Context.StartLocation = Owner->GetActorLocation();
	Context.TargetLocation = TargetLocation;
	Context.Direction = (TargetLocation - Context.StartLocation).GetSafeNormal();
	Context.CurrentDamage = Skill.BaseDamage * Skill.Passives.DamageMultiplier;

	UE_LOG(LogTemp, Log, TEXT("Executing Skill : %s, Core : %d"), *Skill.SkillID.ToString(),
		(int32)Skill.CoreType);

	// Cast VFX
	//if (Skill.CastEffect)
	//{
	//	SpawnEffect(Skill.CastEffect, Context.StartLocation);
	//}

	// Cast Sound
	//if (Skill.CastSound)
	//{
	//	PlaySound(Skill.CastSound, Context.StartLocation);
	//}

	// Core 실행
	switch (Skill.CoreType)
	{
	case ESkillCoreType::Projectile:
		ExecuteProjectile(Skill, Context);
		break;

	case ESkillCoreType::Instant:
		ExecuteInstant(Skill, Context);
		break;

	case ESkillCoreType::Area:
		ExecuteArea(Skill, Context);
		break;

	case ESkillCoreType::Beam:
		ExecuteBeam(Skill, Context);
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("ExecuteSkill: Unknown Core Type: %d"), (int32)Skill.CoreType);
		break;
	}
}

void UCC_SkillSystem::ExecuteSkillOnTarget(const FSkillDefinition& Skill, AActor* TargetActor)
{
	if (!TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteSkillOnTarget: No target!"));
		return;
	}

	ExecuteSkill(Skill, TargetActor->GetActorLocation());
}

//==============================================================================
// CORE EXECUTION - STUBS (다음 단계에서 구현)
//==============================================================================

void UCC_SkillSystem::ExecuteProjectile(const FSkillDefinition& Skill, FSkillExecutionContext& Context)
{
	if(!ProjectileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecuteProjectile: ProjectileClass not set!"));
		return;
	}

	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	int32 ProjectileCount = GetProjectileCount(Skill);

	for (int32 i = 0; i < ProjectileCount; ++i)
	{
		// 2-1. 스폰 위치 (약간 앞쪽)
		FVector SpawnLocation = Context.StartLocation + Context.Direction * 50.0f;

		// 2-2. 스폰 방향 (여러 발사 시 각도 분산)
		FVector SpawnDirection = Context.Direction;
		if (ProjectileCount > 1)
		{
			// MultiShot: 부채꼴 형태로 분산 (중앙 기준 ±30도)
			float AngleStep = 60.0f / (ProjectileCount - 1); // 총 60도 범위
			float Angle = -30.0f + (i * AngleStep);

			// 방향 벡터 회전
			SpawnDirection = Context.Direction.RotateAngleAxis(Angle, FVector::UpVector);
		}

		// 2-3. 투사체 스폰
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnLocation);
		SpawnTransform.SetRotation(SpawnDirection.ToOrientationQuat());

		ACC_Projectile* Projectile = World->SpawnActor<ACC_Projectile>(
			ProjectileClass,
			SpawnTransform
		);

		if (Projectile)
		{
			// 3. 투사체 초기화
			Projectile->SetProjectileOwner(Context.Caster);
			Projectile->InitializeProjectile(Context.CurrentDamage, Skill.Range);

			// 4. Penetrate Addon 설정
			if (Skill.Addons.Contains(ESkillAddonType::Penetrate))
			{
				Projectile->bCanPierce = true;
				Projectile->PierceCount = Skill.Passives.PierceCount;
			}

			Projectile->SetSkillData(this, Skill, Context);

			// 5. 디버그 표시
			if (bShowDebugShapes)
			{
				DrawDebugLine(
					World,
					SpawnLocation,
					SpawnLocation + SpawnDirection * 1000.0f,
					FColor::Red,
					false,
					DebugDrawDuration,
					0,
					2.0f
				);
			}

			UE_LOG(LogTemp, Log, TEXT("Spawned Projectile %d/%d - Damage: %.1f, Pierce: %s"),
				i + 1, ProjectileCount, Context.CurrentDamage,
				Projectile->bCanPierce ? TEXT("YES") : TEXT("NO"));
		}
	}
	
}

void UCC_SkillSystem::ExecuteInstant(const FSkillDefinition& Skill, FSkillExecutionContext& Context)
{
	UE_LOG(LogTemp, Log, TEXT("ExecuteInstant - TODO (Week 9 Day 4)"));

	// TODO: LineTrace, 즉시 피해, Hit VFX
}

void UCC_SkillSystem::ExecuteArea(const FSkillDefinition& Skill, FSkillExecutionContext& Context)
{

	UE_LOG(LogTemp, Log, TEXT("ExecuteArea - TODO (Week 9 Day 4)"));

	// TODO: SphereOverlap, 범위 피해, Area VFX
}

void UCC_SkillSystem::ExecuteBeam(const FSkillDefinition& Skill, FSkillExecutionContext& Context)
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	FVector Start = Context.StartLocation;
	FVector End = Context.TargetLocation;

	FVector Direction = (End - Start).GetSafeNormal();
	float Distance = FVector::Dist(Start, End);
	if (Distance > Skill.Range)
	{
		End = Start + Direction * Skill.Range;
	}

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Context.Caster);
	QueryParams.bTraceComplex = false;

	TArray<FHitResult> HitResults;
	bool bHit = World->LineTraceMultiByChannel(
		HitResults,
		Start,
		End,
		ECC_Pawn,
		QueryParams
	);

	TArray<AActor*> DamagedActors;
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || DamagedActors.Contains(HitActor))
		{
			continue;
		}

		if (HitActor->ActorHasTag(EnemyTag))
		{
			// 데미지 적용
			ApplyDamage(HitActor, Context.CurrentDamage, Context.Caster);
			DamagedActors.Add(HitActor);

			// Hit VFX
			if (Skill.HitEffect)
			{
				SpawnEffect(Skill.HitEffect, Hit.ImpactPoint);
			}

			// Addon 처리 (Explosion, Chain 등)
			ProcessAddons(Skill, Context, Hit);

			UE_LOG(LogTemp, Log, TEXT("Beam hit: %s"), *HitActor->GetName());
		}
	}

	// 4. Beam VFX 스폰 (Niagara Beam)
	if (Skill.CastEffect)
	{
		UNiagaraComponent* BeamEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			World,
			Skill.CastEffect,
			Start,
			FRotator::ZeroRotator,
			FVector(1.0f),
			true,
			true,
			ENCPoolMethod::AutoRelease
		);

		if (BeamEffect)
		{
			// Beam 끝점 설정 (Niagara Parameter)
			BeamEffect->SetVectorParameter(FName("BeamEnd"), End);
			BeamEffect->SetFloatParameter(FName("BeamWidth"), 10.0f);
		}
	}

	// 5. 디버그 표시
	if (bShowDebugShapes)
	{
		DrawDebugLine(
			World,
			Start,
			End,
			FColor::Cyan,
			false,
			DebugDrawDuration,
			0,
			5.0f
		);

		for (const FHitResult& Hit : HitResults)
		{
			DrawDebugSphere(
				World,
				Hit.ImpactPoint,
				20.0f,
				12,
				FColor::Yellow,
				false,
				DebugDrawDuration
			);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Beam fired: %d enemies hit"), DamagedActors.Num());

}

//==============================================================================
// ADDON PROCESSING - STUBS
//==============================================================================

void UCC_SkillSystem::ProcessAddons(const FSkillDefinition& Skill, FSkillExecutionContext& Context, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Log, TEXT("ProcessAddons - TODO (Week 9 Day 5-7)"));
}

void UCC_SkillSystem::ApplyExplosion(const FSkillDefinition& Skill, FSkillExecutionContext& Context, FVector Location)
{
	UE_LOG(LogTemp, Log, TEXT("ApplyExplosion - TODO (Week 9 Day 5-7)"));
}

void UCC_SkillSystem::ApplyChain(const FSkillDefinition& Skill, FSkillExecutionContext& Context, AActor* HitTarget)
{
	UE_LOG(LogTemp, Log, TEXT("ApplyChain - TODO (Week 9 Day 5-7)"));
}

bool UCC_SkillSystem::CanPenetrate(const FSkillDefinition& Skill, FSkillExecutionContext& Context) const
{
	if (!Skill.Addons.Contains(ESkillAddonType::Penetrate))
	{
		return false;
	}

	return Context.CurrentPierceCount < Skill.Passives.PierceCount;
}

int32 UCC_SkillSystem::GetProjectileCount(const FSkillDefinition& Skill) const
{
	int32 Count = Skill.Passives.ProjectileCount;

	if (Skill.Addons.Contains(ESkillAddonType::MultiShot))
	{
		Count += 2;  // MultiShot은 +2개
	}

	return FMath::Max(1, Count);
}

//==============================================================================
// UTILITY FUNCTIONS
//==============================================================================

AActor* UCC_SkillSystem::FindNearestEnemy(FVector Origin, float Radius, const TArray<AActor*>& ExcludeActors) const
{
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), EnemyTag, FoundEnemies);

	AActor* NearestEnemy = nullptr;
	float NearestDistance = Radius;

	for (AActor* Enemy : FoundEnemies)
	{
		if (!Enemy || ExcludeActors.Contains(Enemy))
		{
			continue;
		}

		float Distance = FVector::Dist(Origin, Enemy->GetActorLocation());
		if (Distance < NearestDistance)
		{
			NearestDistance = Distance;
			NearestEnemy = Enemy;
		}
	}

	return NearestEnemy;
}

TArray<AActor*> UCC_SkillSystem::FindEnemiesInRadius(FVector Origin, float Radius) const
{
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), EnemyTag, FoundEnemies);

	TArray<AActor*> EnemiesInRadius;
	for (AActor* Enemy : FoundEnemies)
	{
		if (!Enemy)
		{
			continue;
		}

		float Distance = FVector::Dist(Origin, Enemy->GetActorLocation());
		if (Distance <= Radius)
		{
			EnemiesInRadius.Add(Enemy);
		}
	}

	return EnemiesInRadius;
}

void UCC_SkillSystem::ApplyDamage(AActor* Target, float Damage, AActor* DamageCauser)
{
	if (!Target || Damage <= 0.0f)
	{
		return;
	}

	// Character 타입이면 직접 피해 적용
	if (ACC_Character* Character = Cast<ACC_Character>(Target))
	{
		Character->TakeDamage(Damage, FDamageEvent(), nullptr, DamageCauser);
		UE_LOG(LogTemp, Log, TEXT("Applied %.1f damage to %s"), Damage, *Target->GetName());
	}
}

void UCC_SkillSystem::SpawnEffect(UNiagaraSystem* Effect, FVector Location, FRotator Rotation)
{
	if (!Effect || !GetWorld())
	{
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		Effect,
		Location,
		Rotation,
		FVector(1.0f),
		true,  // Auto Destroy
		true,  // Auto Activate
		ENCPoolMethod::AutoRelease  // 풀링!
	);
}

void UCC_SkillSystem::PlaySound(USoundBase* Sound, FVector Location)
{
	if (!Sound || !GetWorld())
	{
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location);
}
