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

#include "EarthActor.h"
#include "Spice.h"
#include "Math/Vector.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AEarthActor::AEarthActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PrePhysics;

    TimeMultiplier = 10.;
    AnimationDuration = FSEphemerisPeriod::FromSeconds(1.5 * 3600.);

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
    SetRootComponent(StaticMesh);
}

// Called when the game starts or when spawned
void AEarthActor::BeginPlay()
{
	Super::BeginPlay();

    // Copy the material...
    MaterialInstance = StaticMesh->CreateDynamicMaterialInstance(0, EarthMaterial);
	
    // Required for getgeophs, getelm
    ES_ResultCode ResultCode;
    FString ErrorMessage;
    USpice::furnsh(ResultCode, ErrorMessage, TEXT("Content/Spice/Kernels/meta.tm"));

    if (ResultCode != ES_ResultCode::Success)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load kernel: %s"), *ErrorMessage);
    }

    USpice::et_now(et);
}

// Called every frame
void AEarthActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    et += FSEphemerisPeriod(DeltaTime * TimeMultiplier);

    FSEphemerisTime now;
    USpice::et_now(now);

    if (et > now + AnimationDuration)
    {
        et = now;
    }

    // Rotate the earth to the corrent position.
    ES_ResultCode ResultCode;
    FString ErrorMessage;
    FSRotationMatrix toIAU;

    USpice::pxform(ResultCode, ErrorMessage, toIAU, et, TEXT("IAU_EARTH"), TEXT("J2000"));
    if (ResultCode == ES_ResultCode::Success)
    {
        FSQuaternion EarthRotation;
        // We can be pretty darn sure this won't fail, as the only reason for failure is
        // that the matrix is not a rotation matrix.  But, we just got it from pxform, as a
        // rotation matrix.  So, ...
        USpice::m2q(ResultCode, ErrorMessage, toIAU, EarthRotation);

        SetActorRotation(USpiceTypes::Swazzle(EarthRotation));
    }

    // Get the sun's direction...
    FSDistanceVector psun;
    FSEphemerisPeriod lt;
    USpice::spkpos(ResultCode, ErrorMessage, et, psun, lt, TEXT("SUN"), TEXT("EARTH"), TEXT("J2000"));
    if (ResultCode == ES_ResultCode::Success)
    {
        FVector SunDirection = USpiceTypes::Swizzle(psun);
        SunDirection.Normalize();
        MaterialInstance->SetVectorParameterValue("LightDirection", SunDirection);
    }
}

