// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Ren_Low_Poly_Character.h"
#include "Game_Instance.generated.h"

/**
 * 
 */


//class ARen_Low_Poly_Character;



USTRUCT(BlueprintType)
struct FGameSettings
{
    GENERATED_BODY()

        // Audio
        UPROPERTY()
        float MasterVolume = 1.0f;

    // Visual
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool bScreenShakeEnabled = true;

    UPROPERTY()
        float Brightness = 1.0f;

    // Controls
    UPROPERTY()
        bool bGamepadEnabled = false;

    UPROPERTY()
        bool bVibrationEnabled = true;

    // Language
    UPROPERTY()
        FString CurrentLanguage = TEXT("English");
};


USTRUCT(BlueprintType)
struct FLocalizedText
{
    GENERATED_BODY()

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






UCLASS()
class PROJECT_ATLA_API UGame_Instance : public UGameInstance
{
	GENERATED_BODY()


public:
    // Store the selected weapon type
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
        EWeaponType SelectedWeapon;

	
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Save Data")
        TMap<EWeaponType, FWeapon_Proficiency_Struct> WeaponProficiencyMap;

    UFUNCTION(BlueprintCallable, Category = "Save Data")
        void LoadPlayerProgress();

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Save Data")
        FWeaponElementalProficiencyMap WeaponElementalProficiency;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Save Data")
        TMap<EWeaponType, FWeaponElementalAttacks> WeaponElementalAttacks;


    UPROPERTY(BlueprintReadWrite, Category = "Weapon Stats")
        float CurrentAttack;

    UPROPERTY(BlueprintReadWrite, Category = "Weapon Stats")
        float CurrentDefense;

    UPROPERTY(BlueprintReadWrite, Category = "Weapon Stats")
        float CurrentMaxHealth;

    UPROPERTY(BlueprintReadWrite, Category = "Weapon Stats")
        float CurrentElementalAttack;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
        FGameSettings GameSettings;

    UFUNCTION()
        void SaveSettings();

    UFUNCTION()
        void LoadSettings();

    UFUNCTION(BlueprintCallable, Category = "Localization")
        FString GetLocalizedText(const FLocalizedText& TextData) const


    {

        if (GameSettings.CurrentLanguage == "French")
            return TextData.French;
        else if (GameSettings.CurrentLanguage == "German")
            return TextData.German;
        else if (GameSettings.CurrentLanguage == "Spanish")
            return TextData.Spanish;
        else if (GameSettings.CurrentLanguage == "Japanese")
            return TextData.Japanese;

        // Default to English
        return TextData.English;

    }

    UPROPERTY()
        TMap<FString, FLocalizedText> LocalizedTextMap;


    bool GetWeaponBaseStats(EWeaponType WeaponType, float& OutAttack, float& OutDefense, float& OutElementalAttack, int32& OutWeaponLevel) const;
    bool GetElementalProficiencyLevels(EWeaponType WeaponType, int32& OutFireLevel, int32& OutIceLevel, int32& OutThunderLevel) const;

};
