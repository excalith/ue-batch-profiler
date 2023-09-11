// Fill out your copyright notice in the Description page of Project Settings.

#include "ProfilingCamera.h"
#include "BatchProfiler.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Utilities/Utilities.h"
#include "BatchProfilerSettings.h"

#pragma region Constructor & Base Overrides
AProfilingCamera::AProfilingCamera()
	: IsActiveOnProfiling(false), CameraName(TEXT("")), ProfilerModule(nullptr), ProfilingCameraComponent(nullptr)
{
	// Set active on profiling by default
	IsActiveOnProfiling = true;
	
	// Set camera component properties
	ProfilingCameraComponent = GetCameraComponent();
	if (ProfilingCameraComponent)
	{
		ProfilingCameraComponent->bVisualizeComponent = true; // Show the camera component in the editor viewport
		ProfilingCameraComponent->bVisualizeComponent = true; // This is to show the camera in the editor viewport
		ProfilingCameraComponent->bEditableWhenInherited = true; // Make the camera component editable when inherited
		ProfilingCameraComponent->Deactivate();
	}
	
	IsActiveOnProfiling = false; // Initialize the IsActiveOnProfiling property
	if (CameraName.IsEmpty())
	{
		CameraName = GetActorLabel(); // Initialize the CustomActorName property with ActorLabel
	}

	// Retrieve the instance of the FBatchProfilerModule
	ProfilerModule = FModuleManager::Get().GetModulePtr<FBatchProfilerModule>("BatchProfiler");
}

void AProfilingCamera::BeginPlay()
{
	Super::BeginPlay();

	// Register camera if IsActiveOnProfiling is true
	if (IsActiveOnProfiling && ProfilingCameraComponent && ProfilingCameraComponent->IsValidLowLevel())
	{
		RegisterCamera();
	}
}

void AProfilingCamera::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Unregister camera if IsActiveOnProfiling is true
	if (IsActiveOnProfiling && ProfilingCameraComponent && ProfilingCameraComponent->IsValidLowLevel())
	{
		UnregisterCamera();
	}
}

void AProfilingCamera::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Check if IsActiveOnProfiling has changed
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AProfilingCamera, IsActiveOnProfiling))
	{
		if (IsActiveOnProfiling)
		{
			RegisterCamera();
		}
		else
		{
			UnregisterCamera();
		}
	}
}
#pragma endregion

