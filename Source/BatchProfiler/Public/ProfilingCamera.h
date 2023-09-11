// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BatchProfilerSettings.h"
#include "Camera/CameraActor.h"
#include "ProfilingCamera.generated.h"
class FBatchProfilerModule;

/**
 * A profiling camera actor used for profiling operations
 */
UCLASS()
class BATCHPROFILER_API AProfilingCamera : public ACameraActor
{
	GENERATED_BODY()
	
public:
	AProfilingCamera();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Profiling")
	bool IsActiveOnProfiling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Profiling")
	FString CameraName;

	void ActivateCamera();
	void DeactivateCamera() const;

	void StartInsightCapture(const float CaptureSecs, bool IsBatch, bool IsSnapshot);
	void StartRenderDoc(const int FrameCount, bool IsBatch);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private:
	FBatchProfilerModule* ProfilerModule;
	UCameraComponent* ProfilingCameraComponent;
	const UBatchProfilerSettings* BatchProfilerSettings = GetMutableDefault<UBatchProfilerSettings>();
	
	void RegisterCamera();
	void UnregisterCamera();

	void CaptureInsight(const float CaptureSecs, bool IsBatch, bool IsSnapshot) const;
	void CaptureRenderDoc(const int FrameCount, bool IsBatch) const;

	FString GetFilename() const;
};
