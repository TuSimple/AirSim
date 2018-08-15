// Fill out your copyright notice in the Description page of Project Settings.


#include "RoadActor.h"


// Sets default values
ARoadActor::ARoadActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // static ConstructorHelpers::FObjectFinder<UStaticMesh> FoundMesh(TEXT("/Game/StarterContent/Props/SM_Couch"));
    // static ConstructorHelpers::FObjectFinder<UStaticMesh> FoundMesh(TEXT("/Game/StarterContent/Architecture/Floor_400x400"));
    // static ConstructorHelpers::FObjectFinder<UStaticMesh> FoundMesh(TEXT("/Game/Geometry/Meshes/TemplateFloor"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> FoundMesh(TEXT("/Game/Geometry/Meshes/1M_Cube"));
 
	// static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial(TEXT("/Engine/VREditor/BasicMeshes/M_Floor_01.M_Floor_01"));
	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial(TEXT("/Game/Geometry/Meshes/CubeMaterial"));
	// static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial(TEXT("/AirSim/VehicleAdv/Materials/MaterialInstances/SolidOrange"));

	if (FoundMesh.Succeeded())	{
		DefaultMesh = FoundMesh.Object ; 
	}

	if (FoundMaterial.Succeeded()){
	    StoredMaterial = FoundMaterial.Object;
	}

}
void ARoadActor::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);

	auto road_lanes = getSettings().roads.lanes;

	int Nlane = road_lanes.size() ; 
	std::cerr << "ARoadActor::OnConstruction --> Nlane=" << Nlane  << std::endl ;

	for (auto road_points: road_lanes)
	{
		int Npoints = road_points.size() ; 

		Npoints = 2 ; 
		std::cerr << " ---------------------------------------------- " << std::endl ; 
		for (int i = 0; i < Npoints ; ++i)	{
			road_points[i][2] = 500 ; 
			std::cerr << "OnConstruction road_points=" << road_points[i] << std::endl ;
		}

		// road_points[0][0] = 0.0 ;
		// road_points[0][1] = 0.0 ;
		// road_points[0][2] = 500 ;

		// road_points[1][0] = 100 ;
		// road_points[1][1] = 0.0 ;
		// road_points[1][2] = 500 ;

		// constexpr float uniform_height = 100 ; 
		// FVector position = FVector( road_points[0][0], road_points[0][1], uniform_height ) ; 
		FVector position = FVector( road_points[0][0], road_points[0][1], road_points[0][2] ) ; 
		FVector tangent =  FVector(1, 0, 0) ;

		TArray<FVector> increments ;
		increments.SetNumUninitialized(Npoints-1) ; 
		for (int i = 1; i < Npoints; ++i)	{
			// // clear out the old segments and force GC to prevent memory leaks
			// GetWorld()->ForceGarbageCollection(true);

			USplineMeshComponent* LaneSplineMesh = NewObject<USplineMeshComponent>(this);

			LaneSplineMesh->SetupAttachment(RootComponent);

			increments[i-1] =  FVector( road_points[i][0], road_points[i][1], road_points[i][2] ) - FVector( road_points[i-1][0], road_points[i-1][1], road_points[i-1][2] ) ; 
			int scale = 100 ; 
			increments[i-1] = increments[i-1] * scale ;

			LaneSplineMesh->SetStartPosition( position );
			LaneSplineMesh->SetStartTangent( tangent ) ; 
			position = position + increments[i-1] ; 
			LaneSplineMesh->SetEndPosition( position );
			LaneSplineMesh->SetEndTangent( tangent ) ; 

			LaneSplineMesh->SetStaticMesh(DefaultMesh);
			LaneSplineMesh->SetCollisionProfileName(TEXT("Vehicle")) ; 

			DynamicMaterialInst = UMaterialInstanceDynamic::Create(StoredMaterial, LaneSplineMesh);
			LaneSplineMesh->SetMaterial(0, DynamicMaterialInst);


			// UpdateRenderStateAndCollision
			// UpdateMesh
			LaneSplineMesh->UpdateMesh();
			LaneSplineMesh->SetMobility(EComponentMobility::Static);
			LaneSplineMesh->RegisterComponent();
			// LaneSplineMesh->CreationMethod = EComponentCreationMethod::SimpleConstructionScript ; 

			LaneSplineMeshes_.Emplace(LaneSplineMesh);

		}

	}

	





}

// Called when the game starts or when spawned
void ARoadActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARoadActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


const msr::airlib::AirSimSettings& ARoadActor::getSettings() const
{
    return AirSimSettings::singleton();
}

