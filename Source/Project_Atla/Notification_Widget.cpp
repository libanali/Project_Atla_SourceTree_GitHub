// Fill out your copyright notice in the Description page of Project Settings.


#include "Notification_Widget.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetTree.h"


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


    //SetVisibility(ESlateVisibility::Collapsed);


    AddNotification("Item Collected 1!", 3.0f);
    AddNotification("Item Collected 2!", 5.0f);
    AddNotification("Item Collected 3!", 7.0f);


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
            NewText->SetText(FText::FromString(Message));

            // Set the TextBlock as the Border's content
            NewBorder->SetContent(NewText);
        }
    }

    return NewBorder;
}




void UNotification_Widget::AddNotification(const FString& Message, float Duration)
{
    // Create new Border containing the styled TextBlock for this notification
    UBorder* NewBorder = CreateNotificationEntry(Message);
    if (NewBorder && NotificationContainer)
    {
        // Add the Border to the top of the VerticalBox
        NotificationContainer->AddChildToVerticalBox(NewBorder);
        // Create and add notification info to our tracking array
        FNotificationInfo NewNotification(Message, Duration);
        NewNotification.NotificationBorder = NewBorder;
        ActiveNotifications.Add(NewNotification);
        SetVisibility(ESlateVisibility::Visible);  // or just Visible if you prefer

    }
}




