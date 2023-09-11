// Copyright Epic Games, Inc. All Rights Reserved.

#include "BatchProfiler.h"
#include "BatchProfilerSettings.h"
#include "ISettingsModule.h"
#include "Utilities/Utilities.h"

#pragma region Module Initialization
void FBatchProfilerModule::StartupModule()
{
	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "Batch Profiler",
										 FText::FromString("Batch Profiler"),
										 FText::FromString("Settings for batch profiler"),
										 GetMutableDefault<UBatchProfilerSettings>());
	}
	
	// Define Delegates
	FConsoleCommandWithArgsDelegate NextCameraDelegate;
	FConsoleCommandWithArgsDelegate PrevCameraDelegate;

	FConsoleCommandWithArgsDelegate StartInsightTraceDelegate;
	FConsoleCommandWithArgsDelegate BatchInsightTraceDelegate;

	FConsoleCommandWithArgsDelegate StartInsightSnapshotDelegate;
	FConsoleCommandWithArgsDelegate BatchInsightSnapshotDelegate;

	FConsoleCommandWithArgsDelegate StartRenderDocDelegate;
	FConsoleCommandWithArgsDelegate BatchRenderDocDelegate;

	// Bind Delegates
	NextCameraDelegate.BindRaw(this, &FBatchProfilerModule::NextCameraCommand);
	PrevCameraDelegate.BindRaw(this, &FBatchProfilerModule::PrevCameraCommand);

	StartInsightTraceDelegate.BindLambda([this](const TArray<FString>& Args)
	{
		StartInsightCommand(Args, false, false); // Single frame capture with UE Insights
	});
	BatchInsightTraceDelegate.BindLambda([this](const TArray<FString>& Args)
	{
		StartInsightCommand(Args, true, false); // Batch frame capture with UE Insights
	});
	
	StartInsightSnapshotDelegate.BindLambda([this](const TArray<FString>& Args)
	{
		StartInsightCommand(Args, false, true); // Single frame capture with UE Insights
	});
	BatchInsightSnapshotDelegate.BindLambda([this](const TArray<FString>& Args)
	{
		StartInsightCommand(Args, true, true); // Batch frame capture with UE Insights
	});

	StartRenderDocDelegate.BindLambda([this](const TArray<FString>& Args)
	{
		StartRenderDocCommand(Args, false); // Single frame capture with RenderDoc
	});
	BatchRenderDocDelegate.BindLambda([this](const TArray<FString>& Args)
	{
		StartRenderDocCommand(Args, true); // Batch frame capture with RenderDoc
	});

	// Register Commands 
	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("cp.next"),
		TEXT("Switches to next profiling camera"),
		NextCameraDelegate);
	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("cp.prev"),
		TEXT("Switches to previous profiling camera"),
		PrevCameraDelegate);

	
	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("cp.run.trace"),
		TEXT("Runs profiling on active camera using UE Insight"),
		StartInsightTraceDelegate);
	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("cp.batch.trace"),
		TEXT("Batch runs profiling on each ProfilingCamera using UE Insight"),
		BatchInsightTraceDelegate);

	
	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("cp.run.snapshot"),
		TEXT("Runs profiling on active camera using UE Insight"),
		StartInsightTraceDelegate);
	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("cp.batch.snapshot"),
		TEXT("Batch runs profiling on each ProfilingCamera using UE Insight"),
		BatchInsightTraceDelegate);

	
	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("cp.run.renderdoc"),
		TEXT("Runs profiling on active camera using RenderDoc"),
		StartRenderDocDelegate);
	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("cp.batch.renderdoc"),
		TEXT("Batch runs profiling on each ProfilingCamera profiling using RenderDoc"),
		BatchRenderDocDelegate);

	UE_LOG(LogTemp, Display, TEXT("Camera Profiler Initialized"));
}

/**
 * @brief Takes screenshot from active camera
 */
void FBatchProfilerModule::ShutdownModule()
{
	// Unregister the settings you registered during startup
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Batch Profiler");
	}
	
	// Unregister the console commands
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("cp.next"));
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("cp.prev"));

	IConsoleManager::Get().UnregisterConsoleObject(TEXT("cp.run.trace"));
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("cp.run.snapshot"));
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("cp.run.renderdoc"));

	IConsoleManager::Get().UnregisterConsoleObject(TEXT("cp.batch.trace"));
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("cp.batch.snapshot"));
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("cp.batch.renderdoc"));

	UE_LOG(LogTemp, Display, TEXT("Camera Profiler Shutdown"));
}
#pragma endregion

