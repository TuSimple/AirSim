// Fill out your copyright notice in the Description page of Project Settings.


#include "RoadActor.h"


// Sets default values
ARoadActor::ARoadActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> FoundMesh_Line_White(TEXT("/Game/TSAssets/Meshes/line_UV_1"));
	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial_Line_White_Dashed(TEXT("/Game/TSAssets/Materials/line_white_dashed_1"));
	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial_Line_White_Solid(TEXT("/Game/ModularRoads01/Materials/Master/M_Master_Material_Alpha_01"));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> FoundMesh_Road(TEXT("/Game/ModularRoads01/Meshes/Road_Parts/Roads/Style_A/Full_Size/SM_Road_A_01"));
	// static ConstructorHelpers::FObjectFinder<UStaticMesh> FoundMesh_Road(TEXT("/Game/TSAssets/Meshes/Road_Plane/W375L800/RoadOnly_Single_1"));

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> FoundMaterial_Road(TEXT("/Game/SnappyRoads/Materials/M_SR_Asphalt004_Tiled_World_Inst"));


	if (FoundMesh_Line_White.Succeeded())	{ Mesh_Line_White = FoundMesh_Line_White.Object ; 	}
	if (FoundMaterial_Line_White_Solid.Succeeded()){ Material_Line_White_Solid = FoundMaterial_Line_White_Solid.Object ;	}
	if (FoundMaterial_Line_White_Dashed.Succeeded()){ Material_Line_White_Dashed = FoundMaterial_Line_White_Dashed.Object ;	}

	if (FoundMesh_Road.Succeeded())	{ Mesh_Road = FoundMesh_Road.Object ; 	}
	if (FoundMaterial_Road.Succeeded()) { MaterialInstance_Road = FoundMaterial_Road.Object ; 	}


	
	OnConstruction_Called = false ;
}
void ARoadActor::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);

	if (not OnConstruction_Called)	{
		OnConstruction_Called = true ; 

		auto road_bounds_mid = getSettings().roads.bounds_mid;
		auto road_bounds_leftmost = getSettings().roads.bounds_leftmost;
		auto road_bounds_rightmost = getSettings().roads.bounds_rightmost;


		int NBound_mid = road_bounds_mid.size() ; 
		int NBound_leftmost = road_bounds_leftmost.size() ; 
		int NBound_rightmost = road_bounds_rightmost.size() ; 
		std::cerr << "ARoadActor::OnConstruction --> NBound=" << NBound_mid  << std::endl ;
		std::cerr << "ARoadActor::OnConstruction --> NBound_leftmost=" << NBound_leftmost  << std::endl ;
		std::cerr << "ARoadActor::OnConstruction --> NBound_rightmost=" << NBound_rightmost  << std::endl ;

		
		FVector road_min = MinTensor3D(road_bounds_mid); 
		// road_min = FVector( road_bounds[0][0][0],  road_bounds[0][0][1],  road_bounds[0][0][2] ) ; 
		
		TranslateTensor3D(road_bounds_mid, -road_min) ; 
		TranslateTensor3D(road_bounds_leftmost, -road_min) ; 
		TranslateTensor3D(road_bounds_rightmost, -road_min) ; 

		// road_bounds = TestRoad() ; 
		
		// const FVector map_origin = FVector(-10080.0, -10080.0, 0); // meter
		const FVector map_origin = FVector(-5040.0, -5040.0, 0); // meter

		TranslateTensor3D(road_bounds_mid, map_origin) ; 
		TranslateTensor3D(road_bounds_leftmost, map_origin) ; 
		TranslateTensor3D(road_bounds_rightmost, map_origin) ; 

		// FVector increase_height = FVector( 0, 0, 5) ; // 5 meter
		// TranslateTensor3D(road_bounds_mid, increase_height) ; 
		// TranslateTensor3D(road_bounds_leftmost, increase_height) ; 
		// TranslateTensor3D(road_bounds_rightmost, increase_height) ; 

		ScaleTensor3D(road_bounds_mid, 100) ; // meter to centimeter.
		ScaleTensor3D(road_bounds_leftmost, 100) ; // meter to centimeter.
		ScaleTensor3D(road_bounds_rightmost, 100) ; // meter to centimeter.
		// SetHeight(road_bounds, 120) ; // set all z-values
		// SetHeight(road_bounds, 500) ; // set all z-values


		auto const& white_line_translation = FVector(0, 0, -10);
		AddSplineSegment( road_bounds_mid, Mesh_Line_White, Material_Line_White_Dashed, TEXT("NoCollision") , white_line_translation ) ; 
		AddSplineSegment( road_bounds_leftmost, Mesh_Line_White, Material_Line_White_Solid, TEXT("NoCollision"), white_line_translation ) ; 
		AddSplineSegment( road_bounds_rightmost, Mesh_Line_White, Material_Line_White_Solid, TEXT("Vehicle"), white_line_translation ) ; 



		auto const& road_plane_translation = FVector(0, 0, 0) ; 
		AddSplineSegment( road_bounds_mid, Mesh_Road, MaterialInstance_Road, TEXT("NoCollision") , road_plane_translation ) ; 
		
		
		




		FString CurrentLevel = GetWorld()->GetMapName();
		std::cerr << " CurrentLevel = ";
		UE_LOG(LogTemp, Warning, TEXT("%s") , *CurrentLevel) ; 
		std::cerr << " road_min = " << road_min.X << " " << road_min.Y << " " << road_min.Z << std::endl; 
		std::cerr << "ARoadActor::OnConstructionEND --> NBound_mid=" << NBound_mid  << std::endl ;
		std::cerr << "ARoadActor::OnConstructionEND --> NBound_leftmost=" << NBound_leftmost  << std::endl ;

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

void ARoadActor::AddSplineSegment(std::vector<std::vector<msr::airlib::Vector3r>> const& road_bounds,
	UStaticMesh* use_mesh, UMaterialInterface* use_material, FName InCollisionProfileName, FVector const& extra_translation )
{
	const int NBound = road_bounds.size() ; 
	std::cerr << "NBound=" << NBound << std::endl ; 
	for (int b = 0 ; b < NBound ; b++ )	{
		auto road_points = road_bounds[b] ; 
		int Npoints = road_points.size() ; 

		for (int i = 1; i < Npoints; ++i)	{
			auto const& start_position = Vector3rToFVector(road_points.at(i-1) ) ; 
			auto const& end_position = Vector3rToFVector(road_points.at(i) ) ; 

			const FVector road_vector = end_position - start_position ;
			FVector tangent =  road_vector.ToOrientationRotator().Vector();

			float Unit_Len = 1500 ; // 15m
			FVector direction ; 
			float len ; 
			road_vector.ToDirectionAndLength(direction, len); 
			int Nsub = std::floor(len/Unit_Len) ; 
			auto const segment = direction * Unit_Len ; 

			auto start_position_sub = start_position ; 
			auto end_position_sub = start_position + segment ; 

			for( int i = 0 ; i <= Nsub ; i++ ){
				if (i == Nsub) {
					end_position_sub = end_position ; 
				}

				USplineMeshComponent* LaneSplineMesh = NewObject<USplineMeshComponent>(this);
				LaneSplineMesh->SetupAttachment(RootComponent);

				LaneSplineMesh->SetStartPosition( start_position_sub );
				LaneSplineMesh->SetStartTangent( tangent ) ; 
				LaneSplineMesh->SetEndPosition( end_position_sub );
				LaneSplineMesh->SetEndTangent( tangent ) ; 

				LaneSplineMesh->SetStaticMesh(use_mesh);

				DynamicMaterialInst = UMaterialInstanceDynamic::Create(use_material, LaneSplineMesh);
				LaneSplineMesh->SetMaterial(0, DynamicMaterialInst);
				LaneSplineMesh->SetCollisionProfileName(InCollisionProfileName) ; 
				LaneSplineMesh->UpdateMesh();
				LaneSplineMesh->SetMobility(EComponentMobility::Static);
				LaneSplineMesh->RegisterComponent();
				StoredSplineMeshComp.Add(LaneSplineMesh);

				start_position_sub += segment ; 
				end_position_sub += segment ; 
			}
		}
	}
}


std::vector<std::vector<msr::airlib::Vector3r>> ARoadActor::TestRoad(){
	msr::airlib::Vector3r offset = msr::airlib::Vector3r(6,6,6) ; 
	msr::airlib::Vector3r point1 = msr::airlib::Vector3r(0,0,0) + offset ; 
	msr::airlib::Vector3r point2 = msr::airlib::Vector3r(0,50,0) + offset ; 
	msr::airlib::Vector3r point3 = msr::airlib::Vector3r(30,80,0) + offset ; 

	std::vector<msr::airlib::Vector3r> lane1 = {point1, point2, point3 } ; 
	int Npoint = lane1.size(); 
	auto lane2 = lane1 ; 
	auto lane3 = lane1 ; 
	for (int i = 0; i < Npoint; ++i)	{
		lane2[i] += msr::airlib::Vector3r( 3.7, 0, 0 ) ; 
		lane3[i] += msr::airlib::Vector3r( 7.4, 0, 0 ) ;
	}
	// return {lane1, lane2, lane3} ; 
	return {lane1} ; 
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
FVector ARoadActor::MinTensor3D(std::vector<std::vector<msr::airlib::Vector3r>> const& road_bounds){
	FVector sol(1e30, 1e30, 1e30) ; 
	for ( auto const& mat: road_bounds ){
		for ( auto const& vec: mat ){
			sol[0] = std::min( vec[0], sol[0] );
			sol[1] = std::min( vec[1], sol[1] );
			sol[2] = std::min( vec[2], sol[2] );
		}
	}
	return sol ; 
}

const msr::airlib::AirSimSettings& ARoadActor::getSettings() const
{
    return AirSimSettings::singleton();
}



