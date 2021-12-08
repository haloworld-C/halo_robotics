/*planner.h
This file implemented planner nodelet.
by halo 20211201
*/
# ifndef _PLANNER_
# define _PLANNER_

# include <pluginlib/class_list_macros.h>//for pluginlib
# include <nodelet/nodelet.h>//for nodelet
# include <ros/ros.h>
# include <std_msgs/Float64.h>
# include <geometry_msgs/Pose2D.h>
# include <thread>
namespace autoCar{
namespace plan{
	class Planner : public nodelet::Nodelet{
		public:
		Planner():startFlag_(false){};
		~Planner(){};//do nothing
		virtual void onInit();
		private:
		void plannerThread();
		void flagCallback(const geometry_msgs::Pose2D::ConstPtr& msg);
		private:
		std::thread thread_;
		bool startFlag_;

		ros::NodeHandle nh_;
		ros::Publisher pub;
		ros::Subscriber sub;
	};//end of class planner
}//end of namespace controll
}//end of autoCar
# endif
