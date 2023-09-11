#pragma once

class FUtilities
{
public:
	static void ExecuteCommand(const FString Cmd);
	static void ShowNotification(const FString& Message, bool bIsSuccess, const float FadeOutDuration = 7.f);
};