#pragma region Command Bindings
/**
 * @brief Switches to next Profiling Camera
 */
void FBatchProfilerModule::NextCameraCommand(const TArray<FString>& Args)
{
	NextCamera();
}

/**
 * @brief Switches to previous Profiling Camera
 */
void FBatchProfilerModule::PrevCameraCommand(const TArray<FString>& Args)
{
	PreviousCamera();
}

/**
 * @brief Starts profiling using Unreal Engine Insight
 * @param Args From console command (Capture Seconds)
 * @param IsBatch Should run a batch profiling
 * @param IsSnapshot True if uses snapshot instead of trace
 */
void FBatchProfilerModule::StartInsightCommand(const TArray<FString>& Args, const bool IsBatch,  const bool IsSnapshot)
{
	if (TryInitCapture(IsBatch) == false)
	{
		return;
	}
	
	if (IsInsightActive)
	{
		IsInsightActive = true;

		// Start trace capture
		FUtilities::ExecuteCommand("trace.file");
	}
	
	float CaptureSecs = BatchProfilerSettings->TraceSettings.InsightsCaptureSeconds;
	if (!IsSnapshot && Args.Num() >= 1)
	{
		CaptureSecs = FCString::Atof(*Args[0]); // The second argument is the capture duration
	}
	
	CaptureWithInsight(CaptureSecs, IsBatch, IsSnapshot);
}

/**
 * @brief Starts profiling using RenderDoc
 * @param Args From console command 
 * @param IsBatch Should run a batch profiling
 */
void FBatchProfilerModule::StartRenderDocCommand(const TArray<FString>& Args, const bool IsBatch)
{
	if (TryInitCapture(IsBatch) == false)
	{
		return;
	}

	int FrameCount = BatchProfilerSettings->RenderDocFrameCaptureCount;

	if (Args.Num() >= 1)
	{
		FrameCount = FCString::Atoi(*Args[0]);
	}

	CaptureWithRenderDoc(FrameCount, IsBatch);
}
#pragma endregion

#pragma region Capture Functions
/**
 * Tries to initialize capture by checking validity of the status and executes pre-capture commands
 * @param IsBatch Defines is batch run or single camera capture
 * @return If successfully initialized
 */
bool FBatchProfilerModule::TryInitCapture(const bool IsBatch)
{
	if (ProfilingCameras.Num() == 0)
	{
		FUtilities::ShowNotification(TEXT("No active cameras found in world"), false);
		return false;
	}

	// Execute Resolution Command
	if (BatchProfilerSettings->UseCustomResolution)
	{
		const FString FullscreenSuffix = BatchProfilerSettings->UseFullscreen ? "f" : "w";
		const FIntPoint Resolution = BatchProfilerSettings->CaptureResolution;
		const FString ResolutionCommand = FString::Printf(TEXT("r.SetRes %ix%i%s"), Resolution.X, Resolution.Y, *FullscreenSuffix);
		FUtilities::ExecuteCommand(ResolutionCommand);
	}
	
	// Execute Pre-Capture Commands	
	for (const FString& Command : BatchProfilerSettings->PreCaptureCommands)
	{
		FUtilities::ExecuteCommand(Command);
	}

	if (IsBatch)
	{
        // If batch, start from first Profiling Camera
		ActiveCamera = ProfilingCameras[0];
		CurrentCameraIndex = 0;
	}
	else
	{
	    // If not batch, check if we have Profiling Camera assigned
		if (ActiveCamera == nullptr)
		{
			FUtilities::ShowNotification(TEXT("No camera selected for profiling."), false);
			return false;
		}
	}

	return true;
}

/**
 * Captures snapshot from Active Camera using UE Insights
 * @param CaptureSecs How many seconds to capture trace
 * @param IsBatch Defines is batch run or single camera capture
 * @param IsSnapshot Captures using snapshot instead of trace
 */
void FBatchProfilerModule::CaptureWithInsight(const float CaptureSecs, const bool IsBatch, const bool IsSnapshot) const
{
	ActiveCamera->StartInsightCapture(CaptureSecs, IsBatch, IsSnapshot);
}

/**
 * Captures frame from Active Camera using RenderDoc
 * @param FrameCount How many frames to capture
 * @param IsBatch Defines is batch run or single camera capture
 */
