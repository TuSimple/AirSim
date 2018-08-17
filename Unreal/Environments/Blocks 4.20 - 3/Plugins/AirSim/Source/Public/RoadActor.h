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

// Airsim
#include "common/AirSimSettings.hpp"
#include "common/Common.hpp"

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

	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	UMaterialInstanceDynamic* DynamicMaterialInst;


	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	UStaticMesh* Mesh_WhiteLine ; 
	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	UMaterialInstance* MaterialInstance_WhiteLine;
	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	UMaterial* Material_WhiteLine;

	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	UStaticMesh* Mesh_Road ; 
	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	UMaterialInstance* MaterialInstance_Road;
	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	UMaterial* Material_Road;

	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	bool OnConstruction_Called ; 
private:
	void OnConstruction(const FTransform& Transform) override ; 

	void AddSplineSegment(FVector const& start_position, FVector const& end_position,
	UStaticMesh* use_mesh, UMaterialInterface* use_material, FName InCollisionProfileName, FTransform const& extra_transform ) ; 

	void ScaleTensor3D(std::vector<std::vector<msr::airlib::Vector3r>> & tensor3d, float scale ); 
	void TranslateTensor3D(std::vector<std::vector<msr::airlib::Vector3r>> & tensor3d, FVector const& movement ) ; 
	void SetHeight(std::vector<std::vector<msr::airlib::Vector3r>> & tensor3d, float height ) ; 
	FVector Vector3rToFVector(msr::airlib::Vector3r const& other) ; 
	std::vector<std::vector<msr::airlib::Vector3r>> TestRoad() ; 
	
};
