

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
    one_lane.push_back({last_added_pt.x, last_added_pt.y, 0.0});
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
      if (should_append || n_skipped > 100) {
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


int main(int argc, char **argv) {
  std::shared_ptr<TSMap> tsmap = std::make_shared<TSMap>();
  std::string input_map_path = "/mnt/truenas/scratch/maps/devdata/I-10_Tucson2Phoenix_20180807.tsmap";
  std::string output_map_path = "/mnt/truenas/scratch/maps/devdata/I-10_Tucson2Phoenix_20180807.saved.tsmap";
  std::cout << "Loading " << input_map_path << " ..." << std::endl;
  tsmap->LoadFile(input_map_path);
  std::cout << tsmap->ToString() << std::endl;


  std::vector<std::shared_ptr<const Lane>> lanes = tsmap->GetAllLanes();

  constexpr double kVizDistance = 3000;

  // std::vector<const Roadmarker *> road_markers = 
    // GetAllRoadmarkers();

  std::vector<std::vector<Point3d>> bounds = 
    // tsmap->GetSampledBoundPts(12 * M_PI / 180 );
    tsmap->GetSampledBoundPts(1);



  std::cerr << "bounds.size()=" << bounds.size() << std::endl; 
  std::cerr << "bounds[0].size()= " << bounds[0].size() << std::endl; 

  std::cerr << "bounds[0][0] = " << bounds[0][0] << std::endl; 
  std::cerr << "bounds[1][0] = " << bounds[1][0] << std::endl; 
  std::cerr << "bounds[0][1] = " << bounds[0][1] << std::endl; 
  std::cerr << "bounds[1][1] = " << bounds[1][1] << std::endl; 

  const std::vector<std::shared_ptr<const Roadmarker>> all_road_markers = 
    tsmap->GetAllRoadmarkers() ; 

  std::vector<std::shared_ptr<const Roadmarker>> white_line_road_markers ; 
  for (auto const& road_marker : all_road_markers )  {
    if ( road_marker->GetType() == Roadmarker::Type::WHITE_SOLID_LINE ) {
      white_line_road_markers.push_back( road_marker );
    }
  }

  // std::vector<std::vector<std::vector<double>>> SampleMap(
  //     std::shared_ptr<TSMap> tsmap, 
  //     tsmap2::Point3d &cur_point3d)

  tsmap2::Point3d cur_point ; 
  for (auto lane: lanes)  {
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
  // std::cerr << " samples[0][1] = " << samples[0][1] << std::endl ; 
  
}

