// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_Weapon.h"

// Sets default values
ACC_Weapon::ACC_Weapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACC_Weapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACC_Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACC_Weapon::Attack()
{
	// Implement attack logic here
}

bool ACC_Weapon::CanAttack() const
{
	// Implement logic to determine if the weapon can attack
	return bCanAttack;
}

void ACC_Weapon::OnEquipped(AActor* NewOwner)
{
	WeaponOwner = NewOwner;
}

void ACC_Weapon::OnUnequipped()
{
	WeaponOwner = nullptr;
}

void ACC_Weapon::ResetCooldown()
{
}

void ACC_Weapon::PlayAttackEffects()
{
}

float ACC_Weapon::CalculateFinalDamage() const
{
	return 0.0f;
}
