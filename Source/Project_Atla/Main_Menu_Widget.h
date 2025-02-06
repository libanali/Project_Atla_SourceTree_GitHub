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

	//canvas panels
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* TitleCanvas;

	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* MainMenuCanvas;

	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* TutorialCanvas;

	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* WeaponSelectCanvas;

	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* SettingsCanvas;

	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* CreditsCanvas;


	// Buttons
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* PlayButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* TutorialButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* SettingsButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* CreditsButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* QuitButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* SwordButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* StaffButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* MasterAudioButton;



	// "Press Any Button" Text Block
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* PressAnyButtonText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* LegalText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* WeaponDescription;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* AttackStat;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* DefenceStat;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ElementalStat;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* WeaponLevelStat;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* FireProficiencyLevel;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* IceProficiencyLevel;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ThunderProficiencyLevel;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* VolumePercentageText;



	// Animation for fading in and out
	UPROPERTY(meta = (BindWidgetAnim), Transient)
		class UWidgetAnimation* PressAnyButtonFadeAnimation;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
		class UWidgetAnimation* TitleCanvasAnimation;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
		class UWidgetAnimation* MainMenuCanvasAnimation;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
		class UWidgetAnimation* TutorialCanvasAnimation;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
		class UWidgetAnimation* WeaponSelectCanvasAnimation;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
		class UWidgetAnimation* CreditsCanvasAnimation;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
		class UWidgetAnimation* SettingsCanvasAnimation;

	UPROPERTY(meta = (BindWidget))
		class USlider* MasterVolumeSlider;

	UPROPERTY(meta = (BindWidget))
		class UCarousel_Button_Widget* ScreenShakeToggle;


	//images
	UPROPERTY(meta = (BindWidget))
		class UImage* CharacterWeaponRender;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Images")
		class UTexture2D* SwordCharacterTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Images")
		class UTexture2D* StaffCharacterTexture;

	// Store the selected weapon type
	UPROPERTY(BlueprintReadWrite, Category = "Weapons")
		EWeaponType Weapon;



	// Functions for button click events
	UFUNCTION()
		void OnPlayClicked();

	UFUNCTION()
		void OnSettingsClicked();

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

	// Canvas Panel helper functions
	UFUNCTION()
		void InitializeCanvasPanels();

	UFUNCTION()
		void UpdateCanvasVisibility(int32 ActiveIndex);

	
	UFUNCTION()
		void OnMasterVolumeChanged(float Value);

	UFUNCTION()
		void UpdateVolumeText(float Volume);

	// Save/Load settings
	UFUNCTION()
		void SaveAudioSettings();

	UFUNCTION()
		void LoadAudioSettings();

	UFUNCTION()
		void OnScreenShakeValueChanged(const FString& NewValue);


	void UpdateWeaponStatsText(float Attack, float Defense, float ElementalAttack, int32 WeaponLevel);
	void UpdateElementalProficiencyText(EWeaponType WeaponType);
	void UpdateCharacterImage();


	UFUNCTION()
		void OnWeaponButtonHovered(const FString& Description);



	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

private:

	bool bIsOnTitleScreen;
	bool bHasSetFocusForSwordButton;
	ARen_Low_Poly_Character* GetPlayerCharacter();
	TArray<UCanvasPanel*> MenuCanvases;
};




