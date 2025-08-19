// Fill out your copyright notice in the Description page of Project Settings.


#include "Story_Board_Widget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"


void UStory_Board_Widget::NativeConstruct()
{

    Super::NativeConstruct();

    // Initialize typewriter variables
    CurrentCharIndex = 0;
    FullStoryText = "";

    // Initialize widget state
    if (BlackBorder)
    {
        // Ensure black border starts fully opaque
        BlackBorder->SetRenderOpacity(1.0f);
    }

    if (StoryText)
    {
        // Hide story text initially
        StoryText->SetVisibility(ESlateVisibility::Hidden);

        // Set the story text content if configured
        if (!StoryTextContent.IsEmpty())
        {
            StoryText->SetText(StoryTextContent);
        }
    }


}



void UStory_Board_Widget::StartStorySequence()
{

    // Check if the fade animation exists
    if (!BlackBorderFadeAnim)
    {
        UE_LOG(LogTemp, Warning, TEXT("BlackBorderFadeAnim is not set in Story_Board_Widget"));
        // Fallback: directly show text if animation is missing
        ShowStoryText();
        return;
    }

    // Bind to animation finished event
    FWidgetAnimationDynamicEvent AnimationFinishedDelegate;
    AnimationFinishedDelegate.BindDynamic(this, &UStory_Board_Widget::OnFadeAnimationFinished);
    BindToAnimationFinished(BlackBorderFadeAnim, AnimationFinishedDelegate);

    // Play the fade animation (alpha goes from 1 to 0)
    PlayAnimation(BlackBorderFadeAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);

}




void UStory_Board_Widget::OnFadeAnimationFinished()
{

    if (!StoryText)
    {
        UE_LOG(LogTemp, Warning, TEXT("StoryText is null in Story_Board_Widget"));
        OpenNextLevel();
        return;
    }

    // Make story text visible
    StoryText->SetVisibility(ESlateVisibility::Visible);

    if (bUseTypewriterEffect)
    {
        // Start typewriter effect
        StartTypewriterEffect();
    }
    else
    {
        // Show all text immediately
        StoryText->SetText(StoryTextContent);

        // Start timer to wait for the specified duration
        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().SetTimer(
                StoryTextTimerHandle,
                this,
                &UStory_Board_Widget::OnStoryTextDisplayComplete,
                StoryDisplayDuration,
                false
            );
        }
    }

}




void UStory_Board_Widget::ShowStoryText()
{

    if (!StoryText)
    {
        UE_LOG(LogTemp, Warning, TEXT("StoryText is null in Story_Board_Widget"));
        OpenNextLevel();
        return;
    }

    // Make story text visible
    StoryText->SetVisibility(ESlateVisibility::Visible);



    // Start timer to wait for the specified duration
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            StoryTextTimerHandle,
            this,
            &UStory_Board_Widget::OnStoryTextDisplayComplete,
            StoryDisplayDuration,
            false
        );
    }


}



void UStory_Board_Widget::OnStoryTextDisplayComplete()
{


    OpenNextLevel();


}




void UStory_Board_Widget::OpenNextLevel()
{

    // Clear any active timers
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(StoryTextTimerHandle);
    }

    // Optional: Remove this widget from viewport
    RemoveFromParent();

    // Open the specified level
    if (!LevelToOpen.IsNone())
    {
        UGameplayStatics::OpenLevel(GetWorld(), LevelToOpen);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("LevelToOpen is not set in Story_Board_Widget"));
    }


}

void UStory_Board_Widget::StartTypewriterEffect()
{
    if (!StoryText)
    {
        return;
    }

    // Store the full text and clear the text block
    FullStoryText = StoryTextContent.ToString();
    CurrentCharIndex = 0;
    StoryText->SetText(FText::FromString(""));

    // Start the typewriter timer
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            TypewriterTimerHandle,
            this,
            &UStory_Board_Widget::TypeNextCharacter,
            TypewriterSpeed,
            true // Repeat
        );
    }
}

void UStory_Board_Widget::TypeNextCharacter()
{

    if (!StoryText || CurrentCharIndex >= FullStoryText.Len())
    {
        // Typewriter effect complete
        if (UWorld* World = GetWorld())
        {
            // Clear typewriter timer
            World->GetTimerManager().ClearTimer(TypewriterTimerHandle);

            // Start the display duration timer
            World->GetTimerManager().SetTimer(
                StoryTextTimerHandle,
                this,
                &UStory_Board_Widget::OnStoryTextDisplayComplete,
                StoryDisplayDuration,
                false
            );
        }
        return;
    }

    // Add the next character(s) to the displayed text
    CurrentCharIndex++;

    // Check if we should add multiple characters for faster display of spaces
    // This makes the typewriter effect feel more natural
    if (CurrentCharIndex < FullStoryText.Len() && FullStoryText[CurrentCharIndex - 1] == ' ')
    {
        // If we just typed a space, immediately type the next character too
        while (CurrentCharIndex < FullStoryText.Len() && FullStoryText[CurrentCharIndex] == ' ')
        {
            CurrentCharIndex++;
        }
    }

    // Update the displayed text
    FString DisplayedText = FullStoryText.Left(CurrentCharIndex);
    StoryText->SetText(FText::FromString(DisplayedText));

}