#pragma region Camera Registration
void AProfilingCamera::RegisterCamera()
{
	if (ProfilerModule)
	{
		ProfilerModule->RegisterProfilingCamera(this);
		UE_LOG(LogTemp, Warning, TEXT("Registering Camera %s"), *CameraName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FBatchProfilerModule instance not found."));
	}
}

void AProfilingCamera::UnregisterCamera()
{
	if (ProfilerModule)
	{
		ProfilerModule->UnregisterProfilingCamera(this);\
		UE_LOG(LogTemp, Warning, TEXT("Unregistering Camera %s"), *CameraName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FBatchProfilerModule instance not found."));
	}
}
#pragma endregion

#pragma region Camera Activation
void AProfilingCamera::ActivateCamera()
{
	if (ProfilingCameraComponent && ProfilingCameraComponent->IsValidLowLevel())
	{
		ProfilingCameraComponent->Activate();
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			PlayerController->SetViewTarget(this);
		}
	}
}

void AProfilingCamera::DeactivateCamera() const
{
	if (ProfilingCameraComponent && ProfilingCameraComponent->IsValidLowLevel())
	{
		ProfilingCameraComponent->Deactivate();
	}
}
#pragma endregion 

#pragma region UE Insight Capture
void AProfilingCamera::StartInsightCapture(const float CaptureSecs, bool IsBatch, bool IsSnapshot)
{
	// Early exit if conditions are not met
	if (!(ProfilingCameraComponent && ProfilingCameraComponent->IsValidLowLevel()))
	{
		return; 
	}

	UE_LOG(LogTemp, Warning, TEXT("Capturing From: %s"), *CameraName)

	// Activate camera
	ActivateCamera();

	// Show camera name on screen
	const float Delay = BatchProfilerSettings->DelayBeforeEachCapture;
	GEngine->AddOnScreenDebugMessage(31419, Delay + 1.0f, FColor::Green, FString::Printf(TEXT("Current Camera: %s"), *CameraName));

	// Capture after delay defined in Project Settings
	FTimerHandle DelayInsightCaptureHandle;
	FTimerDelegate DelayInsightCaptureDelegate;
	DelayInsightCaptureDelegate.BindLambda([this, CaptureSecs, IsBatch, IsSnapshot]() {
		CaptureInsight(CaptureSecs, IsBatch, IsSnapshot);
	});
	
	// Set up a timer to call the bound function after a delay
	GetWorldTimerManager().SetTimer(DelayInsightCaptureHandle, DelayInsightCaptureDelegate, Delay, false);
}

void AProfilingCamera::CaptureInsight(const float CaptureSecs, bool IsBatch, bool IsSnapshot) const
{
	// Take snapshot and save
	const FString FileName = GetFilename();

	if (IsSnapshot)
	{
		const FString SnapshotCommand = FString::Printf(TEXT("trace.snapshotfile %s_Snapshot"), *FileName);
		FUtilities::ExecuteCommand(SnapshotCommand);
	}
	else
	{
		FString EnabledTraceChannels;
		const UEnum* TraceEnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EProfilingTraceChannel"), true);
		for (const EProfilingTraceChannel TraceChannel : BatchProfilerSettings->TraceSettings.EnabledTraceChannels)
		{
			FString TraceChannelString = TraceEnumPtr->GetDisplayNameTextByValue(static_cast<int64>(TraceChannel)).ToString();
			EnabledTraceChannels += TraceChannelString + TEXT(" ");
		}
		
		const FString TraceCommand = FString::Printf(TEXT("trace.file %s_Trace %s"), *FileName, *EnabledTraceChannels);
		FUtilities::ExecuteCommand(TraceCommand);
		FUtilities::ExecuteCommand("trace.screenshot");
	}
	
	// Call StopInsight after 1 seconds
	FTimerHandle CaptureTimerHandle;
	FTimerDelegate StopInsightDelegate;
	StopInsightDelegate.BindLambda([this, CaptureSecs, IsBatch, IsSnapshot]() {
		if (IsBatch)
		{
			const AProfilingCamera* NextCamera = ProfilerModule->GetNextCamera();
			if (NextCamera != nullptr)
			{
				ProfilerModule->CaptureWithInsight(CaptureSecs, IsBatch, IsSnapshot);
			}
			else
			{
				ProfilerModule->CompleteCapture();
			}

			// Stop trace
			if (!IsSnapshot)
			{
				FUtilities::ExecuteCommand("trace.stop");
			}
		}
		else
		{
			// Stop trace
			if (!IsSnapshot)
			{
				FUtilities::ExecuteCommand("trace.stop");
			}
			
			ProfilerModule->CompleteCapture();
		}
	});
	
	GetWorldTimerManager().SetTimer(CaptureTimerHandle, StopInsightDelegate, CaptureSecs, false);
}
#pragma endregion

#pragma region RenderDoc Capture
void AProfilingCamera::StartRenderDoc(const int FrameCount, bool IsBatch)
{
	// Early exit if conditions are not met
	if (!(ProfilingCameraComponent && ProfilingCameraComponent->IsValidLowLevel()))
	{
		return; 
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Capturing From: %s"), *CameraName)
	
	// Activate and possess the camera
	ActivateCamera();

	// Show Camera Name On Screen
	const float Delay = BatchProfilerSettings->DelayBeforeEachCapture;
	GEngine->AddOnScreenDebugMessage(31419, Delay + 1.0f, FColor::Green, FString::Printf(TEXT("Current Camera: %s"), *CameraName));

	// Capture after delay defined in Project Settings
	FTimerHandle DelayRenderDocCaptureHandle;
	FTimerDelegate DelayRenderDocCaptureDelegate;
	DelayRenderDocCaptureDelegate.BindLambda([this, FrameCount, IsBatch]() {
		CaptureRenderDoc(FrameCount, IsBatch);
	});
	
	// Set up a timer to call the bound function after a delay
	GetWorldTimerManager().SetTimer(DelayRenderDocCaptureHandle, DelayRenderDocCaptureDelegate, Delay, false);
}

void AProfilingCamera::CaptureRenderDoc(const int FrameCount, bool IsBatch) const
{
	// Execute RenderDoc capture
	const FString Command = FString::Printf(TEXT("renderdoc.CaptureFrame %i"), FrameCount);
	FUtilities::ExecuteCommand(Command);
	
	// Try get next camera after 1 seconds of delay
	FTimerHandle CaptureTimerHandle;
	FTimerDelegate StopRenderDocDelegate;
	StopRenderDocDelegate.BindLambda([this, FrameCount, IsBatch]() {
		if (IsBatch)
		{
			const AProfilingCamera* NextCamera = ProfilerModule->GetNextCamera();
			if (NextCamera != nullptr)
			{
				ProfilerModule->CaptureWithRenderDoc(FrameCount, IsBatch);
			}
			else
			{
				ProfilerModule->CompleteCapture();
			}
		}
		else
		{
			ProfilerModule->CompleteCapture();
		}
	});
	
	GetWorldTimerManager().SetTimer(CaptureTimerHandle, StopRenderDocDelegate, 1.0f, false);
}

FString AProfilingCamera::GetFilename() const
{
	const FString NameTokens = BatchProfilerSettings->InsightsFilenameTokens;
	FString ProcessedName = NameTokens;
	
	const FDateTime DateTime = FDateTime::Now();

	if (ProcessedName.Contains("{CameraName}"))
	{
		ProcessedName = ProcessedName.Replace(TEXT("{CameraName}"), *CameraName, ESearchCase::IgnoreCase);
	}
	
	if (ProcessedName.Contains(TEXT("{Year}")))
	{
		const FString Year = FString::Printf(TEXT("%04d"), DateTime.GetYear());
		ProcessedName = ProcessedName.Replace(TEXT("{Year}"), *Year, ESearchCase::IgnoreCase);
	}
	
	if (ProcessedName.Contains(TEXT("{Month}")))
	{
		const FString Month = FString::Printf(TEXT("%02d"), DateTime.GetMonth());
		ProcessedName = ProcessedName.Replace(TEXT("{Month}"), *Month, ESearchCase::IgnoreCase);
	}
	
	if (ProcessedName.Contains(TEXT("{Day}")))
	{
		const FString Day = FString::Printf(TEXT("%02d"), DateTime.GetDay());
		ProcessedName = ProcessedName.Replace(TEXT("{Day}"), *Day, ESearchCase::IgnoreCase);
	}
	
	if (ProcessedName.Contains(TEXT("{Hour}")))
	{
		const FString Hour = FString::Printf(TEXT("%02d"), DateTime.GetHour());
		ProcessedName = ProcessedName.Replace(TEXT("{Hour}"), *Hour, ESearchCase::IgnoreCase);
	}
	
	if (ProcessedName.Contains(TEXT("{Minute}")))
	{
		const FString Minute = FString::Printf(TEXT("%02d"), DateTime.GetMinute());
		ProcessedName = ProcessedName.Replace(TEXT("{Minute}"), *Minute, ESearchCase::IgnoreCase);
	}
	
	return *ProcessedName;
}
#pragma endregion