void FBatchProfilerModule::CaptureWithRenderDoc(const int FrameCount, const bool IsBatch) const
{
	ActiveCamera->StartRenderDoc(FrameCount, IsBatch);
}

void FBatchProfilerModule::CompleteCapture() const
{
	// Execute Pre-Capture Commands	
	for (const FString& Command : BatchProfilerSettings->PostCaptureCommands)
	{
		FUtilities::ExecuteCommand(Command);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Capture Complete"));
	FUtilities::ShowNotification(TEXT("Capture Complete"), true);
}
#pragma endregion

#pragma region Register Profiling Cameras
/**
 * @brief Registers ProfilingCamera to the list of Profiling Cameras
 */
void FBatchProfilerModule::RegisterProfilingCamera(AProfilingCamera* ProfilingCamera)
{
	if (ProfilingCamera && !ProfilingCameras.Contains(ProfilingCamera))
	{
		ProfilingCameras.Add(ProfilingCamera);

		if (ProfilingCameras.Num() == 1)
		{
			ActiveCamera = ProfilingCamera;
		}
	}
}

/**
 * @brief Unregisters ProfilingCamera to the list of Profiling Cameras
 */
void FBatchProfilerModule::UnregisterProfilingCamera(AProfilingCamera* ProfilingCamera)
{
	if (ProfilingCamera && ProfilingCameras.Contains(ProfilingCamera))
	{
		ProfilingCameras.Remove(ProfilingCamera);

		if (ProfilingCameras.Num() == 0)
		{
			ActiveCamera = nullptr;
			CurrentCameraIndex = -1; // No active cameras
		}
		else if (CurrentCameraIndex >= ProfilingCameras.Num())
		{
			CurrentCameraIndex = ProfilingCameras.Num() - 1; // Set to the latest index
		}
	}
}
#pragma endregion

#pragma region Camera Switch & Caches
/**
 * Selects and activates next camera and wraps the array
 */
void FBatchProfilerModule::NextCamera()
{
	if (ProfilingCameras.Num() == 0)
	{
		CurrentCameraIndex = -1;
		ActiveCamera = nullptr;
		return;
	}

	// Increment the index and wrap around if needed
	CurrentCameraIndex = (CurrentCameraIndex + 1) % ProfilingCameras.Num();
	ActiveCamera = ProfilingCameras[CurrentCameraIndex];
	ActiveCamera->ActivateCamera();

	const FString CameraName = ActiveCamera->CameraName;
	UE_LOG(LogTemp, Display, TEXT("Switched to next ProfilingCamera: %s"), *CameraName);
}

/**
 * Selects and activates previous camera and wraps the array
 */
void FBatchProfilerModule::PreviousCamera()
{
	if (ProfilingCameras.Num() == 0)
	{
		CurrentCameraIndex = -1;
		ActiveCamera = nullptr;
		return;
	}

	// Decrement the index and wrap around if needed
	CurrentCameraIndex = (CurrentCameraIndex - 1 + ProfilingCameras.Num()) % ProfilingCameras.Num();
	ActiveCamera = ProfilingCameras[CurrentCameraIndex];
	ActiveCamera->ActivateCamera();

	const FString CameraName = ActiveCamera->CameraName;
	UE_LOG(LogTemp, Display, TEXT("Switched to previous ProfilingCamera: %s"), *CameraName);
}


/**
 * Tries to get next available camera
 * @return Returns either the camera or null if no available camera left
 */
AProfilingCamera* FBatchProfilerModule::GetNextCamera()
{
	// No cameras or invalid index
	if (ProfilingCameras.Num() == 0 || CurrentCameraIndex < 0)
	{
		ActiveCamera = nullptr;
		CurrentCameraIndex = -1;
		FUtilities::ShowNotification(TEXT("No active cameras found in world."), false);
		return nullptr;
	}

	const int32 NextIndex = CurrentCameraIndex + 1;
	if (NextIndex < ProfilingCameras.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Getting camera %i"), NextIndex);
		ActiveCamera = ProfilingCameras[NextIndex];
		CurrentCameraIndex = NextIndex;
		return ProfilingCameras[NextIndex];
	}

	// Array is complete, return nullptr
	ActiveCamera = nullptr;
	CurrentCameraIndex = 0;
	return nullptr;
}
#pragma endregion

IMPLEMENT_MODULE(FBatchProfilerModule, BatchProfiler)
