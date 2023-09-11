#pragma once

#include "CoreMinimal.h"
#include "ProfilingCamera.h"
#include "Modules/ModuleManager.h"

class BATCHPROFILER_API FBatchProfilerModule : public IModuleInterface
{
public:
	/** IModuleInterface Implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Camera Registration */
	void RegisterProfilingCamera(AProfilingCamera* ProfilingCamera);
	void UnregisterProfilingCamera(AProfilingCamera* ProfilingCamera);

	/** Camera Switch Logic */
	void NextCamera();
	void PreviousCamera();
	AProfilingCamera* GetNextCamera();

	/** Capture Functions */
	void CaptureWithInsight(const float CaptureSecs, const bool IsBatch, const bool IsSnapshot) const;
	void CaptureWithRenderDoc(const int FrameCount, const bool IsBatch) const;
	void CompleteCapture() const;

protected:
	/** Command Bindings */
	void NextCameraCommand(const TArray<FString>& Args);
	void PrevCameraCommand(const TArray<FString>& Args);
	void StartInsightCommand(const TArray<FString>& Args, bool IsBatch,  const bool IsSnapshot);
	void StartRenderDocCommand(const TArray<FString>& Args, bool IsBatch);
	
private:
	int CurrentCameraIndex = 0;
	bool IsInsightActive = false;
	AProfilingCamera* ActiveCamera = nullptr;
	TArray<AProfilingCamera*> ProfilingCameras;
	const UBatchProfilerSettings* BatchProfilerSettings = GetMutableDefault<UBatchProfilerSettings>();
	bool TryInitCapture(bool IsBatch);
	// void RegisterKeyBindings();
};
