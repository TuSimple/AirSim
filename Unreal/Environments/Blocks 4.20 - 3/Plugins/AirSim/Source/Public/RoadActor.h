// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "CircularQueue.h"
#include "Array.h"

// #include "tsmap/tsmap.h"
#include "common/AirSimSettings.hpp"

#include "RoadActor.generated.h"

#define NSpMesh 127 

UCLASS()
class AIRSIM_API ARoadActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoadActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	const msr::airlib::AirSimSettings& getSettings() const ; 

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<USplineMeshComponent*> LaneSplineMeshes_ ;
	
	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	UMaterial* StoredMaterial;
	
	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	UMaterialInstanceDynamic* DynamicMaterialInst;

private:
	void OnConstruction(const FTransform& Transform) override ; 

	UStaticMesh* DefaultMesh ; 
};
