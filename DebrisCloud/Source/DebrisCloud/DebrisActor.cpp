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

#include "DebrisActor.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "EngineUtils.h"
#include "EarthActor.h"
#include "Spice.h"

// Sets default values
ADebrisActor::ADebrisActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    InstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>("InstancedMesh");
    SetRootComponent(InstancedMesh);

    GetRootComponent()->SetAbsolute(false, true, true);
}

// Called when the game starts or when spawned
void ADebrisActor::BeginPlay()
{
	Super::BeginPlay();

    UMaterialInstanceDynamic* MaterialInstance = InstancedMesh->CreateDynamicMaterialInstance(0, DebrisMaterial);
    MaterialInstance->SetVectorParameterValue("DebrisColor", DebrisColor);
   
    LoginWithSpaceTrack(ObjectId, SpaceTrackUser, SpaceTrackPassword);
    
    TActorIterator<AActor> Iterator(GetWorld(), AEarthActor::StaticClass());
    for ( ; Iterator; ++Iterator)
    {
        AEarthActor* Earth = Cast<AEarthActor>(*Iterator);

        if (Earth != nullptr)
        {
            FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;

            SetActorTransform(Earth->GetActorTransform());
            AttachToActor(Earth, FAttachmentTransformRules::KeepRelativeTransform);
            GetRootComponent()->SetRelativeLocation(FVector(0,0,0));
            GetRootComponent()->SetRelativeRotation(FQuat::Identity);
            GetRootComponent()->SetWorldScale3D(FVector(1,1,1));

            EarthTheActor = Earth;
            break;
        }
    }
}



// Be sure to read the space-track.org CAREFULLY before accessing the server.
// https://space-track.org/documentation#user_agree
// Do NOT Spam the server with repeated requests!!
// If you're building a client that makes use of data in some way, do NOT allow the
// client to directly connect.  It's against the user agreement.   You would instead
// need to read and cache data from your own server, and connect your client to it.
void ADebrisActor::LoginWithSpaceTrack(const FString& NewObjectId, const FString& NewUser, const FString& NewPassword)
{
    FString uriBase = LIVE_URL_BASE;
    FString uriLogin = uriBase + TEXT("/ajaxauth/login");

    FString uriQuery = uriBase + TEXT("/basicspacedata/query/class/gp/OBJECT_ID/~~") + NewObjectId + TEXT("/orderby/DECAY_DATE%20desc/emptyresult/show");

    FHttpModule& httpModule = FHttpModule::Get();

    // Create an http request
    // The request will execute asynchronously, and call us back on the Lambda below
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest = httpModule.CreateRequest();

    FString RequestContent;

    if (!UseTestUrl)
    {
        pRequest->SetVerb(TEXT("POST"));
        pRequest->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));

        RequestContent = TEXT("identity=") + NewUser + TEXT("&password=") + NewPassword + TEXT("&query=") + uriQuery;
        pRequest->SetContentAsString(RequestContent);
    }
    else
    {
        pRequest->SetVerb(TEXT("GET"));
        RequestContent = TEXT("TEST_CONTENT");
        uriLogin = TestUrl;
    }

    pRequest->SetURL(uriLogin);

    pRequest->OnProcessRequestComplete().BindLambda(
        [&](
            FHttpRequestPtr pRequest,
            FHttpResponsePtr pResponse,
            bool connectedSuccessfully) mutable {

        if (connectedSuccessfully) {
            UE_LOG(LogTemp, Log, TEXT("Space-Track response: %s"),*(pResponse->GetContentAsString().Left(64)));

            ProcessSpaceTrackResponse(pResponse->GetContentAsString());
        }
        else {
            switch (pRequest->GetStatus()) {
            case EHttpRequestStatus::Failed_ConnectionError:
                UE_LOG(LogTemp, Error, TEXT("Connection failed."));
            default:
                UE_LOG(LogTemp, Error, TEXT("Request failed."));
            }
        }
    });

    UE_LOG(LogTemp, Log, TEXT("Space-Track request: %s; content:%s"), *uriLogin, *RequestContent);

    pRequest->ProcessRequest();
}

void ADebrisActor::ProcessSpaceTrackResponse(const FString& ResponseContent)
{
    // Validate http called us back on the Game Thread...
    check(IsInGameThread());

    TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(ResponseContent);
    TArray<TSharedPtr<FJsonValue>> OutArray;

    if (FJsonSerializer::Deserialize(JsonReader, OutArray))
    {
        ProcessSpaceTrackResponse(OutArray);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to deserialize Space-Track response"));
    }
}

void ADebrisActor::ProcessSpaceTrackResponse(const TArray<TSharedPtr<FJsonValue>>& JsonResponseArray)
{
    FSEphemerisTime now;

    auto Earth = EarthTheActor.Get();
    if (Earth != nullptr)
    {
        now = Earth->et;
    }
    
    SpaceTrackBeginResponse();
    for (int i = 0; i < JsonResponseArray.Num(); ++i)
    {
        ProcessSpaceTrackResponseObject(JsonResponseArray[i]->AsObject());
    }
    SpaceTrackEndResponse(now);
}

