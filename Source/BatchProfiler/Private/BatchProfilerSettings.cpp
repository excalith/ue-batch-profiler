#include "BatchProfilerSettings.h"

UBatchProfilerSettings::UBatchProfilerSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Generic settings
	DelayBeforeEachCapture = 5.0f;
	
	// Command Settings
	PreCaptureCommands.Add("stat fps");
	PreCaptureCommands.Add("stat unit");

	PostCaptureCommands.Add("stat fps");
	PostCaptureCommands.Add("stat unit");

	// Resolution Settings
	UseCustomResolution = false;
	UseFullscreen = true;
	CaptureResolution = FIntPoint(1920, 1080);
	
	// UE Insights settings
	InsightsFilenameTokens = TEXT("{CameraName}_{Year}.{Month}.{Day}_{Hour}.{Minute}");
	InsightsAvailableTokens = TEXT("{CameraName} {CameraName} {Year} {Month} {Day} {Hour} {Minute}");

	// UE Insights Trace settings
	TraceSettings.InsightsCaptureSeconds = 5.0f;
	
	// RenderDoc Settings
	RenderDocFrameCaptureCount = 1;
}
