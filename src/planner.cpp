# include "ros/ros.h"
# include "std_msgs/Float64.h"
# include "geometry_msgs/Pose2D.h"
# include <math.h>
//This version intend to generate a sine shape velocity file
bool startFlag = false;
void callback(const geometry_msgs::Pose2D::ConstPtr& msg){
	startFlag = true;
}
int main(int argc, char **argv){
    //std::cout << "This is a planner~" << std::endl;
	ros::init(argc, argv, "planner");
	ros::NodeHandle n;
	ros::Publisher trajtory_pub = n.advertise<std_msgs::Float64>("trajtory", 1);
	ros::Subscriber trajtory_sub = n.subscribe<geometry_msgs::Pose2D>("simulink_pose", 1, callback);
	ros::Rate loop_rate(1);
	int counter = 0;
	double targetV = 5;
	int direction = 1;
	std_msgs::Float64 msg;
	while(ros::ok()){
	if(startFlag){
		//msg.data = 5 * sin(counter * M_PI / 500) + 5;
		msg.data = 10;
		//if((counter % 10) == 0){
		//	targetV += 0.2;
		//}
		if(counter == 1000){
			counter = 0;
		}
		//msg.data = targetV;	
		trajtory_pub.publish(msg);
		ROS_INFO("planner published target speed: %f", msg.data);}
		ros::spinOnce();
		loop_rate.sleep();
	}

    return 0;
}
