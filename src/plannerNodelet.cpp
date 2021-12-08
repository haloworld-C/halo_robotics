# include <auto_car/planner.h>
# include <math.h>
//This version intend to generate a sine shape velocity file
// modified as nodelet at 20211201
namespace autoCar{
namespace plan{
	void Planner::onInit(){
		nh_ = getPrivateNodeHandle();
		NODELET_DEBUG("Initialized the Nodelet");
		pub = nh_.advertise<std_msgs::Float64>("trajtory", 1);
		sub = nh_.subscribe("simulink_pose", 1 , &Planner::flagCallback, this);
		thread_ = std::thread(std::bind(&Planner::plannerThread, this));//开启一个线程，在线程中运行主要的逻辑代码
	}
	void Planner::plannerThread(){
		//TODO: 是否加入线程锁
		ros::Rate loop_rate(1);
		int counter = 0;
		std_msgs::Float64 msg;//message to send
		while(ros::ok()){
			//msg.data = 5 * sin(counter * M_PI / 500) + 10;
			msg.data = 10;
			if(counter == 1000) counter = 0;
			pub.publish(msg);
			ROS_INFO("planner published target speed in thread: %f", msg.data);
			ros::spinOnce();
			loop_rate.sleep();
		}
	}
	void Planner::flagCallback(const geometry_msgs::Pose2D::ConstPtr& msg){
		startFlag_ = true;
	}
	
}//end of namespace autoCar::controll
}//end of namespace autoCar
PLUGINLIB_EXPORT_CLASS(autoCar::plan::Planner,nodelet::Nodelet);
