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

#include "OrbitPawn.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EngineUtils.h"

AOrbitPawn::AOrbitPawn()
{
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
    CameraBoom->SetupAttachment(GetRootComponent());

    Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
    Camera->SetupAttachment(CameraBoom);

    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PostPhysics;
}

void AOrbitPawn::BeginPlay()
{
    Super::BeginPlay();

    CameraMoveLocked = true;
}


void AOrbitPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    ZoomLerp -= CameraZoomAxisInput * ZoomSpeed * DeltaTime;
    ZoomLerp = FMath::Clamp(ZoomLerp, 0.f, 1.f);

    float ZoomDistance = FMath::Lerp(CameraZoomMinDistance, CameraZoomMaxDistance, ZoomLerp*ZoomLerp);
    float ZoomFOV = FMath::Lerp(CameraZoomMinFOV, CameraZoomMaxFOV, ZoomLerp);

    CameraBoom->TargetArmLength = ZoomDistance;
    Camera->FieldOfView = ZoomFOV;

    if (!CameraMoveLocked)
    {
        CameraYaw += CameraYawAxisInput * DeltaTime * CameraYawSpeed;
        CameraPitch += CameraPitchAxisInput * DeltaTime * CameraPitchSpeed;

        FRotator CameraRotation(CameraPitch, CameraYaw, 0.f);

        CameraBoom->SetWorldRotation(CameraRotation.Quaternion());
    }
}


void AOrbitPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("CameraMoveLock", EInputEvent::IE_Pressed, this , &AOrbitPawn::OnCameraMoveLockPressed);
    PlayerInputComponent->BindAction("CameraMoveLock", EInputEvent::IE_Released, this , &AOrbitPawn::OnCameraMoveLockReleased);

    PlayerInputComponent->BindAxis("CameraZoom", this, &AOrbitPawn::OnCameraZoomAxis);
    PlayerInputComponent->BindAxis("CameraYaw", this, &AOrbitPawn::OnCameraYawAxis);
    PlayerInputComponent->BindAxis("CameraPitch", this, &AOrbitPawn::OnCameraPitchAxis);
}


void AOrbitPawn::OnCameraZoomAxis(float input)
{
    CameraZoomAxisInput = input;
}

void AOrbitPawn::OnCameraYawAxis(float input)
{
    CameraYawAxisInput = input;
}

void AOrbitPawn::OnCameraPitchAxis(float input)
{
    CameraPitchAxisInput = input;
}

void AOrbitPawn::OnCameraMoveLockPressed()
{
    CameraMoveLocked = false;
}

void AOrbitPawn::OnCameraMoveLockReleased()
{
    CameraMoveLocked = true;
}
