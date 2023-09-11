#pragma once
#include "BatchProfilerSettings.generated.h"

UENUM(BlueprintType)
enum class EProfilingTraceChannel : uint8
{
	MemAlloc UMETA(DisplayName = "Memory Allocations"),
	MemTag UMETA(DisplayName = "Memory Tag Statistics"),
	VisualLogger UMETA(DisplayName = "Visual Logger"),
	Physics UMETA(DisplayName = "Physics"),
	Animation UMETA(DisplayName = "Animation"),
	Object UMETA(DisplayName = "Object"),
	ObjectProperties UMETA(DisplayName = "Object Properties"),
	NetworkPrediction UMETA(DisplayName = "Network Prediction"),
	RDG UMETA(DisplayName = "RDG"),
	Slate UMETA(DisplayName = "Slate"),
	Niagara UMETA(DisplayName = "Niagara"),
	Messaging UMETA(DisplayName = "Messaging"),
	TraceSourceFilters UMETA(DisplayName = "Trace Source Filters"),
	PoseSearch UMETA(DisplayName = "Pose Search"),
	Concert UMETA(DisplayName = "Concert"),
	LevelSnapshots UMETA(DisplayName = "Level Snapshots")
};

USTRUCT(BlueprintType)
struct BATCHPROFILER_API FBatchProfilerTraceSettings
{
	GENERATED_BODY()

	// Defines how many seconds to capture (Trace only)
	UPROPERTY(Config, EditAnywhere, Category="UE Insights Settings", DisplayName="Capture Seconds", meta = (DisplayOrder = "0"))
	float InsightsCaptureSeconds;

	// Array of enabled trace channels (Gpu, Bookmark, Frame, Cpu, and Log are enabled by default)
	UPROPERTY(Config, EditAnywhere, Category="UE Insights Settings", DisplayName="Enabled Trace Channels", meta = (DisplayOrder = "1"))
	TArray<EProfilingTraceChannel> EnabledTraceChannels;
};

UCLASS(config = CameraProfilrSettings)
class BATCHPROFILER_API UBatchProfilerSettings : public  UObject
{
	GENERATED_BODY()
	
public:
	explicit UBatchProfilerSettings(const FObjectInitializer& ObjectInitializer);

#pragma region Generic Settings
	// After activating camera, delays the capture for seconds
	UPROPERTY(Config, EditAnywhere, Category="Generic Settings", DisplayName="Delay Before Capture", meta = (DisplayOrder = "0"))
	float DelayBeforeEachCapture;

	// Console commands to run before capture start
	UPROPERTY(Config, EditAnywhere, Category="Generic Settings", DisplayName="Pre Capture Commands", meta = (DisplayOrder = "1"))
	TArray<FString> PreCaptureCommands;
	
	// Console commands to run after capture end
	UPROPERTY(Config, EditAnywhere, Category="Generic Settings", DisplayName="Post Capture Commands", meta = (DisplayOrder = "2"))
	TArray<FString> PostCaptureCommands;
	
	// Use custom resolution or the default
	UPROPERTY(Config, EditAnywhere, Category="Generic Settings", DisplayName="Use Custom Resolutions", meta = (DisplayOrder = "3"))
	bool UseCustomResolution;

	// Defines if the window will be fullscreen or windowed
	UPROPERTY(Config, EditAnywhere, Category="Generic Settings", DisplayName="Fullscreen Window", meta = (DisplayOrder = "4"))
	bool UseFullscreen;
	
	// Custom resolution (ie. 1920x1080)
	UPROPERTY(Config, EditAnywhere, Category="Generic Settings", DisplayName="Custom Capture Resolution", meta = (DisplayOrder = "5"))
	FIntPoint CaptureResolution;
#pragma endregion

#pragma region UE Insights Settings
	// Capture file name
	UPROPERTY(Config, EditAnywhere, Category="UE Insights Settings", DisplayName="Capture Naming", meta = (DisplayOrder = "3"))
	FString InsightsFilenameTokens;

	// Read-only references for available tokens
	UPROPERTY(VisibleAnywhere, Category="UE Insights Settings", DisplayName="Available Tokens", meta = (DisplayOrder = "4"))
	FString InsightsAvailableTokens = TEXT("{CameraName} {CameraName} {Year} {Month} {Day} {Hour} {Minute}");
	
	// UE Insights Trace Settings
	UPROPERTY(Config, EditAnywhere, Category="UE Insights Settings", DisplayName="Trace Settings", meta = (DisplayOrder = "1"))
	FBatchProfilerTraceSettings TraceSettings;
#pragma endregion

#pragma region RenderDoc Settings
	// Defines how many frames to capture using RenderDoc
	UPROPERTY(Config, EditAnywhere, Category="RenderDoc Settings", DisplayName="Frame Amount", meta = (DisplayOrder = "0"))
	int RenderDocFrameCaptureCount;
#pragma endregion

// #pragma region Key Bindings
// 	UPROPERTY(config, EditAnywhere, Category="Key Bindings",  DisplayName="Next Camera", meta = (DisplayOrder = "0"))
// 	FKey NextProfilingCameraBinding;
//
// 	UPROPERTY(config, EditAnywhere, Category="Key Bindings",  DisplayName="Previous Camera", meta = (DisplayOrder = "0"))
// 	FKey PrevProfilingCameraBinding;
//
// 	UPROPERTY(config, EditAnywhere, Category="Key Bindings",  DisplayName="Capture UE Insights", meta = (DisplayOrder = "0"))
// 	FKey StartUEInsights;
//
// 	UPROPERTY(config, EditAnywhere, Category="Key Bindings",  DisplayName="Capture RenderDoc", meta = (DisplayOrder = "0"))
// 	FKey StartRenderDoc;
// #pragma endregion
};


