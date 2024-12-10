// Fill out your copyright notice in the Description page of Project Settings.


#include "Game_Over_Widget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Components/BackgroundBlur.h"
#include "Ren_Low_Poly_Character.h"
#include "Player_Save_Game.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/WidgetAnimation.h"
#include "Results_camera.h"
#include "LowPoly_Survival_GameMode.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/VerticalBox.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/Gameplaystatics.h"
#include "Kismet/KismetMathLibrary.h"




void UGame_Over_Widget::NativeConstruct()
{
    Super::NativeConstruct();


    

    SetIsFocusable(true);

	CurrentDisplayedScore = 0;

    // Set the blur strength to 0 initially
    if (BackgroundBlur)
    {
        BackgroundBlur->SetBlurStrength(CurrentBlurStrength);
    }

    // Start the blur intensity animation
    StartBlurEffect();



    if (!NotificationBox)
    {
        UE_LOG(LogTemp, Error, TEXT("NotificationBox is not set up in the widget!"));
        return;
    }

    UpdateInterval = 0.03f; // Time between updates, 0.05 seconds (50 ms) is typically smooth for animations QueuedEXPIncrement = 10.0f;
    QueuedEXPIncrement = 100.0f;
  



    // Get the player controller
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        // Set input mode to UI-only
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(TakeWidget()); // Focus this widget
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PlayerController->SetInputMode(InputMode);

        // Show the mouse cursor for UI interaction
        PlayerController->bShowMouseCursor = true;

        UE_LOG(LogTemp, Warning, TEXT("Input mode set to Game Over UI in NativeConstruct"));
    }

    else

    {

        UE_LOG(LogTemp, Error, TEXT("Failed to get PlayerController in NativeConstruct!"));

    }


   
}




void UGame_Over_Widget::StartBlurEffect()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            BlurAnimationTimer,
            this,
            &UGame_Over_Widget::UpdateBlurEffect,
            0.08f, // Timer interval
            true,
            3.5f// Loop the timer
        );

        UE_LOG(LogTemp, Log, TEXT("Started blur effect timer."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("World context is null in StartBlurEffect."));
    }
}


void UGame_Over_Widget::UpdateBlurEffect()
{
    if (BackgroundBlur)
    {
        CurrentBlurStrength += (MaxBlurStrength / BlurDuration) * 0.08f;

        if (CurrentBlurStrength >= MaxBlurStrength)
        {
            CurrentBlurStrength = MaxBlurStrength;
            GetWorld()->GetTimerManager().ClearTimer(BlurAnimationTimer);

            UE_LOG(LogTemp, Log, TEXT("Blur effect completed."));
            PlayGameOverAnimation(); // Proceed to the next step
        }

        BackgroundBlur->SetBlurStrength(CurrentBlurStrength);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("BackgroundBlur is null in UpdateBlurEffect."));
    }

}




void UGame_Over_Widget::OnGameOverTextAnimationComplete()
{

    UE_LOG(LogTemp, Log, TEXT("GameOverTextAnimation completed."));

    StartCameraFade();

}



void UGame_Over_Widget::PlayGameOverAnimation()
{
    if (GameOverTextAnimation)
    {
        float AnimationDuration = GameOverTextAnimation->GetEndTime();
        UE_LOG(LogTemp, Log, TEXT("Playing GameOverTextAnimation. Duration: %f"), AnimationDuration);

        PlayAnimation(GameOverTextAnimation);
        Game_Over_Text->SetVisibility(ESlateVisibility::Visible);

        // Set timer for the next step
        GetWorld()->GetTimerManager().SetTimer(
            AnimationTimerHandle,
            this,
            &UGame_Over_Widget::OnGameOverTextAnimationComplete,
            AnimationDuration,
            false
        );
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GameOverTextAnimation is null."));
    }

}



void UGame_Over_Widget::StartCameraFade()
{

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        UE_LOG(LogTemp, Log, TEXT("Starting camera fade to black."));
        PlayerController->ClientSetCameraFade(true, FColor::Black, FVector2D(0.0f, 1.0f), 1.0f, true, true);

        // Transition to the results camera after fade
        GetWorld()->GetTimerManager().SetTimer(
            AnimationTimerHandle,
            this,
            &UGame_Over_Widget::SwitchToResultsCamera,
            1.0f, // Match fade duration
            false,
            3.0f
        );
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerController is null in StartCameraFade."));
    }
}



