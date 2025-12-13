// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_GameHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UCC_GameHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCC_GameHUD::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	HealthBar->SetPercent(CurrentHealth / MaxHealth);
}

void UCC_GameHUD::UpdateExp(float CurrentExp, float MaxExp, int32 Level)
{
	ExpBar->SetPercent(CurrentExp / MaxExp);
	FText InLevelText = FText::Format(FText::FromString(TEXT("Level : {0}")), Level);

	LevelText->SetText(InLevelText);
}

void UCC_GameHUD::UpdateTimer(float TotalSeconds)
{
	int32 Minutes = FMath::Floor(TotalSeconds / 60);
	int32 Seconds = FMath::Floor(int32(TotalSeconds) % 60);

	FText InTimerText = FText::Format(FText::FromString(TEXT("{0} : {1}")), Minutes, Seconds);

	TimerText->SetText(InTimerText);
}

