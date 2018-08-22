#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "tsmap/tsmap.h"

using namespace tsmap2;

bool IsValid(double v){
    if( std::isnan(v) || std::isinf(v) ){ return false; }
    return true; 
}
bool IsValid(std::vector<double> const& v){
    for (auto const& a: v)
        if (not IsValid(a))
            return false;
    return true ;
}
bool sortX(std::vector<double> const& a, std::vector<double> const& b ){
    return a[0] < b[0] ; 
}
bool InRange2D(Point3d const& p, std::vector<double> const& lowXY, std::vector<double> const& highXY ){
  if (p.x < lowXY[0] || p.x > highXY[0])  {
    return false; 
  }
  if (p.y < lowXY[1] || p.y > highXY[1])  {
    return false; 
  }
  return true; 
}

using json = nlohmann::json;
void BasicSetting(json& j){
  j["SeeDocsAt"] = "https://github.com/Microsoft/AirSim/blob/master/docs/settings.md" ; 

  j["SettingsVersion"] = 1.2 ; 

  json cap ; 

  cap["ImageType"] = 0 ; 
  cap["Width"] = 1024 ; 
  cap["Height"] = 576 ; 

  j["CameraDefaults"]["CaptureSettings"] =  json::array( { cap } ) ; 

  j["Vehicles"]["SUV"]["VehicleType"] = "PhysXCar" ; 
  j["Vehicles"]["SUV"]["X"] = 0 ; 
  j["Vehicles"]["SUV"]["Y"] = -6 ; 
  j["Vehicles"]["SUV"]["Z"] = 0 ; 
  j["Vehicles"]["SUV"]["Pitch"] = 0 ; 
  j["Vehicles"]["SUV"]["Roll"] = 0 ; 
  j["Vehicles"]["SUV"]["Yaw"] = 0 ; 

  j["SimMode"] = "Car" ; 
}

void RoadBoundsSetting(json& j, std::string name, std::vector<const Bound*> ts_road_bounds) {
  int count_out = 0; 
  int Nbound = ts_road_bounds.size(); 
  std::ofstream bounds_file ;
  bounds_file.open( name + ".txt");
  for (int i = 0; i < Nbound ; ++ i )  {
    auto bound_ptr = ts_road_bounds[i] ;
    std::vector<Point3d> points = bound_ptr->GetSemanticPts();
    // std::vector<Point3d> points = bounds3D[i];
    // if (not InRange2D(points[0], lowXY, highXY ) ) {
      // continue ;
    // }
    j["RoadPoints"][name][ std::to_string(i) ][ std::to_string(0) ]["X"] = points[0].x ; 
    j["RoadPoints"][name][ std::to_string(i) ][ std::to_string(0) ]["Y"] = points[0].y ; 
    j["RoadPoints"][name][ std::to_string(i) ][ std::to_string(0) ]["Z"] = points[0].z ; // airsim use NED coordinate
    j["RoadPoints"][name][ std::to_string(i) ][ std::to_string(1) ]["X"] = points.back().x ; 
    j["RoadPoints"][name][ std::to_string(i) ][ std::to_string(1) ]["Y"] = points.back().y ; 
    j["RoadPoints"][name][ std::to_string(i) ][ std::to_string(1) ]["Z"] = points.back().z ; // airsim use NED coordinate

    // std::ofstream bound_file ;
    // bound_file.open("points"+ std::to_string(i) + ".txt") ;
    // bound_file << points[0].x << " " << points[0].y << " " << points[0].z << std::endl; 
    // bound_file << points.back().x << " " << points.back().y << " " << points.back().z << std::endl; 
    // bound_file.close();

    bounds_file << points[0].x << " " << points[0].y << " " << points[0].z << " " ; 
    bounds_file << points.back().x << " " << points.back().y << " " << points.back().z << std::endl; 
    count_out++;
  }
  std::cerr << "File [" << name << "] count_out = " << count_out << std::endl ;

  bounds_file.close();
}

int main(int argc, char **argv) {
  std::shared_ptr<TSMap> tsmap = std::make_shared<TSMap>();
  std::string input_map_path = "/mnt/truenas/scratch/maps/devdata/I-10_Tucson2Phoenix_20180807.tsmap";
  std::cout << "Loading " << input_map_path << " ..." << std::endl;
  tsmap->LoadFile(input_map_path);
  std::cout << tsmap->ToString() << std::endl;

  // ------------------------------------------------------------------------------------------------------------------------
  std::vector<std::shared_ptr<const Lane>> ts_road_Lanes = tsmap->GetAllLanes() ; 
  int NLane = ts_road_Lanes.size() ;
  std::cerr << " Total NLane = " << NLane << std::endl ;  
  // ------------------------------------------------------------------------------------------------------------------------

  // ------------------------------------------------------------------------------------------------------------------------
  // bounds-leftmost 
  // ------------------------------------------------------------------------------------------------------------------------
  std::unordered_set<std::shared_ptr<const Lane>> lane_leftmost ;
  std::unordered_set<const Bound*> bound_leftmost ;

  for (auto item: ts_road_Lanes)  {
    if ( item->GetLeftLane() ) { continue ;}
    if ( lane_leftmost.count(item) > 0 )  { continue ; }
    lane_leftmost.insert(item);
    bound_leftmost.insert(item->GetLeftBound()) ;
  }
  std::cerr << " lane_leftmost.size()=" << lane_leftmost.size() << std::endl ; 
  std::cerr << " bound_leftmost.size()=" << bound_leftmost.size() << std::endl ; 

  std::vector<const Bound*> ts_road_bounds_leftmost = std::vector<const Bound*>( bound_leftmost.begin(), bound_leftmost.end() );


  // ------------------------------------------------------------------------------------------------------------------------
  // bounds-rightmost 
  // ------------------------------------------------------------------------------------------------------------------------
  std::unordered_set<std::shared_ptr<const Lane>> lane_rightmost ;
  std::unordered_set<const Bound*> bound_rightmost ;

  for (auto item: ts_road_Lanes)  {
    if ( item->GetRightLane() ) { continue ;}
    if ( lane_rightmost.count(item) > 0 )  { continue ; }
    lane_rightmost.insert(item);
    bound_rightmost.insert(item->GetRightBound()) ;
  }
  std::cerr << " lane_rightmost.size()=" << lane_rightmost.size() << std::endl ; 
  std::cerr << " bound_rightmost.size()=" << bound_rightmost.size() << std::endl ; 

  std::vector<const Bound*> ts_road_bounds_rightmost = std::vector<const Bound*>( bound_rightmost.begin(), bound_rightmost.end() );
  

  // ------------------------------------------------------------------------------------------------------------------------
  // bounds-all 
  // ------------------------------------------------------------------------------------------------------------------------
  std::vector<std::shared_ptr<const Bound>> ts_road_bounds = tsmap->GetAllBounds() ; 
  std::vector<const Bound*> ts_road_bounds_all;
  for (auto ptr: ts_road_bounds)  {
    ts_road_bounds_all.push_back(ptr.get()) ;
  }



  // ------------------------------------------------------------------------------------------------------------------------
  // Writing Json
  // ------------------------------------------------------------------------------------------------------------------------

  json j ; 

  BasicSetting(j);
  RoadBoundsSetting(j, "LeftBounds", ts_road_bounds_leftmost) ; 
  RoadBoundsSetting(j, "RightBounds", ts_road_bounds_rightmost) ; 
  RoadBoundsSetting(j, "Bounds", ts_road_bounds_all) ; 

  std::ofstream myfile;
  myfile.open("settings.json") ; 
  myfile << j << std::endl; 
  myfile.close();

  return 0 ;   
}