void UGame_Over_Widget::SwitchToResultsCamera()
{

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController && Results_Camera)
    {
        UE_LOG(LogTemp, Log, TEXT("Switching to Results Camera."));

        PlayerController->SetViewTargetWithBlend(
            Results_Camera,  // Camera to switch to
            0.0f,            // Blend duration
            EViewTargetBlendFunction::VTBlend_Cubic
        );

        // Fade back from black
        PlayerController->ClientSetCameraFade(false, FColor::Black, FVector2D(1.0f, 0.0f), 1.0f, true, true);

        ALowPoly_Survival_GameMode* SurvivalGameMode = Cast<ALowPoly_Survival_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));

        if (SurvivalGameMode)

        {
            SurvivalGameMode->StopSpawningAndDestroyEnemies();
        }


        ARen_Low_Poly_Character* Ren = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

        if (Ren)

        {

            Ren->SpawnPlayerCharacterForRender();

        }


        // Start score fade-in animation
        GetWorld()->GetTimerManager().SetTimer(
            AnimationTimerHandle,
            this,
            &UGame_Over_Widget::PlayScoresFadeInAnimation,
            1.0f,
            false
        );


        // Start score fade-in animation
        GetWorld()->GetTimerManager().SetTimer(
            RenderImageFadeInAnimationTimerHandle,
            this,
            &UGame_Over_Widget::PlayRenderImageFadeInAnimation,
            1.0f,
            false
        );



        // Start score fade-in animation
        GetWorld()->GetTimerManager().SetTimer(
            ResultsTitleFadeInAnimationTimerHandle,
            this,
            &UGame_Over_Widget::PlayResultsTitleFadeInAnimation,
            1.0f,
            false
        );



    }
    else
    {
        if (!PlayerController)
        {
            UE_LOG(LogTemp, Error, TEXT("PlayerController is null in SwitchToResultsCamera."));
        }

        if (!Results_Camera)
        {
            UE_LOG(LogTemp, Error, TEXT("Results_Camera is null in SwitchToResultsCamera."));
        }
    }

}



void UGame_Over_Widget::PlayScoresFadeInAnimation()
{

    UE_LOG(LogTemp, Log, TEXT("Playing scores fade-in animation."));

    if (ScoreFadeInAnimation)
    {
        PlayAnimation(ScoreFadeInAnimation);

        if (FinalScoreText && HighScoreText)
        {
            FinalScoreText->SetVisibility(ESlateVisibility::Visible);
            HighScoreText->SetVisibility(ESlateVisibility::Visible);
            StartScoreAnimation();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("FinalScoreText or HighScoreText is null."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ScoreFadeInAnimation is null."));
    }

}



void UGame_Over_Widget::PlayResultsTitleFadeInAnimation()
{


    UE_LOG(LogTemp, Log, TEXT("Playing results title fade-in animation."));

    if (Results_Title_Animation)
    {
        PlayAnimation(Results_Title_Animation);

        if (Results_Title_Text)
        {
            Results_Title_Text->SetVisibility(ESlateVisibility::Visible);
            
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("results title text is null."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("results animation is null."));
    }



}



void UGame_Over_Widget::PlayRenderImageFadeInAnimation()
{


    UE_LOG(LogTemp, Log, TEXT("Playing PlayRenderImage fade-in animation."));

    if (Render_Image_Animation)
    {
        PlayAnimation(Render_Image_Animation);

        if (Render_Image)
        {
            Render_Image->SetVisibility(ESlateVisibility::Visible);

        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Render Image is null."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayRenderImage fade-in animation is null."));
    }


}



void UGame_Over_Widget::PlayButtonsFadeInAnimation()
{


    UE_LOG(LogTemp, Log, TEXT("Playing PlayRenderImage fade-in animation."));

    if (Buttons_Animation)
    {
        PlayAnimation(Buttons_Animation);

        if (MainMenu_Button && Retry_Button)
        {
            MainMenu_Button->SetVisibility(ESlateVisibility::Visible);
            Retry_Button->SetVisibility(ESlateVisibility::Visible);
            Retry_Button->SetKeyboardFocus();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Buttons is null."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Buttons fade-in animation is null."));
    }



}



void UGame_Over_Widget::SetInputModeToUI()
{

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (PlayerController)
    {
        FInputModeUIOnly InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

        PlayerController->SetInputMode(InputMode);
        PlayerController->bShowMouseCursor = true;
    }


}











void UGame_Over_Widget::ShowNotification(const FString& Message)
{
    if (!NotificationBox)
    {
        UE_LOG(LogTemp, Error, TEXT("NotificationBox is not set up in the widget!"));
        return;
    }

    // Create a new text widget for the notification
    UTextBlock* TheNotificationText = NewObject<UTextBlock>(this);

    if (!TheNotificationText)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create notification text block."));
        return;
    }

    // Set the text, font, color, and justification
    TheNotificationText->SetText(FText::FromString(Message));
    TheNotificationText->SetColorAndOpacity(FSlateColor(FLinearColor::Green)); // Customize color
  

   

    // Add the text block to the vertical box
    NotificationBox->AddChild(TheNotificationText);

    // Make the vertical box visible if it's not already
    NotificationBox->SetVisibility(ESlateVisibility::Visible);

    // Log for debugging
    UE_LOG(LogTemp, Log, TEXT("Added notification: %s"), *Message);

    // Optionally, clear notifications after a delay
    GetWorld()->GetTimerManager().SetTimer(NotificationClearTimer, this, &UGame_Over_Widget::ClearNotification, 6.0f, false);

}




