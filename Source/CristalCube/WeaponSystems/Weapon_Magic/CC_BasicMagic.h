// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../CC_Weapon.h"
#include "CC_BasicMagic.generated.h"

/**
 * 
 */
UCLASS()
class CRISTALCUBE_API ACC_BasicMagic : public ACC_Weapon
{
	GENERATED_BODY()
	
public:

	ACC_BasicMagic();

	virtual void Attack() override;

protected:

	UPROPERTY(EditAnywhere, Category = "Magic Stats|Targeting")
	ETargetingMode TargetingMode = ETargetingMode::Single;

	UPROPERTY(EditAnywhere, Category = "Magic Stats|Targeting",
		meta = (EditCondition = "TargetingMode != ETargetingMode::None"))
	float SearchRadius = 800.0f;

	UPROPERTY(EditAnywhere, Category = "Magic Stats|Targeting",
		meta = (EditCondition = "TargetingMode == ETargetingMode::Multi"))
	int32 MaxTargets = 3;

	UPROPERTY(EditAnywhere, Category = "Magic Stats|Targeting")
	bool bRequireTarget = true;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UNiagaraSystem* MagicEffect;

	void ExecuteMagicAttack();

	void ExecuteSingleTarget();
	void ExecuteMultiTarget();
	void ExecuteAreaTarget();

	void ApplyMagicDamage(const FVector& Location);

};
