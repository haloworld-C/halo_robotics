# ifndef _LANELET2HELPER_
# define _LANELET2HELPER_
//include lanelet2 header files
# include <lanelet2_core/primitives/Lanelet.h>
# include <lanelet2_core/geometry/Area.h>
# include <lanelet2_io/Io.h>
# include <lanelet2_io/io_handlers/Factory.h>
# include <lanelet2_io/io_handlers/Writer.h>
# include <lanelet2_projection/UTM.h>
# include <lanelet2_routing/Route.h>
# include <lanelet2_routing/RoutingCost.h>
# include <lanelet2_routing/RoutingGraph.h>
# include <lanelet2_routing/RoutingGraphContainer.h>
# include <lanelet2_traffic_rules/TrafficRulesFactory.h>
# include <string>
# include <vector>

using namespace lanelet;

namespace laneletHelpers{
    class mapGraph{
        public:
        mapGraph(const std::string& filepath, const double& origin_lat, const double& origin_lon);
        void setTrafficRules(const std::string& location, const std::string& participant);
        LaneletMapPtr getMap()const{return map_;}
        routing::RoutingGraphUPtr routingGraph;
        //void pushPath(std::vector<ConstLineString3d> pathToAdd){real_path.push_back(pathToAdd);}

        private:
        std::string filepath_;
        LaneletMapPtr map_;
        traffic_rules::TrafficRulesUPtr trafficRules_;
        //std::vector<ConstLineString3d> real_path;

        

    };
    mapGraph::mapGraph(const std::string& filepath, const double& origin_lat, const double& origin_lon){
        map_ = load(filepath, projection::UtmProjector(Origin({origin_lat, origin_lon})));
        filepath_ = filepath;     
    }
    inline void 
    mapGraph::setTrafficRules(const std::string& location, const std::string& participant){
        trafficRules_ = traffic_rules::TrafficRulesFactory::create(location, participant);
        routingGraph = routing::RoutingGraph::build(*map_, *trafficRules_);//creating lanenet graph
    }
}//end of namespace laneletHelpers


# endif //_LANELET2HELPER_