/* Copyright (C) 2022 Chuck Noble <chuck@gamergenic.com>
 * This work is free.  You can redistribute it and /or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar.  See http://www.wtfpl.net/ for more details.
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "OrbitPawn.generated.h"

// --------------------------------------

class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS(Blueprintable)
class DEBRISCLOUD_API AOrbitPawn 
    : public APawn
{
    GENERATED_BODY()

public:
    AOrbitPawn();

    void BeginPlay() override;
    void Tick(float DeltaTime) override;
    void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UCameraComponent> Camera;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float CameraZoomMinDistance = 18000.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float CameraZoomMaxDistance = 50000.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float CameraZoomMinFOV = 20.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float CameraZoomMaxFOV = 60.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float ZoomLerp = 0.3f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float ZoomSpeed = 0.5f;

    UPROPERTY(Transient, VisibleInstanceOnly)
    float CameraZoomAxisInput = 0.f;

    UPROPERTY(Transient, VisibleInstanceOnly)
    float CameraYawAxisInput = 0.f;

    UPROPERTY(Transient, VisibleInstanceOnly)
    float CameraPitchAxisInput = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    float CameraYaw = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    float CameraPitch = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float CameraYawSpeed = 180.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float CameraPitchSpeed = 180.f;

    bool CameraMoveLocked;

protected:

    void OnCameraZoomAxis(float input);
    void OnCameraYawAxis(float input);
    void OnCameraPitchAxis(float input);
    void OnCameraMoveLockPressed();
    void OnCameraMoveLockReleased();
};

