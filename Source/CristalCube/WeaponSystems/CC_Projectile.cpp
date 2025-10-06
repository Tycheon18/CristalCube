// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_Projectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "../Characters/CC_EnemyCharacter.h"

// Sets default values
ACC_Projectile::ACC_Projectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create collision sphere
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(15.0f);
	CollisionSphere->SetCollisionProfileName(TEXT("Projectile"));

	RootComponent = CollisionSphere;

	// Query Only: ���� �浹 ���� ����
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// Projectile ���� ä��
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);

	// �⺻�� ��� ����
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);

	// Enemy ä��(ECC_GameTraceChannel1)�� Overlap
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Overlap �̺�Ʈ�� ���
	CollisionSphere->SetGenerateOverlapEvents(true);



	// Create projectile mesh (optional visual)
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(CollisionSphere);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create projectile movement component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionSphere;
	ProjectileMovement->InitialSpeed = 1000.0f;
	ProjectileMovement->MaxSpeed = 1000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bSweepCollision = true;

	// Initialize projectile properties
	Damage = 10.0f;
	Speed = 1000.0f;
	Lifetime = 5.0f;
	bDestroyOnHit = true;
	bCanPierce = false;
	PierceCount = 0;
	CurrentPierceCount = 0;

	// Effects
	HitEffect = nullptr;
	HitSound = nullptr;

	// Ownership
	ProjectileOwner = nullptr;

	// Set lifespan
	InitialLifeSpan = Lifetime;
}

// Called when the game starts or when spawned
void ACC_Projectile::BeginPlay()
{
	Super::BeginPlay();

	if (CollisionSphere)
	{
		//CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		//CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		//CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		//CollisionSphere->SetGenerateOverlapEvents(true);
		CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ACC_Projectile::OnOverlapBegin);

		//UE_LOG(LogTemp, Error, TEXT("[PROJECTILE] Forced collision settings in BeginPlay"));
	}

	if (ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = Speed;
		ProjectileMovement->MaxSpeed = Speed;
	}

	//if (CollisionSphere)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("[PROJECTILE] Collision Enabled: %d"),
	//		(int32)CollisionSphere->GetCollisionEnabled());

	//	UE_LOG(LogTemp, Error, TEXT("[PROJECTILE] Generate Overlap Events: %s"),
	//		CollisionSphere->GetGenerateOverlapEvents() ? TEXT("TRUE") : TEXT("FALSE"));

	//	UE_LOG(LogTemp, Error, TEXT("[PROJECTILE] Pawn Response: %d"),
	//		(int32)CollisionSphere->GetCollisionResponseToChannel(ECC_Pawn));
	//}

	//UE_LOG(LogTemp, Warning, TEXT("[PROJECTILE] Spawned - Damage: %.1f, Speed: %.1f"),
	//	Damage, Speed);
}

// Called every frame
void ACC_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//TArray<AActor*> OverlappingActors;
	//GetOverlappingActors(OverlappingActors, ACC_EnemyCharacter::StaticClass());

	//if (OverlappingActors.Num() > 0)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("[PROJECTILE] Manual overlap check found %d enemies!"),
	//		OverlappingActors.Num());
	//}
}

void ACC_Projectile::SetProjectileOwner(AActor* NewOwner)
{
	ProjectileOwner = NewOwner;
	SetInstigator(Cast<APawn>(NewOwner));
}

void ACC_Projectile::InitializeProjectile(float InDamage, float InSpeed)
{
	Damage = InDamage;

	if (InSpeed > 0.0f)
	{
		Speed = InSpeed;
		if (ProjectileMovement)
		{
			ProjectileMovement->InitialSpeed = Speed;
			ProjectileMovement->MaxSpeed = Speed;
		}
	}
}

void ACC_Projectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	if (OtherActor == ProjectileOwner)
	{
		return;
	}


	if (!OtherActor->ActorHasTag(FName("Enemy")))
	{
		UE_LOG(LogTemp, Warning, TEXT("[PROJECTILE] Not an enemy, ignoring: %s"),
			*OtherActor->GetName());
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[PROJECTILE] Hit: %s"), *OtherActor->GetName());

	ApplyDamageToActor(OtherActor);

	FVector HitLocation = SweepResult.ImpactPoint.IsZero() ?
		OtherActor->GetActorLocation() : FVector(SweepResult.ImpactPoint);
	PlayHitEffects(HitLocation);
	
	if (bCanPierce && ShouldPierceThrough(OtherActor))
	{
		CurrentPierceCount++;
		if (PierceCount > 0 && CurrentPierceCount >= PierceCount)
		{
			Destroy();
		}
	}
	else if (bDestroyOnHit)
	{
		Destroy();
	}
}

void ACC_Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}

void ACC_Projectile::ApplyDamageToActor(AActor* HitActor)
{
	if (!HitActor)
	{
		return;
	}

	// Apply damage using Unreal's damage system
	UGameplayStatics::ApplyDamage(
		HitActor,
		Damage,
		GetInstigatorController(),
		this,
		UDamageType::StaticClass()
	);

	UE_LOG(LogTemp, Log, TEXT("Projectile hit %s for %.1f damage"), *HitActor->GetName(), Damage);

}

void ACC_Projectile::PlayHitEffects(const FVector& HitLocation)
{
	// Spawn hit particle effect
	if (HitEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			HitEffect,
			HitLocation
		);
	}

	// Play hit sound
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			HitSound,
			HitLocation
		);
	}
}

bool ACC_Projectile::ShouldPierceThrough(AActor* HitActor)
{
	// Can pierce if:
	// 1. Pierce is enabled
	// 2. Haven't reached pierce limit
	// 3. Hit actor is valid

	if (!bCanPierce || !HitActor)
	{
		return false;
	}

	return CurrentPierceCount < PierceCount;
}

