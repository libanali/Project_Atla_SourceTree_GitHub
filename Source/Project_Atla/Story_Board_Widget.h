// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Story_Board_Widget.generated.h"

/**
 * 
 */


class UButton;
class UWidgetAnimation;
class UBackgroundBlur;
class UTextBlock;
class UImage;
class UBorder;


UCLASS()
class PROJECT_ATLA_API UStory_Board_Widget : public UUserWidget
{
	GENERATED_BODY()
	
public:


    virtual void NativeConstruct() override;


    UPROPERTY(meta = (BindWidget))
    UBorder* BlackBorder;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* StoryText;

    UPROPERTY(meta = (BindWidget))
    UImage* LastWaveLogo;

    UPROPERTY(meta = (BindWidget))
    UImage* BackgroundImage;

    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* BlackBorderFadeAnim;

    // Configuration Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story Board")
    float StoryDisplayDuration = 4.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story Board")
    FName LevelToOpen = "YourLevelName";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story Board")
    FText StoryTextContent;

    // Typewriter configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story Board|Typewriter")
    float TypewriterSpeed = 0.05f; // Time between each character

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story Board|Typewriter")
    bool bUseTypewriterEffect = true;


    // Main function to start the story sequence
    UFUNCTION(BlueprintCallable, Category = "Story Board")
    void StartStorySequence();

    // Sequence functions
    UFUNCTION()
    void OnFadeAnimationFinished();

    UFUNCTION()
    void ShowStoryText();

    UFUNCTION()
    void OnStoryTextDisplayComplete();

    UFUNCTION()
    void OpenNextLevel();

    UFUNCTION()
    void StartTypewriterEffect();

    UFUNCTION()
    void TypeNextCharacter();

    // Timer handles
    FTimerHandle StoryTextTimerHandle;
    FTimerHandle TypewriterTimerHandle;

    // Typewriter effect variables
    FString FullStoryText;
    int32 CurrentCharIndex;
};