void UGame_Over_Widget::ClearNotification()
{
    if (!NotificationBox)
    {
        UE_LOG(LogTemp, Error, TEXT("NotificationBox is not set up in the widget!"));
        return;
    }

    // Clear all notifications
    NotificationBox->ClearChildren();

    // Hide the vertical box
    NotificationBox->SetVisibility(ESlateVisibility::Collapsed);

    // Log for debugging
    UE_LOG(LogTemp, Log, TEXT("Cleared all notifications."));
}





void UGame_Over_Widget::StartEXPTransferAnimation()
{
    // Prevent multiple calls to this function
    if (bIsExpTransferInProgress) return;

    bIsExpTransferInProgress = true; // Mark as in progress

    ARen_Low_Poly_Character* Ren = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (!Ren) return;

    EWeaponType CurrentWeaponType = Ren->WeaponType;

    FWeapon_Proficiency_Struct* Proficiency = Ren->WeaponProficiencyMap.Find(CurrentWeaponType);

    if (!Proficiency) return;

    // Initialize values
    CurrentEXP = Proficiency->CurrentEXP;
    QueuedEXP = Ren->GetQueuedEXP();
    EXPToNextLevel = Proficiency->EXPToNextLevel;

    // Start the timer to update the EXP animation
    GetWorld()->GetTimerManager().SetTimer(EXPUpdateTimerHandle, this, &UGame_Over_Widget::UpdateEXPAnimation, UpdateInterval, true);
}





