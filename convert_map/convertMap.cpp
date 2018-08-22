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
int main(int argc, char **argv) {
  std::shared_ptr<TSMap> tsmap = std::make_shared<TSMap>();
  std::string input_map_path = "/mnt/truenas/scratch/maps/devdata/I-10_Tucson2Phoenix_20180807.tsmap";
  std::cout << "Loading " << input_map_path << " ..." << std::endl;
  tsmap->LoadFile(input_map_path);
  std::cout << tsmap->ToString() << std::endl;

  std::vector<std::shared_ptr<const Bound>> ts_road_bounds = tsmap->GetAllBounds() ; 

  int Nbound = ts_road_bounds.size() ;

  std::vector<double> lowXY = {50000, -70000};
  std::vector<double> highXY = {60000, -60000};

  json j ; 
  // Nbound = 1000 ; 
  int count_out = 0 ; 
  std::cerr << " Nbound = " << Nbound << std::endl ;  

  // std::vector<std::vector<Point3d>> bounds3D(Nbound, std::vector<Point3d>(2, Point3d(0,0,0)) ) ;
  // std::vector<double> smallest(3, 1e30) ; 
  // for (int i = 0; i < Nbound ; ++ i ) {
  //   auto bound_ptr = ts_road_bounds[i] ;
  //   std::vector<Point3d> points = bound_ptr->GetSemanticPts();
  //   bounds3D[i][0].x = points[0].x ;
  //   bounds3D[i][0].y = points[0].y ;
  //   bounds3D[i][0].z = points[0].z ;
  //   bounds3D[i][1].x = points.back().x ;
  //   bounds3D[i][1].y = points.back().y ;
  //   bounds3D[i][1].z = points.back().z ;
  //   smallest[0] = std::min(smallest[0], points[0].x) ; 
  //   smallest[1] = std::min(smallest[1], points[0].y) ; 
  //   smallest[2] = std::min(smallest[2], points[0].z) ; 
  // }

  // for (auto& mat: bounds3D)  {
  //   for (auto& vec: mat)  {
  //     vec.x -= smallest[0] ;
  //     vec.y -= smallest[1] ;
  //     vec.z -= smallest[2] ;
  //   }
  // }

  std::ofstream bounds_file ;
  bounds_file.open("bounds.txt");
  for (int i = 0; i < Nbound ; ++ i )  {
    auto bound_ptr = ts_road_bounds[i] ;
    std::vector<Point3d> points = bound_ptr->GetSemanticPts();
    // std::vector<Point3d> points = bounds3D[i];
    if (not InRange2D(points[0], lowXY, highXY ) ) {
      continue ;
    }
    j["RoadPoints"]["Bounds"][ std::to_string(i) ][ std::to_string(0) ]["X"] = points[0].x ; 
    j["RoadPoints"]["Bounds"][ std::to_string(i) ][ std::to_string(0) ]["Y"] = points[0].y ; 
    j["RoadPoints"]["Bounds"][ std::to_string(i) ][ std::to_string(0) ]["Z"] = points[0].z ; // airsim use NED coordinate
    j["RoadPoints"]["Bounds"][ std::to_string(i) ][ std::to_string(1) ]["X"] = points.back().x ; 
    j["RoadPoints"]["Bounds"][ std::to_string(i) ][ std::to_string(1) ]["Y"] = points.back().y ; 
    j["RoadPoints"]["Bounds"][ std::to_string(i) ][ std::to_string(1) ]["Z"] = points.back().z ; // airsim use NED coordinate

    // std::ofstream bound_file ;
    // bound_file.open("points"+ std::to_string(i) + ".txt") ;
    // bound_file << points[0].x << " " << points[0].y << " " << points[0].z << std::endl; 
    // bound_file << points.back().x << " " << points.back().y << " " << points.back().z << std::endl; 
    // bound_file.close();

    bounds_file << points[0].x << " " << points[0].y << " " << points[0].z << " " ; 
    bounds_file << points.back().x << " " << points.back().y << " " << points.back().z << std::endl; 
    count_out++;
  }
  std::cerr << " count_out = " << count_out << std::endl ;



  bounds_file.close();

  BasicSetting(j);

  std::ofstream myfile;
  myfile.open("settings.json") ; 
  myfile << j << std::endl; 
  myfile.close();

  return 0 ;   
}

