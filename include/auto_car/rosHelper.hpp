/*
name: ROS registor and handler class\
by halo at 20211129
function:intend to simplified the operations of ROS 
and for the consideration of data sharing between deffient
callback func and nodes.
*/
# ifndef _ROSHELPER_
# define _ROSHELPER_

# include <ros/ros.h>
# include <std_msgs/Bool.h>
# include <std_msgs/Float64.h>
# include <geometry_msgs/Pose2D.h>
# include <map>
# include <string>

# include "cmdAdaptor.hpp"
using std::string;
// define a base class to define the basic operation
// TODO: modify to templete
class rosBase{
public:
	rosBase(ros::NodeHandle* nodehandle, std::string config);//initialize by main 
	void creatPublisher(string messageName, int cache);
	void creatSubscriber(string messageName1, string messageName2, int cache);
	void callbackChasis(const geometry_msgs::Pose2D receiveMsg);//virtual function
														    //call back func for baseClass
														    //every sub-class should define their own
														    //call back functions
	void callbackController(const std_msgs::Float64 receiveMsg); //callback func for controller messages
	void setRecordFlag(bool recordFlag){recordFlag_ = recordFlag;}
private:
	ros::NodeHandle nh_;//ros handler
	ros::Subscriber sub1_;//subscriber object
	ros::Subscriber sub2_;//subscriber object
	ros::ServiceServer ser_; //may be used later
	ros::Publisher pub_;//publisher object
	//may use dictionary as date containor
	double cmdA_;   //controller output command
	geometry_msgs::Pose2D cmdCali_; //output cmd after calibration
	bool recordFlag_;
	
public:
	cmdAdaptorBase cali_; //store the config class
};//end of class rosBase

inline
rosBase::rosBase(ros::NodeHandle* nodeHandle, std::string config)
					:nh_(*nodeHandle), cali_(config){
	cmdA_ = 0;
	ROS_INFO("Initialize rosBase..");
	recordFlag_ = false;
}
//??this type may different with class template??
inline void
rosBase::creatPublisher(string messageName, int cache){
	ROS_INFO("Initializeing publisher..");
	pub_ = nh_.advertise<geometry_msgs::Pose2D>(messageName, cache);
}

inline void
rosBase::creatSubscriber(string messageName1, string messageName2, int cache){
	sub1_ = nh_.subscribe(messageName1, cache, &rosBase::callbackChasis, this);//?this? is nessary?
	sub2_ = nh_.subscribe(messageName2, cache, &rosBase::callbackController, this);
}

inline void
rosBase::callbackChasis(const geometry_msgs::Pose2D receiveMsg){
	static int counter = 0;
 	if(counter%100 == 0){
 	double speed = receiveMsg.theta;//current speed
    double acc = receiveMsg.y;//current accelaration                                               
    double cmd = receiveMsg.x;//current command
    cali_.recordIMU(acc, cmd, speed);
    //cali->calibration(acc, speed);
    //ROS_INFO("prediction vs received:{prediction: %f, received: %f}", acc, speed);
    ROS_INFO("received:{acc: %f, speed: %f, cmd: %f}", acc, speed, cmd);
    }
    counter++;

}

inline void
rosBase::callbackController(const std_msgs::Float64 receiveMsg){
	double currentCMD = receiveMsg.data;
	if(currentCMD > 0.1 || currentCMD < -0.1){
		cmdA_ = currentCMD;
	}//死区判断，后续可以放在PID中
	else{
		cmdA_ = 0;
	}
	if(recordFlag_){
		geometry_msgs::Pose2D sendMsg;
		if(cmdA_ > 0){
			sendMsg.x = cmdA_;//thottle
			sendMsg.y = 0;//steer angle current not used
			sendMsg.theta = 0;//brake
		}
		else{
			sendMsg.x = 0;
			sendMsg.y = 0;
			sendMsg.theta = -cmdA_;
		}
		pub_.publish(sendMsg);
		
	}//end of recordFlag_
}
/*TODO 
template<typename messageType>
class rosCalibration: public rosBase<messageType>{
	public:
	rosCalibration(std::string config, ros::NodeHandle* nodeHandle);
	void callback(const messageType& receiveMsg);//call back func for baseClass
	private:
	double cmdA_; //command caculated from PID controller
	cmdAdaptorBase cali_;//contain a class
};
template<typename messageType>
inline 
rosCalibration<messageType>::rosCalibration(std::string config, ros::NodeHandle* nodeHandle):cali_(config){
	cmdA_ = 0;
    rosBase<messageType>(nodeHandle);
}

template<typename messageType>
inline void
rosCalibration<messageType>::callback(const messageType& receiveMsg){
	static int counter = 0;
 	if(counter%100 == 0){
 	double speed = receiveMsg.theta;//current speed
    double acc = receiveMsg.y;//current accelaration                                               
    double cmd = receiveMsg.x;//current command
    cali_.recordIMU(acc, cmd, speed);
    //cali->calibration(acc, speed);
    //ROS_INFO("prediction vs received:{prediction: %f, received: %f}", acc, speed);
    ROS_INFO("received:{acc: %f, speed: %f, cmd: %f}", acc, speed, cmd);
    }
    counter++;
}
*/
# endif //end of defence
