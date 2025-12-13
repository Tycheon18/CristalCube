// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../CC_Weapon.h"
#include "CC_BasicSword.generated.h"

/**
 * 
 */
UCLASS()
class CRISTALCUBE_API ACC_BasicSword : public ACC_Weapon
{
	GENERATED_BODY()
	
public:

	ACC_BasicSword();

	virtual void Attack() override;

protected:

	void ExecuteSwordAttack();
};
