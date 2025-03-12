// Fill out your copyright notice in the Description page of Project Settings.


#include "Localization_Manager.h"



ULocalization_Manager::ULocalization_Manager()
{
	// Default to English
	CurrentLanguage = "English";
}

void ULocalization_Manager::Initialize(UDataTable* InLocalizationTable)
{

    if (InLocalizationTable)
    {
        LocalizationTable = InLocalizationTable;
        UE_LOG(LogTemp, Log, TEXT("Localization Manager initialized with %d entries"),
            LocalizationTable->GetRowMap().Num());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to initialize Localization Manager: Invalid table!"));
    }


}

FText ULocalization_Manager::GetLocalizedText(const FString& Key) const
{

    if (!LocalizationTable)
    {
        UE_LOG(LogTemp, Error, TEXT("Localization table not set when looking up key: %s"), *Key);
        return FText::FromString(Key);
    }

    // Find the row with the matching key
    for (auto& Pair : LocalizationTable->GetRowMap())
    {
        FLocalizationEntry* Entry = (FLocalizationEntry*)Pair.Value;
        if (Entry && Entry->Key == Key)
        {
            // Return the text based on current language
            if (CurrentLanguage == "English")
                return FText::FromString(Entry->English);
            else if (CurrentLanguage == "French")
                return FText::FromString(Entry->French);
            else if (CurrentLanguage == "German")
                return FText::FromString(Entry->German);
            else if (CurrentLanguage == "Spanish")
                return FText::FromString(Entry->Spanish);
            else if (CurrentLanguage == "Japanese")
                return FText::FromString(Entry->Japanese);
            else
                return FText::FromString(Entry->English); // Default to English
        }
    }

    // If we couldn't find the key, return the key itself
    UE_LOG(LogTemp, Warning, TEXT("Localization key not found: %s"), *Key);
    return FText::FromString(Key);
}

void ULocalization_Manager::SetCurrentLanguage(const FString& NewLanguage)
{

    if (CurrentLanguage != NewLanguage)
    {
        CurrentLanguage = NewLanguage;
        UE_LOG(LogTemp, Log, TEXT("Game language changed to: %s"), *CurrentLanguage);
    }

}
