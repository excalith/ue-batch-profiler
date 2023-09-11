#include "Utilities/Utilities.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

/**
 * @brief Executes a console command
 * @param Cmd FString command
 */
void FUtilities::ExecuteCommand(const FString Cmd)
{
	const bool bSuccess = GEngine->Exec(nullptr, *Cmd, *GLog);
	// const bool bSuccess = true;
	
	if (bSuccess)
	{
		UE_LOG(LogTemp, Display, TEXT("Executed Command: %s"), *Cmd);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Could Not Execute Command: %s"), *Cmd);
	}
}

/**
 * @brief Shows a toast message in the editor
 * @param Message The message to be displayed
 * @param bIsSuccess Defines Success or Error message
 * @param FadeOutDuration How long the message should be displayed
 */
void FUtilities::ShowNotification(const FString& Message, const bool bIsSuccess, const float FadeOutDuration)
{
	FNotificationInfo NotifyInfo(FText::FromString(Message));
	NotifyInfo.bUseLargeFont = true;
	NotifyInfo.FadeOutDuration = FadeOutDuration;
		
	if (bIsSuccess)
	{
		NotifyInfo.Image = FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.SuccessWithColor.Large").GetIcon();
	}
	else
	{
		NotifyInfo.Image = FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.ErrorWithColor.Large").GetIcon();
	}

	FSlateNotificationManager::Get().AddNotification(NotifyInfo);
}
