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

	// ------------------------------------------------------------------------------------------------------------------------------------
	// White Lines 
	// ------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	UStaticMesh* Mesh_Line_White ; 
	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	TArray<UMaterialInterface*> Material_Line_White_Solid;
	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	TArray<UMaterialInterface*> Material_Line_White_Dashed;

	// ------------------------------------------------------------------------------------------------------------------------------------
	// Road Planes
	// ------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	UStaticMesh* Mesh_Road ; 
	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	TArray<UMaterialInterface*> Material_Road;

	// ------------------------------------------------------------------------------------------------------------------------------------
	// StreetLamp
	// ------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	UStaticMesh* Mesh_StreetLamp ; 
	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	TArray<UMaterialInterface*> Material_StreetLamp ; 

	// ------------------------------------------------------------------------------------------------------------------------------------
	// StreetLamp
	// ------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	UStaticMesh* Mesh_GuardRail ; 
	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	TArray<UMaterialInterface*> Material_GuardRail ; 
	
	// ------------------------------------------------------------------------------------------------------------------------------------
	// RoadDivider
	// ------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	UStaticMesh* Mesh_RoadDivider ; 
	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	TArray<UMaterialInterface*> Material_RoadDivider ; 

	// ------------------------------------------------------------------------------------------------------------------------------------
	// Tree
	// ------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	UStaticMesh* Mesh_Tree ; 
	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	TArray<UMaterialInterface*> Material_Tree ; 



	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	TArray<USplineMeshComponent*> StoredSplineMeshComp ; 


	UPROPERTY(EditAnywhere, Category = "LaneSplineMesh")
	bool OnConstruction_Called ; 

private:
	void OnConstruction(const FTransform& Transform) override ; 

	void AddSplineSegment(std::vector<std::vector<msr::airlib::Vector3r>> const& bounds,
	UStaticMesh* use_mesh, TArray<UMaterialInterface*> use_material, FName InCollisionProfileName, FVector const& extra_translation ) ; 

	void ScaleTensor3D(std::vector<std::vector<msr::airlib::Vector3r>> & tensor3d, float scale ); 
	void TranslateTensor3D(std::vector<std::vector<msr::airlib::Vector3r>> & tensor3d, FVector const& movement ) ; 
	FVector MinTensor3D(std::vector<std::vector<msr::airlib::Vector3r>> const& road_bounds);
	void SetHeight(std::vector<std::vector<msr::airlib::Vector3r>> & tensor3d, float height ) ; 
	FVector Vector3rToFVector(msr::airlib::Vector3r const& other) ; 
	std::vector<std::vector<msr::airlib::Vector3r>> TestRoad() ; 
	
};