void UGame_Over_Widget::UpdateEXPAnimation()
{
    // Early exit if there is no queued EXP
    if (QueuedEXP <= 0.0f)
    {
        // Clear the timer when there is no queued EXP
        GetWorld()->GetTimerManager().ClearTimer(EXPUpdateTimerHandle);

        // Reset the flag
        bIsExpTransferInProgress = false;

        UE_LOG(LogTemp, Warning, TEXT("EXP animation finished"));

       // PlayButtonsFadeInAnimation();

        ARen_Low_Poly_Character* Ren = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

        if (Ren)

        {

            Ren->GenerateStatUpgradeMessages();

        }

      //  return;
    }

    // Calculate the amount to add to current EXP
    float AmountToAdd = FMath::Min(QueuedEXPIncrement, QueuedEXP);
    CurrentEXP += AmountToAdd;
    QueuedEXP -= AmountToAdd; // Reduce the queued EXP by the added amount
    EXPToNextLevel -= AmountToAdd; // Update the EXP to next level


    // If we reach or exceed the EXP to the next level, handle level-up
    if (EXPToNextLevel <= 0.0f)
    {
        // Calculate the excess EXP after leveling up
        float ExcessEXP = -EXPToNextLevel;

        ARen_Low_Poly_Character* Ren = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        if (Ren)
        {
            EWeaponType CurrentWeaponType = Ren->WeaponType;
            Ren->CheckWeaponLevelUp(CurrentWeaponType);
            Ren->ApplyQueuedLevelUp(CurrentWeaponType);

            // Get the proficiency data for the weapon type
            FWeapon_Proficiency_Struct* Proficiency = Ren->WeaponProficiencyMap.Find(CurrentWeaponType);

            if (!Proficiency || Proficiency->EXPToNextLevel <= 0)
            {
                UE_LOG(LogTemp, Error, TEXT("Invalid Proficiency data for WeaponType: %d"), static_cast<int32>(CurrentWeaponType));
                return;
            }

            // Add the excess EXP to the current proficiency
            Proficiency->CurrentEXP += ExcessEXP;
            EXPToNextLevel = Proficiency->EXPToNextLevel;

            // Save the player's progress after the level-up
            Ren->SavePlayerProgress();
            OnQueuedEXPAdded();
        }
    }


    // Update the UI with the new values
    if (CurrentEXPText)
    {
        CurrentEXPText->SetText(FText::FromString(FString::Printf(TEXT("Current EXP: %.0f"), CurrentEXP)));
        CurrentEXPText->SetVisibility(ESlateVisibility::Visible);
    }

    if (QueuedEXPText)
    {
        QueuedEXPText->SetText(FText::FromString(FString::Printf(TEXT("EXP Earned: %.0f"), QueuedEXP)));
        QueuedEXPText->SetVisibility(ESlateVisibility::Visible);
    }

    if (EXPToNextLevelText)
    {
        EXPToNextLevelText->SetText(FText::FromString(FString::Printf(TEXT("EXPToNextLevel: %.0f"), EXPToNextLevel)));
        EXPToNextLevelText->SetVisibility(ESlateVisibility::Visible);
    }
}



