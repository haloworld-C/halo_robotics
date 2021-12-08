# include "ros/ros.h"
# include <boost/optional/optional_io.hpp>
# include <vector>
# include <typeinfo>
# include "../include/auto_car/lanelet2Helper.hpp"
# include <cstdio>


// we want assert statements to work in release mode
//#undef NDEBUG
using namespace laneletHelpers;
using namespace lanelet;
using namespace std;

namespace {
  std::string MapPath = "/home/westwell/workspace/lanelet2/maps/qingpu.osm";
  std::string outWritePath = "../res/maps/output1.osm";
  std::string tempfile(const std::string& name) {
    char tmpDir[] = "/tmp/lanelet2_example_XXXXXX";
    auto* file = mkdtemp(tmpDir);
    if (file == nullptr) {
      throw lanelet::IOError("Failed to open a temporary file for writing");
    }
    return std::string(file) + '/' + name;
  }
}  // namespace

int main(int argc, char **argv) {
  // this tutorial finally shows you how to use routing graphs.
  //ros::init(argc, argv, "lanelet2");
  vector<ConstLineString3d> realPath;
  mapGraph mapQingPu(MapPath, 49, 8.4);
  mapQingPu.setTrafficRules(Locations::Germany, Participants::Vehicle);
  auto map = mapQingPu.getMap();
  ConstLanelet laneletStart  = map->laneletLayer.get(2029);
  ConstLanelet laneletTarget = map->laneletLayer.get(2002);

  routing::LaneletPaths paths = mapQingPu.routingGraph->possiblePaths(laneletStart, 10, 0, true);
  Optional<routing::LaneletPath> shortestPath = mapQingPu.routingGraph->shortestPath(laneletStart, laneletTarget, 1);
  LaneletMap shortestMap;
  assert(!!shortestPath);
  for(auto& laneletInPath : *shortestPath){
    //cout << typeid(pointInPath).name() << "\t";
    auto centerLine = laneletInPath.centerline();
    //cout << typeid(centerLine).name() << "\t";
    realPath.push_back(centerLine);
    shortestMap.add(map->laneletLayer.get(laneletInPath.id()));
  }
  write(tempfile("output.osm"), shortestMap);
  
  
  cout << "可能路径的数量为： " << paths.size() << endl;  

  //add here
  //ROS_INFO("works fine!");
  cout << "works fine!" << endl;
  return 0;
}

