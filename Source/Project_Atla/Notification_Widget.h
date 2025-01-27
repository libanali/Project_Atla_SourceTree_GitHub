// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Notification_Widget.generated.h"

/**
 * 
 */

USTRUCT()
struct FNotificationInfo
{
    GENERATED_BODY()

        FString Message;
    float Duration;
    float CurrentTime;
   class UBorder* NotificationBorder;  // Reference to the border widget

    FNotificationInfo()
        : Message("")
        , Duration(3.0f)
        , CurrentTime(0.0f)
        , NotificationBorder(nullptr)
    {}

    FNotificationInfo(const FString& InMessage, float InDuration = 3.0f)
        : Message(InMessage)
        , Duration(InDuration)
        , CurrentTime(0.0f)
        , NotificationBorder(nullptr)
    {}
};



UCLASS()
class PROJECT_ATLA_API UNotification_Widget : public UUserWidget
{
	GENERATED_BODY()



public:
    // Function to add a new notification
    UFUNCTION(BlueprintCallable, Category = "Notification")
        void AddNotification(const FString& Message, float Duration = 3.0f);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // The VerticalBox that will contain our notifications
    UPROPERTY(meta = (BindWidget))
     class UVerticalBox* NotificationContainer;

    UPROPERTY(meta = (BindWidget))
    class  UTextBlock* NotificationText;

    // Template border and text for styling
    UPROPERTY(meta = (BindWidget))
    class  UBorder* Border;  // Your styled border


private:
    // Array to track active notifications
    TArray<FNotificationInfo> ActiveNotifications;

    // Creates a new TextBlock for a notification
   class UBorder* CreateNotificationEntry(const FString& Message);
};
