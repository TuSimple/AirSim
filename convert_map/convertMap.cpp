#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "tsmap/tsmap.h"

using namespace tsmap2;

std::vector<std::vector<std::vector<double>>> SampleMap(
    std::shared_ptr<TSMap> tsmap, 
    tsmap2::Point3d &cur_point3d)
{
  constexpr double kVizDistance = 3000;
  std::vector<const Roadmarker *> road_markers =
      tsmap->GetRoadmarkersByRange(cur_point3d, kVizDistance, Roadmarker::Type::WHITE_SOLID_LINE);
  std::vector<Roadmarker> bounds;
  for (auto &b : road_markers)
    if (b->GetType() != Roadmarker::Type::VIRTUAL_LINE) bounds.emplace_back(*b);
  std::vector<std::vector<std::vector<double>>> sampled_bounds;
  auto cos_theta_sq_threshold = std::pow(std::cos(1.5 / 180 * M_PI ), 2);
  for (auto &bound : bounds) {
    auto &points = bound.GetPts()[0];
    if (points.empty()) continue;
    int n_skipped = 0;
    std::vector<std::vector<double>> one_lane;
    tsmap2::Point3d last_added_pt = points[0];
    one_lane.push_back({last_added_pt.x, last_added_pt.y, last_added_pt.z});
    for (uint i = 1; i < points.size() - 1; ++i) {
      bool should_append = false;
      auto diff1 = points[i + 1] - points[i];
      auto diff2 = points[i + 1] - last_added_pt;
      auto dot_product = diff1.x * diff2.x + diff1.y * diff2.y;
      auto self_pow = (diff1.x * diff1.x + diff1.y * diff1.y) *
          (diff2.x * diff2.x + diff2.y * diff2.y);
      if ( std::fabs(self_pow) < 0.0001 ) continue;
      auto cos_theta_sq = dot_product * dot_product / self_pow;
      if (cos_theta_sq < cos_theta_sq_threshold)
        should_append = true;
      else
        n_skipped += 1;
      if (should_append || n_skipped > 10) {
        n_skipped = 0;
        last_added_pt = points[i];
        one_lane.push_back({points[i].x, points[i].y, points[i].z});
      }
    }
    one_lane.push_back({points.back().x, points.back().y, points.back().z});
    sampled_bounds.emplace_back(one_lane);
  }
  return sampled_bounds;
}

bool IsValid(double v){
    if( std::isnan(v) || std::isinf(v) ){ return false; }
    return true; 
}
bool IsValid(std::vector<double> const& v){
    for (auto const& a: v)
    {
        if (not IsValid(a))
        {
            return false;
        }
    }
    return true ;
}
bool sortX(std::vector<double> const& a, std::vector<double> const& b ){
    return a[0] < b[0] ; 
}

