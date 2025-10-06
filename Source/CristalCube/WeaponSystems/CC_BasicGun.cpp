// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_BasicGun.h"
#include "../CC_LogHelper.h"

ACC_BasicGun::ACC_BasicGun()
{
    // Weapon info
    BaseStats.WeaponCategory = EWeaponCategory::Ranged;
    BaseStats.BaseDamage = 15.0f;
    BaseStats.AttackSpeed = 2.0f;  // 2 attacks per second

    // Ranged settings
    RangedStats.ProjectileCount = 1;
    RangedStats.ProjectileSpeed = 1200.0f;
    RangedStats.Accuracy = 0.95f;  // 95% accuracy (5% spread)

    // Auto-aim settings
    bAutoAim = true;
    AutoAimRadius = 1500.0f;  // 15 meters

    CC_LOG_WEAPON(Log, "BasicGun created (Damage: %.1f, Speed: %.1f/s)",
        BaseStats.BaseDamage, BaseStats.AttackSpeed);
}

void ACC_BasicGun::Attack()
{
	Super::Attack();

    // Update attack state
    LastAttackTime = GetWorld()->GetTimeSeconds();
    bCanAttack = false;

    // Perform ranged attack (spawns projectile with auto-aim)
    PerformRangedAttack();

    // Play effects
    PlayAttackEffects();

    // Schedule cooldown reset
    float CooldownDuration = 1.0f / BaseStats.AttackSpeed;
    GetWorld()->GetTimerManager().SetTimer(
        AttackCooldownTimer,
        this,
        &ACC_BasicGun::ResetCooldown,
        CooldownDuration,
        false
    );

    CC_LOG_WEAPON(VeryVerbose, "BasicGun fired");
}
