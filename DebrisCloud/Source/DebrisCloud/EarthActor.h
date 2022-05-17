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
#include "GameFramework/Actor.h"
#include "SpiceTypes.h"
#include "EarthActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class DEBRISCLOUD_API AEarthActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEarthActor();

	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly)
	FSEphemerisTime et;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    double TimeMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FSEphemerisPeriod AnimationDuration;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> StaticMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UMaterialInstance> EarthMaterial;

    UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MaterialInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
