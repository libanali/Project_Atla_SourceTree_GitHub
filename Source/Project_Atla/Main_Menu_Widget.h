// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Ren_Low_Poly_Character.h"
#include "MediaPlayer.h"
#include "MediaTexture.h"
#include "MediaSource.h"
#include "Main_Menu_Widget.generated.h"

/**
 * 
 */

class UHorizontalBox;
class UTextBlock;
class UImage;
class UTexture2D;



UCLASS()
class PROJECT_ATLA_API UMain_Menu_Widget : public UUserWidget
{
	GENERATED_BODY()


public:

	virtual void NativeConstruct() override;
	void InitializeMenuButtons();
	void InitializeSettingsControls();


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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* ResetToDefaultButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* ApplyChangesButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* ControlsButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* GameplayButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* LevellingUpButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* AttributesButton;



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
		class UTextBlock* HighScore;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ThunderProficiencyLevel;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* VolumePercentageText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TutorialTitleText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TutorialDescriptionText;




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

	UPROPERTY(meta = (BindWidget))
		class UCarousel_Button_Widget* VibrationToggle;

	//UPROPERTY(meta = (BindWidget))
	//	class UCarousel_Button_Widget* LanguageToggle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		USoundBase* MenuMusic;


	UPROPERTY()
		class UAudioComponent* BackgroundMusic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		class USoundMix* MasterSoundMix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		class USoundClass* MasterSoundClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		USoundBase* NavigationSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		USoundBase* BackSound;

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


	//Containers
	UPROPERTY(meta = (BindWidget))
		class UVerticalBox* ControlsContainer;

	//Scrollbox
	UPROPERTY(meta = (BindWidget))
		class UScrollBox* TutorialScrollBox;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial|Icons")
		UTexture2D* FireIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial|Icons")
		UTexture2D* IceIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial|Icons")
		UTexture2D* ThunderIcon;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
	bool bIsDemoBuild;

	UPROPERTY(meta = (BindWidget))
	class UBorder* StaffImage;



	// Functions for button click events
	UFUNCTION()
		void OnPlayClicked();

	UFUNCTION()
		void OnTutorialClicked();

	UFUNCTION()
		void OnSettingsClicked();

	UFUNCTION()
		void OnCreditsClicked();

	UFUNCTION()
		void OnQuitClicked();

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

	UFUNCTION()
		void OnVibrationValueChanged(const FString& NewValue);

	UFUNCTION()
		void OnLanguageValueChanged(const FString& NewValue);

	UFUNCTION()
		void OnResetToDefaultClicked();

	UFUNCTION()
		void OnApplyChangesClicked();

	UFUNCTION()
		void AdjustMasterVolume(bool bIncrease);

	// Optional function to handle when audio button is focused
	UFUNCTION()
		void OnMasterAudioButtonFocused();

	void ResetSettingsToDefault();


	void UpdateWeaponStatsText(float Attack, float Defense, float ElementalAttack, int32 WeaponLevel);
	void UpdateElementalProficiencyText(EWeaponType WeaponType);
	void UpdateCharacterImage();
	void PlayNavigationSound();
	void PlayBackSound();


	UFUNCTION()
		void OnWeaponButtonHovered(const FString& Description);

	UFUNCTION()
	void OnControlsButtonHovered();

	UFUNCTION()
	void OnControlsButtonUnhovered();

	UFUNCTION()
	void OnGameplayButtonHovered();

	UFUNCTION()
	void OnLevellingUpButtonHovered();

	UFUNCTION()
	void OnAttributesButtonHovered();

	UFUNCTION()
	void OnControlsButtonFocused();

	UFUNCTION()
	void OnGameplayButtonFocused();

	UFUNCTION()
	void OnLevellingUpButtonFocused();

	UFUNCTION()
	void OnAttributesButtonFocused();

	UFUNCTION()
	void UpdateTutorialContent(const FText& Title, const FText& Description);

	UFUNCTION()
	void ScrollTutorialContent(float Value);



	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogInputEvent) override;
	virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

private:

	bool bIsOnTitleScreen;
	bool bHasSetFocusForSwordButton;
	ARen_Low_Poly_Character* GetPlayerCharacter();
	TArray<UCanvasPanel*> MenuCanvases;
	class UButton* LastFocusedButton;
	void CheckAndApplyDemoRestrictions();
};