int main(int argc, char **argv) {
  std::shared_ptr<TSMap> tsmap = std::make_shared<TSMap>();
  std::string input_map_path = "/mnt/truenas/scratch/maps/devdata/I-10_Tucson2Phoenix_20180807.tsmap";
  // std::string input_map_path = "/mnt/truenas/scratch/data/map/hdmap/data/MAP_new/I-10_Phoenix2Tucson_20180607.hdmap";
  std::cout << "Loading " << input_map_path << " ..." << std::endl;
  tsmap->LoadFile(input_map_path);
  std::cout << tsmap->ToString() << std::endl;


  // const std::vector<std::shared_ptr<const Roadmarker>> all_road_markers = 
  //   tsmap->GetAllRoadmarkers() ; 
  // std::vector<std::shared_ptr<const Roadmarker>> white_line_road_markers ; 
  // for (auto const& road_marker : all_road_markers )  {
  //   if ( road_marker->GetType() == Roadmarker::Type::WHITE_SOLID_LINE ) {
  //     white_line_road_markers.push_back( road_marker );
  //   }
  // }

  std::vector<std::shared_ptr<const Lane>> all_lanes = tsmap->GetAllLanes();
  tsmap2::Point3d cur_point ; 
  for (auto lane: all_lanes)  {
    std::cerr << " Id = " << lane->GetId() ;
    std::cerr << " ; GroupId() = " << lane->GetGroupId() ;
    std::cerr << " ; Length() = " << lane->GetLength() << std::endl; 
    for (auto const& point: lane->GetPolygon() )    {
        std::cerr << point << ", " ;
        cur_point = point ; 
        break;
    }
    std::cerr << std::endl ; 
    break;
  }

  auto samples = SampleMap(tsmap, cur_point) ; 
  std::cerr << " samples.size() = " << samples.size() << std::endl ; 
  std::cerr << " samples[0].size() = " << samples[0].size() << std::endl ; 
  std::cerr << " samples[0][0].size() = " << samples[0][0].size() << std::endl ; 
  std::cerr << " samples[0][0] = " << samples[0][0][0] << ", " << samples[0][0][1] << ", " << samples[0][0][2] << std::endl ; 
  std::cerr << " samples[0][1] = " << samples[0][1][0] << ", " << samples[0][1][1] << ", " << samples[0][1][2] << std::endl ; 
  std::cerr << " samples[0][2] = " << samples[0][2][0] << ", " << samples[0][2][1] << ", " << samples[0][2][2] << std::endl ; 





  using json = nlohmann::json;
  json j;  

  j["SeeDocsAt"] = "https://github.com/Microsoft/AirSim/blob/master/docs/settings.md" ; 

  j["SettingsVersion"] = 1.2 ; 

  json cap ; 

  cap["ImageType"] = 0 ; 
  cap["Width"] = 1024 ; 
  cap["Height"] = 576 ; 

  j["CameraDefaults"]["CaptureSettings"] =  json::array( { cap } ) ; 

  // j["CameraDefaults"]["CaptureSettings"] = json::array( {"ImageType", "0"} ) ; 

  // j["CameraDefaults"]["CaptureSettings"]["ImageType"] = 0 ; 
  // j["CameraDefaults"]["CaptureSettings"]["Width"] = 1024 ; 
  // j["CameraDefaults"]["CaptureSettings"]["Height"] = 576 ; 

  j["Vehicles"]["SUV"]["VehicleType"] = "PhysXCar" ; 
  j["Vehicles"]["SUV"]["X"] = 0 ; 
  j["Vehicles"]["SUV"]["Y"] = -6 ; 
  j["Vehicles"]["SUV"]["Z"] = 0 ; 
  j["Vehicles"]["SUV"]["Pitch"] = 0 ; 
  j["Vehicles"]["SUV"]["Roll"] = 0 ; 
  j["Vehicles"]["SUV"]["Yaw"] = 0 ; 

  j["SimMode"] = "Car" ; 



  std::map<int, std::map<int, std::vector<double> > > lanesAll ;
  for (int i = 0; i < samples.size() ; ++i)  {
    for (int lane = 0; lane < samples[i].size() ; ++lane)   {
      std::vector<double> point(3,0);
      point[0] = samples[i][lane][0] ; 
      point[1] = samples[i][lane][1] ; 
      point[2] = samples[i][lane][2] ; 
      lanesAll[lane][i] = point ; 
    }
  }

  
  std::vector<std::vector<std::vector<double> > > lanesAll_Sorted ;
  for ( int lane = 0; lane < lanesAll.size() ; ++lane)  {
    int ip = 0 ; 
    std::vector<std::vector<double>> one_lane ; 
    for (auto& lane2point: lanesAll[lane]) {
        one_lane.push_back({lane2point.second});
    }
    // std::sort(one_lane.begin(), one_lane.end(), sortX) ; 
    lanesAll_Sorted.push_back(one_lane) ;
  }

  for ( int lane = 0; lane < lanesAll_Sorted.size() ; ++lane)  {
    int ip = 0 ; 
    for (auto& lane2point: lanesAll_Sorted[lane]) {
        j["RoadPoints"][ std::to_string(lane) ][ std::to_string(ip) ]["X"] = lane2point[0] ; 
        j["RoadPoints"][ std::to_string(lane) ][ std::to_string(ip) ]["Y"] = lane2point[1] ; 
        j["RoadPoints"][ std::to_string(lane) ][ std::to_string(ip) ]["Z"] = lane2point[2] ; 
        ip++ ; 
    }
  }

  std::cerr <<"\n\n\n\n" ; 

  for (int l = 0; l < 4; ++l) {
    int Npoint = j["RoadPoints"][ std::to_string(l) ].size() ; 
    std::cerr <<" Npoint = " << Npoint << std::endl ; 
    for (int p = 0; p < Npoint ; ++p)  {
        std::cerr << j["RoadPoints"][ std::to_string(l) ][ std::to_string(p) ]["X"] << " " 
                  << j["RoadPoints"][ std::to_string(l) ][ std::to_string(p) ]["Y"] << std::endl ; 

    }
  }
  std::cerr <<"\n\n\n\n" ; 


  std::ofstream plotfile;
  plotfile.open("points.txt");
  for (int l = 0; l < 1; ++l) {
    int Npoint = j["RoadPoints"][ std::to_string(l) ].size() ; 
    std::cerr <<" Npoint = " << Npoint << std::endl ; 
    for (int p = 0; p < Npoint ; ++p)  {
        plotfile << j["RoadPoints"][ std::to_string(l) ][ std::to_string(p) ]["X"] << " " 
                 << j["RoadPoints"][ std::to_string(l) ][ std::to_string(p) ]["Y"] << std::endl ; 
    }
  }
  plotfile.close();

  // std::cerr << j << std::endl ; 




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



