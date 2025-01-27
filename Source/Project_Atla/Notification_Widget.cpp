// Fill out your copyright notice in the Description page of Project Settings.


#include "Notification_Widget.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBoxSlot.h"


void UNotification_Widget::NativeConstruct()
{


    Super::NativeConstruct();



    // Check if our required widgets are properly bound
    if (!NotificationContainer)
    {
        UE_LOG(LogTemp, Warning, TEXT("NotificationContainer is not set in the widget blueprint"));
        return;
    }

    if (!Border || !NotificationText)
    {
        UE_LOG(LogTemp, Warning, TEXT("Template Border or NotificationText is not set in the widget blueprint"));
        return;
    }



    SetVisibility(ESlateVisibility::Hidden);


   // AddNotification("Item Collected 1!", 3.0f);
   // AddNotification("Item Collected 2!", 5.0f);
   // AddNotification("Item Collected 3!", 7.0f);

}




void UNotification_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

    Super::NativeTick(MyGeometry, InDeltaTime);

    // Update all active notifications
    for (int32 i = ActiveNotifications.Num() - 1; i >= 0; --i)
    {
        ActiveNotifications[i].CurrentTime += InDeltaTime;

        // If notification has expired
        if (ActiveNotifications[i].CurrentTime >= ActiveNotifications[i].Duration)
        {
            // Remove the Border (which contains the TextBlock) from the VerticalBox
            if (ActiveNotifications[i].NotificationBorder)
            {
                ActiveNotifications[i].NotificationBorder->RemoveFromParent();
            }

            // Remove this notification from our tracking array
            ActiveNotifications.RemoveAt(i);
        }
    }
}


UBorder* UNotification_Widget::CreateNotificationEntry(const FString& Message)
{
    if (!Border || !NotificationText)
    {
        UE_LOG(LogTemp, Warning, TEXT("Template Border or NotificationText is not set!"));
        return nullptr;
    }

    // Create new Border
    UBorder* NewBorder = NewObject<UBorder>(this);
    if (NewBorder)
    {
        // Get the background brush from the template border
        FSlateBrush NewBrush = Border->Background;  // Using Background instead of GetBrush()

        // Set the brush
        NewBorder->Background = NewBrush;
        NewBorder->SetBrushColor(Border->GetBrushColor());
        NewBorder->SetPadding(Border->GetPadding());
        NewBorder->SetRenderShear(FVector2D(-16.0f, 0.0f));
        //NewBorder->SetPadding(FMargin(15.0f));

        // Create new TextBlock
        UTextBlock* NewText = NewObject<UTextBlock>(this);
        if (NewText)
        {
            // Copy ALL text styling from template
            NewText->SetFont(NotificationText->GetFont());
            NewText->SetColorAndOpacity(NotificationText->GetColorAndOpacity());
            NewText->SetShadowOffset(NotificationText->GetShadowOffset());
            NewText->SetShadowColorAndOpacity(NotificationText->GetShadowColorAndOpacity());
            NewText->SetJustification(ETextJustify::Center);
            NewText->SetAutoWrapText(true);
            NewText->SetWrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping);
            NewText->SetText(FText::FromString(Message));

            // Set the TextBlock as the Border's content
            NewBorder->SetContent(NewText);
        }
    }

    return NewBorder;
}




void UNotification_Widget::AddNotification(const FString& Message, float Duration)
{
    UBorder* NewBorder = CreateNotificationEntry(Message);
    if (NewBorder && NotificationContainer)
    {
        // Get the slot and set its properties
        UVerticalBoxSlot* NotificationSlot = Cast<UVerticalBoxSlot>(NotificationContainer->AddChild(NewBorder));
        if (NotificationSlot)
        {
            // First number: Left padding
            // Second number: Top padding
            // Third number: Right padding
            // Fourth number: Bottom padding (this creates the space between notifications)
            NotificationSlot->SetPadding(FMargin(10.0f, 10.0f, 10.0f, 10.0f));

            // Make sure notification fills the width
           // NotificationSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
        }

        FNotificationInfo NewNotification(Message, Duration);
        NewNotification.NotificationBorder = NewBorder;
        ActiveNotifications.Add(NewNotification);

        SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    }
}




