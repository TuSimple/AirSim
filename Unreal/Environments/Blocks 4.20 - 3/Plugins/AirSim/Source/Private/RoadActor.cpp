// Fill out your copyright notice in the Description page of Project Settings.


#include "RoadActor.h"


// Sets default values
ARoadActor::ARoadActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // static ConstructorHelpers::FObjectFinder<UStaticMesh> FoundMesh_WhiteLine(TEXT("/Game/ModularRoads01/Meshes/Road_Parts/Road_Markings/SM_Line_White_01")); // 90 degrees
	// static ConstructorHelpers::FObjectFinder<UMaterialInstance> FoundMaterial_WhiteLine(TEXT("/Game/ModularRoads01/Materials/MI_Road_Markings_01"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> FoundMesh_WhiteLine(TEXT("/Game/Geometry/Meshes/1M_Cube")); // 90 degrees
	// static ConstructorHelpers::FObjectFinder<UMaterialInstance> FoundMaterial_WhiteLine(TEXT("/AirSim/VehicleAdv/SUV/AutomotiveMaterials/Materials/Glass/Headlights"));
	// static ConstructorHelpers::FObjectFinder<UMaterialInstance> FoundMaterial_WhiteLine(TEXT("/Game/ModularRoads01/Materials/MI_Asphalt_02"));
	// static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial_WhiteLine(TEXT("/AirSim/Modelds/MiniQuadCopter/Prop_White_Plastic"));
	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial_WhiteLine(TEXT("/Game/ModularRoads01/Materials/Master/M_Master_Material_Alpha_01"));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> FoundMesh_Road(TEXT("/Game/ModularRoads01/Meshes/Road_Parts/Roads/Style_A/Full_Size/SM_Road_A_01"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> FoundMaterial_Road(TEXT("/Game/ModularRoads01/Materials/MI_Asphalt_01"));
	// static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial_Road(TEXT("/Game/ModularRoads01/Materials/MI_Asphalt_02"));

	// static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial(TEXT("/Game/AsphaltConcreteRoads/Materials/Mat1_master_tessel"));
	// '/Game/ModularRoads01/Materials/MI_Asphalt_01'
	// '/Game/ModularRoads01/Materials/MI_Asphalt_02'
	// '/Game/ModularRoads01/Materials/MI_Asphalt_03'
	// '/Game/ModularRoads01/Materials/MI_Asphalt_04'
	// '/Game/ModularRoads01/Materials/MI_Asphalt_05'
	// '/Game/ModularRoads01/Materials/MI_Asphalt_06'

	if (FoundMesh_WhiteLine.Succeeded())	{
		Mesh_WhiteLine = FoundMesh_WhiteLine.Object ; 
	}
	if (FoundMaterial_WhiteLine.Succeeded()){
	    Material_WhiteLine = FoundMaterial_WhiteLine.Object ;
	}

	if (FoundMesh_Road.Succeeded())	{
		Mesh_Road = FoundMesh_Road.Object ; 
	}
	if (FoundMaterial_Road.Succeeded()) {
		MaterialInstance_Road = FoundMaterial_Road.Object ; 
	}
	OnConstruction_Called = false ;
}
void ARoadActor::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);

	if (not OnConstruction_Called)	{
		OnConstruction_Called = true ; 

		auto road_bounds = getSettings().roads.bounds;

		// road_bounds = TestRoad() ; 

		int NBound = road_bounds.size() ; 
		std::cerr << "ARoadActor::OnConstruction --> NBound=" << NBound  << std::endl ;

		FVector origin = FVector( road_bounds[0][0][0], road_bounds[0][0][1], road_bounds[0][0][2] ) ; 

		TranslateTensor3D(road_bounds, -origin) ; 
		ScaleTensor3D(road_bounds, 100) ; // meter to centimeter.
		SetHeight(road_bounds, 120) ; // set all z-values

		for (int b = 0 ; b < NBound ; b++ )	{
			auto road_points = road_bounds[b] ; 
			int Npoints = road_points.size() ; 
			for (int i = 1; i < Npoints; ++i)	{

				auto const& start_position = Vector3rToFVector(road_points.at(i-1) ) ; 
				auto const& end_position = Vector3rToFVector(road_points.at(i) ) ; 

				// -------------------------------------------------------
				// Road Single lines
				// -------------------------------------------------------
				auto const& road_vector = end_position - start_position ;
				const float angle = std::atan2(road_vector.X, road_vector.Y) * 180 / M_PI; 
				const auto len = road_vector.Size() / 100 ; 
				auto const& white_line_3D_scale = FVector( 1 , 1 , 1 ) ; 
				auto const& white_line_translation = FVector(0, 0, -61) + GetTransform().GetLocation() ;
				auto const& white_line_quat = FQuat( FRotator(0, 0, 0 ) ) ; 
				auto const& white_line_transform = FTransform(white_line_quat, white_line_translation, white_line_3D_scale) ; 
				AddSplineSegment( start_position, end_position, Mesh_WhiteLine, Material_WhiteLine, TEXT("NoCollision") , white_line_transform ) ; 

				// -------------------------------------------------------
				// Road Plates
				// -------------------------------------------------------
				auto const& road_3D_scale = FVector( 1 , 1 , 1 ) ; 
				auto const& road_translation = FVector(0, 0, 0) + GetTransform().GetLocation() ;
				auto const& road_quat = FQuat( FRotator(0, 0, 0 ) ) ; 
				auto const& road_transform = FTransform(road_quat, road_translation, road_3D_scale) ; 
				AddSplineSegment( start_position, end_position, Mesh_Road, MaterialInstance_Road, TEXT("Vehicle"), road_transform ) ; 
			}
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

void ARoadActor::AddSplineSegment(FVector const& start_position, FVector const& end_position,
	UStaticMesh* use_mesh, UMaterialInterface* use_material, FName InCollisionProfileName, FTransform const& extra_transform )
{
	FVector road_vector = end_position - start_position ;
	FVector tangent =  road_vector.ToOrientationRotator().Vector();

	USplineMeshComponent* LaneSplineMesh = NewObject<USplineMeshComponent>(this);

	LaneSplineMesh->SetupAttachment(RootComponent);

	LaneSplineMesh->SetStartPosition( start_position );
	LaneSplineMesh->SetStartTangent( tangent ) ; 
	LaneSplineMesh->SetEndPosition( end_position );
	LaneSplineMesh->SetEndTangent( tangent ) ; 

	LaneSplineMesh->SetStaticMesh(use_mesh);

	DynamicMaterialInst = UMaterialInstanceDynamic::Create(use_material, LaneSplineMesh);
	LaneSplineMesh->SetMaterial(0, DynamicMaterialInst);

	FQuat quat = FQuat(end_position - start_position, 0) ; 
	// LaneSplineMesh->SetWorldTransform( /*FTransform(quat, start_position) +*/ extra_transform  ) ; 
	LaneSplineMesh->SetRelativeLocation( extra_transform.GetLocation() ) ; 
	LaneSplineMesh->SetRelativeScale3D( extra_transform.GetScale3D() ) ; 
	// LaneSplineMesh->SetWorldScale3D( extra_transform.GetScale3D() ) ; 

	LaneSplineMesh->SetCollisionProfileName(InCollisionProfileName) ; 

	LaneSplineMesh->UpdateMesh();
	LaneSplineMesh->SetMobility(EComponentMobility::Static);
	LaneSplineMesh->RegisterComponent();
	
}


std::vector<std::vector<msr::airlib::Vector3r>> ARoadActor::TestRoad(){
	msr::airlib::Vector3r offset = msr::airlib::Vector3r(6,6,6) ; 
	msr::airlib::Vector3r point1 = msr::airlib::Vector3r(0,0,0) + offset ; 
	msr::airlib::Vector3r point2 = msr::airlib::Vector3r(0,5,0) + offset ; 
	msr::airlib::Vector3r point3 = msr::airlib::Vector3r(3,8,0) + offset ; 

	std::vector<msr::airlib::Vector3r> lane1 = {point1, point2, point3 } ; 
	int Npoint = lane1.size(); 
	auto lane2 = lane1 ; 
	auto lane3 = lane1 ; 
	for (int i = 0; i < Npoint; ++i)	{
		lane2[i] += msr::airlib::Vector3r( 3.7, 0, 0 ) ; 
		lane3[i] += msr::airlib::Vector3r( 7.4, 0, 0 ) ;
	}
	return {lane1, lane2, lane3} ; 
	// return {lane1} ; 
}

void ARoadActor::ScaleTensor3D(std::vector<std::vector<msr::airlib::Vector3r>> & tensor3d, float scale ){
	for ( auto& mat: tensor3d ){
		for ( auto& vec: mat ){
			vec[0] *= scale ; 
			vec[1] *= scale ; 
			vec[2] *= scale ; 
		}
	}
}

void ARoadActor::TranslateTensor3D(std::vector<std::vector<msr::airlib::Vector3r>> & tensor3d, FVector const& movement ){
	for ( auto& mat: tensor3d ){
		for ( auto& vec: mat ){
			vec[0] += movement.X ; 
			vec[1] += movement.Y ; 
			vec[2] += movement.Z ; 
		}
	}
}
void ARoadActor::SetHeight(std::vector<std::vector<msr::airlib::Vector3r>> & tensor3d, float height ){
	for ( auto& mat: tensor3d ){
		for ( auto& vec: mat ){
			vec[2] = height ; 
		}
	}
}
FVector ARoadActor::Vector3rToFVector(msr::airlib::Vector3r const& other){
	FVector sol ; 
	sol.X = other[0] ; 
	sol.Y = other[1] ; 
	sol.Z = other[2] ; 
	return sol ; 
}

const msr::airlib::AirSimSettings& ARoadActor::getSettings() const
{
    return AirSimSettings::singleton();
}



