// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "Localization_Manager.generated.h"

USTRUCT(BlueprintType)
struct FLocalizationEntry : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Key;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString English;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString French;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString German;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Spanish;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Japanese;
};

/**
 * Basic localization manager
 */
UCLASS(BlueprintType, Blueprintable)
class PROJECT_ATLA_API ULocalization_Manager : public UObject
{
    GENERATED_BODY()

public:

    ULocalization_Manager();

    // Initialize with a data table
    void Initialize(UDataTable* InLocalizationTable);

    // Get localized text by key
    FText GetLocalizedText(const FString& Key) const;

    // Set current language
    void SetCurrentLanguage(const FString& NewLanguage);

    // Get current language
    FString GetCurrentLanguage() const { return CurrentLanguage; };

private:
    UPROPERTY()
    UDataTable* LocalizationTable;

    UPROPERTY()
    FString CurrentLanguage;

};