// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_EnemyAttackHit.h"
#include "../Characters/CC_EnemyCharacter.h"

void UAnimNotify_EnemyAttackHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (ACC_EnemyCharacter* EnemyCharacter = Cast<ACC_EnemyCharacter>(MeshComp->GetOwner()))
	{
		EnemyCharacter->DealDamageToTarget();

		UE_LOG(LogTemp, Log, TEXT("[ANIM NOTIFY] Enemy attack hit!"));
	}
}
