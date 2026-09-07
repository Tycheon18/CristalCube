// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_RosterCardHeaderButton.h"
#include "CC_SkillSlotDragOp.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UCC_RosterCardHeaderButton::SetDragPayload(int32 InSlotIndex, UTexture2D* InIconForDrag, bool bInIsOccupied)
{
    SlotIndex = InSlotIndex;
    IconForDrag = InIconForDrag;
    bIsOccupied = bInIsOccupied;
}