void UGame_Over_Widget::OnQueuedEXPAdded()
{
    UE_LOG(LogTemp, Log, TEXT("OnQueuedEXPAdded called"));

    ARen_Low_Poly_Character* Ren = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (!Ren)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to cast to ARen_Low_Poly_Character."));
        return;
    }

    EWeaponType CurrentWeaponType = Ren->WeaponType;
    FWeapon_Proficiency_Struct* Proficiency = Ren->WeaponProficiencyMap.Find(CurrentWeaponType);
    if (!Proficiency)
    {
        UE_LOG(LogTemp, Error, TEXT("WeaponProficiencyMap does not contain data for WeaponType: %d"), static_cast<int32>(CurrentWeaponType));
        return;
    }

    if (WeaponLevelText)
    {
        WeaponLevelText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), Proficiency->WeaponLevel)));
        WeaponLevelText->SetVisibility(ESlateVisibility::Visible);
    }

    if (CurrentEXPText)
    {
        CurrentEXPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Proficiency->CurrentEXP, Proficiency->EXPToNextLevel)));
        CurrentEXPText->SetVisibility(ESlateVisibility::Visible);
    }

    // Clear UnlockedTechniques to ensure only current level-up techniques are displayed
    UnlockedTechniques.Empty();
    UE_LOG(LogTemp, Log, TEXT("Cleared UnlockedTechniques"));

    // Check if there are any queued techniques to collect
    if (Ren->QueuedUnlockTechniques.Num() > 0)
    {
        for (const FString& TechniqueName : Ren->QueuedUnlockTechniques)
        {
            UnlockedTechniques.Add(TechniqueName); // Collect the unlocked technique names
            UE_LOG(LogTemp, Log, TEXT("Collected unlocked technique: %s"), *TechniqueName);  // Debug log to verify techniques are collected
        }

        Ren->UnlockQueuedTechniques();  // Unlock techniques after collecting for debug log
        UE_LOG(LogTemp, Log, TEXT("Unlocked queued techniques"));

        Ren->QueuedUnlockTechniques.Empty(); // Clear the queue after collecting
        UE_LOG(LogTemp, Log, TEXT("Cleared QueuedUnlockTechniques"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No techniques to collect"));
    }

    StartEXPTransferAnimation();
}









void UGame_Over_Widget::SkipScoreAnimation()
{

    // Check if FinalScoreText and HighScoreText are both valid and visible
    if (FinalScoreText && FinalScoreText->IsVisible() && HighScoreText && HighScoreText->IsVisible())
    {
        // Immediately set the displayed score to the target score
        CurrentDisplayedScore = TargetScore;

        // Clear the timer (stop any ongoing updates)
        GetWorld()->GetTimerManager().ClearTimer(ScoreUpdateTimer);

        // Update the score display immediately
        if (FinalScoreText)
        {
            FinalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Final Score: %d"), CurrentDisplayedScore)));
        }

        // Log for debugging
        UE_LOG(LogTemp, Log, TEXT("Score animation skipped"));

        // Trigger the character's score reaction animation and high score update
        ARen_Low_Poly_Character* Ren = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

        if (Ren)
        {
            Ren->Score_Reaction_Anim();
            Ren->UpdateHighScore(TargetScore);  // Update high score here
            if (HighScoreText)
            {
                HighScoreText->SetText(FText::FromString(FString::Printf(TEXT("High Score: %d"), TargetScore)));
            }

            OnQueuedEXPAdded();
            UpdateEXPAnimation();
        }
    }
    else
    {
        // Log if trying to skip when the texts are not visible
        UE_LOG(LogTemp, Warning, TEXT("Cannot skip animation, texts are not visible"));
    }

}



void UGame_Over_Widget::SkipEXPTransferAnimation()
{

    if (WeaponLevelText && WeaponLevelText->IsVisible() && CurrentEXPText && CurrentEXPText->IsVisible() && QueuedEXPText && QueuedEXPText->IsVisible() && EXPToNextLevelText && EXPToNextLevelText->IsVisible())

    {


        bIsExpTransferInProgress = false;
        UE_LOG(LogTemp, Warning, TEXT("Skipped EXP transfer animaiton"));


    }

    else


    {

        UE_LOG(LogTemp, Warning, TEXT("Cannot skip animation, exp texts are not visible"));

    }




}



void UGame_Over_Widget::ShowStatsUpgradeNotification(const TArray<FString>& StatMessages)
{
    UE_LOG(LogTemp, Log, TEXT("ShowStatsUpgradeNotification called"));

    if (StatMessages.Num() == 0)
    {
        if (StatUpgradeNotificationBorder)
        {
            StatUpgradeNotificationBorder->SetVisibility(ESlateVisibility::Hidden);
        }

        if (UnlockedTechniques.Num() > 0)
        {
            ShowTechniqueNotification(UnlockedTechniques);
        }
        else
        {
            PlayButtonsFadeInAnimation();
        }

        UE_LOG(LogTemp, Warning, TEXT("No stat upgrades to display!"));
        return;
    }

    FString CombinedMessage = "Attributes\n";
    for (const FString& Message : StatMessages)
    {
        CombinedMessage += Message + TEXT("\n");
    }

    if (StatsUpgrade_Animation)
    {
        PlayAnimation(StatsUpgrade_Animation, 1.0f);
    }

    if (StatUpgradeTextBlock)
    {
        StatUpgradeTextBlock->SetText(FText::FromString(CombinedMessage));
        StatUpgradeTextBlock->SetVisibility(ESlateVisibility::Visible);
        UE_LOG(LogTemp, Log, TEXT("Stats displayed"));
    }

    if (StatUpgradeNotificationBorder)
    {
        StatUpgradeNotificationBorder->SetVisibility(ESlateVisibility::Visible);
    }

    GetWorld()->GetTimerManager().SetTimer(
        StatUpgradeNotificationTimerHandle,
        this,
        &UGame_Over_Widget::RemoveStatsUpgradeNotification,
        2.0f,
        false
    );
}







void UGame_Over_Widget::RemoveStatsUpgradeNotification()
{
    UE_LOG(LogTemp, Log, TEXT("RemoveStatsUpgradeNotification called"));

    if (StatUpgradeNotificationBorder && StatUpgradeTextBlock)
    {
        PlayAnimationReverse(StatsUpgrade_Animation, 1.0f);
    }

    if (UnlockedTechniques.Num() > 0)
    {
        ShowTechniqueNotification(UnlockedTechniques);
    }
    else
    {
        PlayButtonsFadeInAnimation();
    }
}



void UGame_Over_Widget::ShowTechniqueNotification(const TArray<FString>& TechniqueMessages)
{
    UE_LOG(LogTemp, Log, TEXT("Entering ShowTechniqueNotification with %d techniques"), TechniqueMessages.Num());

    if (TechniqueMessages.Num() == 0)
    {
        if (StatUpgradeNotificationBorder)
        {
            StatUpgradeNotificationBorder->SetVisibility(ESlateVisibility::Hidden);
        }

        PlayButtonsFadeInAnimation();
        UE_LOG(LogTemp, Warning, TEXT("No techniques to display!"));
        return;
    }

    FString CombinedMessage = "Techniques\n";
    for (const FString& Technique : TechniqueMessages)
    {
        CombinedMessage += FString::Printf(TEXT("Unlocked: %s\n"), *Technique);
    }

    if (StatsUpgrade_Animation)
    {
        PlayAnimation(StatsUpgrade_Animation, 1.0f);
    }

    if (StatUpgradeTextBlock)
    {
        StatUpgradeTextBlock->SetText(FText::FromString(CombinedMessage));
        StatUpgradeTextBlock->SetVisibility(ESlateVisibility::Visible);
        UE_LOG(LogTemp, Log, TEXT("Techniques displayed"));
    }

    if (StatUpgradeNotificationBorder)
    {
        StatUpgradeNotificationBorder->SetVisibility(ESlateVisibility::Visible);
    }

    GetWorld()->GetTimerManager().SetTimer(
        StatUpgradeNotificationTimerHandle,
        this,
        &UGame_Over_Widget::RemoveTechniqueNotification,
        5.0f,
        false
    );
}







void UGame_Over_Widget::RemoveTechniqueNotification()
{


    // Reverse the stats upgrade animation and hide the notification
    if (StatUpgradeNotificationBorder && StatUpgradeTextBlock)
    {
        PlayAnimationReverse(StatsUpgrade_Animation, 1.0f);
    }

    // After techniques notification, proceed to the next step (e.g., show buttons or return to menu)
    PlayButtonsFadeInAnimation();




}








FReply UGame_Over_Widget::NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{

     // If the Gamepad_Bottom FaceButton is pressed, we handle both skip actions
    if (InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Bottom) 
    {
        // Skip Score Animation if possible
        SkipScoreAnimation();

        return FReply::Handled(); // Indicate the key press has been handled
    }

    // If not the correct key, continue with normal behavior
    return FReply::Unhandled();
}








void UGame_Over_Widget::SetUpGameOverUI(int32 FinalScore, int32 HighScore)
{
    // Set the target score to the player's final score
    TargetScore = FinalScore;

    // Immediately set the high score
    if (HighScoreText)
    {
        HighScoreText->SetText(FText::FromString(FString::Printf(TEXT("High Score: %d"), HighScore)));
    }

    // Directly set CurrentDisplayedScore to TargetScore to display the final score immediately
   // CurrentDisplayedScore = TargetScore;

    // Update the UI to show the final score
    if (FinalScoreText)
    {
        FinalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Final Score: %d"), CurrentDisplayedScore)));
    }
}



void UGame_Over_Widget::UpdateDisplayedScore()
{
    // Increment the displayed score by ScoreStep
    CurrentDisplayedScore++;

    // Cap the displayed score at the target score
    if (CurrentDisplayedScore >= TargetScore)
    {
        CurrentDisplayedScore = TargetScore;

        // Stop the timer when the final score is reached
        GetWorld()->GetTimerManager().ClearTimer(ScoreUpdateTimer);

       // PlayButtonsFadeInAnimation();

        ARen_Low_Poly_Character* Ren = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

        if (Ren)
        {
            Ren->Score_Reaction_Anim();
            // Now that the animation is complete, update the high score
            Ren->UpdateHighScore(TargetScore);  // Update high score here

            HighScoreText->SetText(FText::FromString(FString::Printf(TEXT("High Score: %d"), TargetScore)));

            OnQueuedEXPAdded();

            UpdateEXPAnimation();

        }
    }

    // Update the final score text with the current displayed score
    if (FinalScoreText)
    {
        FinalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Final Score: %d"), CurrentDisplayedScore)));
    }

}



void UGame_Over_Widget::StartScoreAnimation()
{

    // Ensure the timer is cleared before starting a new animation
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(ScoreUpdateTimer);

        // Start a timer to increment the displayed score
        GetWorld()->GetTimerManager().SetTimer(
            ScoreUpdateTimer,
            this,
            &UGame_Over_Widget::UpdateDisplayedScore,
            0.03f,
            true // Loop the timer
        );
    }


}
