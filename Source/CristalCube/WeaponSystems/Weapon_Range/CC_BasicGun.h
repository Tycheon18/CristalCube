// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../CC_Weapon.h"
#include "CC_BasicGun.generated.h"

/**
 * 
 */
UCLASS()
class CRISTALCUBE_API ACC_BasicGun : public ACC_Weapon
{
	GENERATED_BODY()
	
public:

	ACC_BasicGun();

	virtual void Attack() override;

protected:

	void ExecuteGunAttack();
};
