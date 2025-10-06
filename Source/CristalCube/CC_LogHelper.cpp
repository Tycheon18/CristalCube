// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_LogHelper.h"

FString UCC_LogHelper::GetCleanActorName(const AActor* Actor)
{
    if (!Actor)
    {
        return TEXT("NULL");
    }

    // Ŭ���� �̸� ����
    FString ClassName = Actor->GetClass()->GetName();

    // Blueprint ���λ�/���̻� ����
    ClassName = ClassName.Replace(TEXT("BP_"), TEXT(""));
    ClassName = ClassName.Replace(TEXT("_C"), TEXT(""));

    // CristalCube ���λ絵 �����ؼ� �� ª��
    ClassName = ClassName.Replace(TEXT("CristalCube"), TEXT(""));
    ClassName = ClassName.Replace(TEXT("CC_"), TEXT(""));

    // ���� ID �߰� (������ 3�ڸ��� ���)
    int32 UniqueID = Actor->GetUniqueID() % 1000;

    return FString::Printf(TEXT("%s_%d"), *ClassName, UniqueID);
}

FString UCC_LogHelper::GetCleanClassName(const UObject* Object)
{
    if (!Object)
    {
        return TEXT("NULL");
    }

    FString ClassName = Object->GetClass()->GetName();

    // ���� ���λ�/���̻� ����
    ClassName = ClassName.Replace(TEXT("BP_"), TEXT(""));
    ClassName = ClassName.Replace(TEXT("_C"), TEXT(""));
    ClassName = ClassName.Replace(TEXT("CristalCube"), TEXT(""));
    ClassName = ClassName.Replace(TEXT("CC_"), TEXT(""));

    return ClassName;
}

FColor UCC_LogHelper::GetLogColor(const FString& InLogType)
{
    // �α� Ÿ�Ժ� �÷� ����
    if (InLogType == TEXT("PLAYER"))
    {
        return FColor::Cyan;
    }
    else if (InLogType == TEXT("ENEMY"))
    {
        return FColor::Red;
    }
    else if (InLogType == TEXT("AI"))
    {
        return FColor::Blue;
    }
    else if (InLogType == TEXT("COMBAT"))
    {
        return FColor::Orange;
    }
    else if (InLogType == TEXT("WEAPON"))
    {
        return FColor::Magenta;
    }
    else if (InLogType == TEXT("STATS"))
    {
        return FColor::Yellow;
    }
    else
    {
        return FColor::White;  // �⺻ ����
    }
}
