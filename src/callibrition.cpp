//calibration.cpp
//normally the node process only
//do some settings, the workflow is hidden
//in the callback function
# include "cmdAdaptor.hpp"
# include "ros/ros.h"
# include "geometry_msgs/Pose2D.h"
# include "std_msgs/Float64.h"
# include <string>
# include <iostream>
# include <rosHelper.hpp>
using namespace std;

//test for interplation
namespace{
	string cwd = string(get_current_dir_name());
	string config_path = cwd + "/config/";
}//namespace
int main(int argc, char **argv){
	ros::init(argc, argv, "calibration");
	ros::NodeHandle n;
	//initialise calibration object
	//cmdAdaptorBase cali(config_path);
	rosBase rosOB(&n, config_path);
	//calibration.readConf();
	rosOB.setRecordFlag(true);
	rosOB.creatPublisher("caliCMD", 1);
	rosOB.creatSubscriber("simulink_pose", "cmd_a", 1);
	//TODO 写为闭环形式
	ros::Rate loop_rate(100);
	while(ros::ok()){
		ros::spinOnce();
		loop_rate.sleep();
	}
	rosOB.cali_.writeConf();//write to config

	return 0;
}

