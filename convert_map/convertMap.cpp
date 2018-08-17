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


  json j ; 
  Nbound = 5000 ; 
  for (int i = 0; i < Nbound ; ++ i )  {
    auto bound_ptr = ts_road_bounds[i] ;
    std::vector<Point3d> points = bound_ptr->GetSemanticPts();
    j["RoadPoints"]["Bounds"][ std::to_string(i) ][ std::to_string(0) ]["X"] = points[0].x ; 
    j["RoadPoints"]["Bounds"][ std::to_string(i) ][ std::to_string(0) ]["Y"] = points[0].y ; 
    j["RoadPoints"]["Bounds"][ std::to_string(i) ][ std::to_string(0) ]["Z"] = points[0].z ; 
    j["RoadPoints"]["Bounds"][ std::to_string(i) ][ std::to_string(1) ]["X"] = points.back().x ; 
    j["RoadPoints"]["Bounds"][ std::to_string(i) ][ std::to_string(1) ]["Y"] = points.back().y ; 
    j["RoadPoints"]["Bounds"][ std::to_string(i) ][ std::to_string(1) ]["Z"] = points.back().z ; 
    // std::ofstream bound_file ;
    // bound_file.open("points"+ std::to_string(i) + ".txt") ;
    // bound_file << points[0].x << " " << points[0].y << std::endl; 
    // bound_file << points.back().x << " " << points.back().y << std::endl; 
    // bound_file.close();
  }

  BasicSetting(j);

  std::ofstream myfile;
  myfile.open("settings.json") ; 
  myfile << j << std::endl; 
  myfile.close();

  return 0 ;   
}

// "SeeDocsAt": "https://github.com/Microsoft/AirSim/blob/master/docs/settings.md",
// "SettingsVersion": 1.2,
// "CameraDefaults": {
//     "CaptureSettings": [
//       {
//         "ImageType": 0,
//         "Width": 1024,
//         "Height": 576,
//         "FOV_Degrees": 90,
//         "AutoExposureSpeed": 100,
//         "MotionBlurAmount": 0
//       }
//   ]
// },
// "Vehicles": {
//   "SUV": {
//     "VehicleType": "PhysXCar",
//     "X": 0, "Y": 0, "Z": 0,
//     "Pitch": 0, "Roll": 0, "Yaw": -20,
//   }
// }



