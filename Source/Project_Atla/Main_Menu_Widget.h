// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Ren_Low_Poly_Character.h"
#include "Main_Menu_Widget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UMain_Menu_Widget : public UUserWidget
{
	GENERATED_BODY()


public:

	virtual void NativeConstruct() override;




protected:
		// Widget Switcher
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UWidgetSwitcher* WidgetSwitcher;

	// Buttons
	UPROPERTY(meta = (BindWidget))
		class UButton* PlayButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* SettingsButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* CreditsButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* BackButton;

	UPROPERTY(meta = (BindWidget))
		class UBorder* BackgroundImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* SwordButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* StaffButton;

	// "Press Any Button" Text Block
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* PressAnyButtonText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* LegalText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* WeaponSelectTitle;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* AttackStat;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* DefenceStat;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ElementalStat;


	// Animation for fading in and out
	UPROPERTY(meta = (BindWidgetAnim), Transient)
		class UWidgetAnimation* FadeAnimation;


	// Store the selected weapon type
	UPROPERTY(BlueprintReadWrite, Category = "Weapons")
		EWeaponType Weapon;



	// Functions for button click events
	UFUNCTION()
		void OnPlayClicked();

	UFUNCTION()
		void OnBackClicked();

	UFUNCTION()
		void OnSwordButtonClicked();

	UFUNCTION()
		void OnSwordButtonHovered();

	UFUNCTION()
		void OnSwordButtonFocused();

	UFUNCTION()
		void OnStaffButtonClicked();

	UFUNCTION()
		void OnStaffButtonHovered();

	UFUNCTION()
		void OnStaffButtonFocused();

	UFUNCTION()
		void HandleGoBack();

	UFUNCTION()
		void SwitchToMainMenu();

	UFUNCTION()
		void SwitchToWeaponSelectMenu();

	UFUNCTION()
		void UpdateWeaponStats(EWeaponType WeaponType);


	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

private:

	bool bIsOnTitleScreen;
	bool bHasSetFocusForSwordButton;
};