void ADebrisActor::ProcessSpaceTrackResponseObject(const TSharedPtr<FJsonObject>& JsonResponseObject)
{
    if (JsonResponseObject)
    {
        FString DECAY_DATE;

        // Only add objects that haven't decayed yet...
        if (!JsonResponseObject->TryGetStringField(TEXT("DECAY_DATE"), DECAY_DATE))
        {
            FString TLE_LINE1 = JsonResponseObject->GetStringField(TEXT("TLE_LINE1"));
            FString TLE_LINE2 = JsonResponseObject->GetStringField(TEXT("TLE_LINE2"));
            ES_ResultCode ResultCode;
            FString ErrorMessage;
            FSTwoLineElements Elements;
            FSEphemerisTime et;

            USpice::getelm(ResultCode, ErrorMessage, et, Elements, TLE_LINE1, TLE_LINE2);

            if (ResultCode == ES_ResultCode::Success)
            {
                DebrisElements.Add(Elements);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to parse TLE Elements: %s"), *ErrorMessage);
            }
        }

        // The rest is just for fun, we don't retain anything... But you can step through
        // it in the debugger to see the values, etc...
        FString EPOCH = JsonResponseObject->GetStringField(TEXT("EPOCH"));

        double MEAN_MOTION = 0.;
        JsonResponseObject->TryGetNumberField(TEXT("MEAN_MOTION"), MEAN_MOTION);

        double ECCENTRICITY = 0.;
        JsonResponseObject->TryGetNumberField(TEXT("ECCENTRICITY"), ECCENTRICITY);

        double INCLINATION = 0.;
        JsonResponseObject->TryGetNumberField(TEXT("INCLINATION"), INCLINATION);

        double RA_OF_ASC_NODE = 0.;
        JsonResponseObject->TryGetNumberField(TEXT("RA_OF_ASC_NODE"), RA_OF_ASC_NODE);

        double ARG_OF_PERICENTER = 0.;
        JsonResponseObject->TryGetNumberField(TEXT("ARG_OF_PERICENTER"), ARG_OF_PERICENTER);

        double MEAN_ANOMALY = 0.;
        JsonResponseObject->TryGetNumberField(TEXT("MEAN_ANOMALY"), MEAN_ANOMALY);

        double BSTAR = 0.;
        JsonResponseObject->TryGetNumberField(TEXT("BSTAR"), BSTAR);

        double MEAN_MOTION_DOT = 0.;
        JsonResponseObject->TryGetNumberField(TEXT("MEAN_MOTION_DOT"), MEAN_MOTION_DOT);

        double MEAN_MOTION_DDOT = 0.;
        JsonResponseObject->TryGetNumberField(TEXT("MEAN_MOTION_DDOT"), MEAN_MOTION_DDOT);

        double SEMIMAJOR_AXIS = 0.;
        JsonResponseObject->TryGetNumberField(TEXT("SEMIMAJOR_AXIS"), SEMIMAJOR_AXIS);

        double PERIOD = 0.;
        JsonResponseObject->TryGetNumberField(TEXT("PERIOD"), PERIOD);

        double APOAPSIS = 0.;
        JsonResponseObject->TryGetNumberField(TEXT("APOAPSIS"), APOAPSIS);

        double PERIAPSIS = 0.;
        JsonResponseObject->TryGetNumberField(TEXT("PERIAPSIS"), PERIAPSIS);
    }
}


FVector ADebrisActor::LocationFromTLE(const FSEphemerisTime& et, const FSTLEGeophysicalConstants& GeophysicalConstants, const FSTwoLineElements& Elements)
{
    ES_ResultCode ResultCode;
    FString ErrorMessage;
    FVector scenegraphPosition = FVector(0.);

    FSStateVector state;
    USpice::evsgp4(ResultCode, ErrorMessage, state, et, GeophysicalConstants, Elements);

    if (ResultCode == ES_ResultCode::Success)
    {
        if (FGenericPlatformMath::IsNaN(state.r.x.AsKilometers())
            || FGenericPlatformMath::IsNaN(state.r.y.AsKilometers())
            || FGenericPlatformMath::IsNaN(state.r.z.AsKilometers())
            || FGenericPlatformMath::IsNaN(state.v.dx.AsKilometersPerSecond())
            || FGenericPlatformMath::IsNaN(state.v.dy.AsKilometersPerSecond())
            || FGenericPlatformMath::IsNaN(state.v.dz.AsKilometersPerSecond()))
        {
            UE_LOG(LogTemp, Error, TEXT("evsgp4 returned a NaN"));
        }
        else
        {
            // The state is in Ref=J2000, and so is the scene, so no need to xform
            scenegraphPosition = USpiceTypes::Swizzle(state.r);
        }
    }

    return scenegraphPosition;
}

void ADebrisActor::SpaceTrackBeginResponse()
{
    DebrisElements.Empty();
    InstancedMesh->ClearInstances();
}

void ADebrisActor::SpaceTrackEndResponse(const FSEphemerisTime& et)
{
    FSTLEGeophysicalConstants earth;
    USpice::getgeophs(earth, TEXT("EARTH"));

    for (int i = 0; i < DebrisElements.Num(); ++i)
    {
        FTransform t;
        t.SetLocation(LocationFromTLE(et, earth, DebrisElements[i]));
        t.SetScale3D(FVector(ObjectScale));
        InstancedMesh->AddInstance(t, false);
    }
}


// Called every frame
void ADebrisActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    FSEphemerisTime now;

    auto Earth = EarthTheActor.Get();
    if (Earth != nullptr)
    {
        now = Earth->et;
    }

    FSTLEGeophysicalConstants earth;
    USpice::getgeophs(earth, TEXT("EARTH"));

    for (int i = 0; i < DebrisElements.Num(); ++i)
    {
        FTransform t;
        t.SetLocation(LocationFromTLE(now, earth, DebrisElements[i]));
        t.SetScale3D(FVector(ObjectScale));
        // In an actual app, we'd update the instances as a batch -- of course...
        InstancedMesh->UpdateInstanceTransform(i, t, false, i == DebrisElements.Num() -1);
    }
}

