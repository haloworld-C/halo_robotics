//controller.cpp
//using ros website examples://http://wiki.ros.org/ROS/Tutorials/WritingPublisherSubscriber
//modified by halo
//time:20211029
#include "ros/ros.h" 
#include "std_msgs/Float64.h"
#include "../include/auto_car/controllers.hpp"
#include "geometry_msgs/Pose2D.h"
#include <string>
//#include<boost/bind/bind.hpp>
/* only if get a veclocity then output the pid cmd
*/
#define BOOST_BIND_NO_PLACEHOLDERS
//using namepace boost::placholders

static bool startFlag = false; 
void pidCallback(const geometry_msgs::Pose2D::ConstPtr& msg, PIDcontroller<double>* pid, ros::Publisher* sender){ 
    double currentV = msg->theta;
    double currentCmd = pid->PIDCaculator(pid->getTargetSpeed(), currentV);
	std::string mode = pid->getRunMode();
    //geometry_msgs::Pose2D chassisMsg;
	std_msgs::Float64 cmd_a;
    ROS_INFO("Target speed:%f, currentMode: %s", pid->getTargetSpeed(), mode.c_str());
    /*
	if(currentCmd > 0.1){
      chassisMsg.x = currentCmd;
      chassisMsg.theta = 0;
    }
    else if(currentCmd < -0.1){
      chassisMsg.x = 0;
      chassisMsg.theta = -currentCmd;
    }
    else{
      chassisMsg.x = 0;
      chassisMsg.theta = 0;
    }*/
	cmd_a.data = currentCmd;
	if(startFlag)
        sender->publish(cmd_a);
}

void trajCallback(const std_msgs::Float64::ConstPtr& msg, PIDcontroller<double>* pid){
    startFlag = true;
	pid->setTargetSpeed(msg->data);
    ROS_INFO("received Target speed: %f", msg->data);
}

/**
 * This tutorial demonstrates simple sending of messages over the ROS system.
 */
//extern double _v;//declare target_v
int main(int argc, char **argv) {
  /** * The ros::init() function needs to see argc and argv so that it can perform
   * any ROS arguments and name remapping that were provided at the command line.
   * For programmatic remappings you can use a different version of init() which takes
   * remappings directly, but for most command-line programs, passing argc and argv is
   * the easiest way to do it.  The third argument to init() is the name of the node.
   *
   * You must call one of the versions of ros::init() before using any other
   * part of the ROS system.
   */
    ros::init(argc, argv, "controller");

  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle n;

  /**
   * The advertise() function is how you tell ROS that you want to
   * publish on a given topic name. This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing. After this advertise() call is made, the master
   * node will notify anyone who is trying to subscribe to this topic name,
   * and they will in turn negotiate a peer-to-peer connection with this
   * node.  advertise() returns a Publisher object which allows you to
   * publish messages on that topic through a call to publish().  Once
   * all copies of the returned Publisher object are destroyed, the topic
   * will be automatically unadvertised.
   *
   * The second parameter to advertise() is the size of the message queue
   * used for publishing messages.  If messages are published more quickly
   * than we can send them, the number here specifies how many messages to
   * buffer up before throwing some away.
   */
  ros::Publisher cmd_pub = n.advertise<std_msgs::Float64>("cmd_a", 1);
  double _p, _i, _d, _v;
  //using boost::lexical_cast;
  if(ros::param::get("/controller/pid_p", _p)){
    ROS_INFO("read parameters pid_p successed:%f", _p);
  }
  else{ROS_INFO("read paramters pid_p failed!");}
  if(ros::param::get("/controller/pid_i", _i)){
    ROS_INFO("read parameters pid_i successed:%f", _i);
  }
  else{ROS_INFO("read paramters pid_i failed!");}
  if(ros::param::get("/controller/pid_d", _d)){
    ROS_INFO("read parameters pid_d successed:%f", _d);
  }
  else{ROS_INFO("read paramters pid_d failed!");}
  /*
  if(ros::param::get("/controller/target_v", _v)){
    ROS_INFO("read parameters target_V successed:%f", _v);
  }
  else{ROS_INFO("read paramters target_v failed!");}
  */
  //初始化控制器
  PIDcontroller<double> pid(_p, _i, _d);
  //pid.setPIDParameters(_p, _i, _d);
  pid.setOutputLimit(350, -15);//this limit works fine20211115
  //pid.setOutputLimit(5000, -5000);//it's seems the thottle scale is 500 and 
                                 //brake scale is 20
  //pid.setOutputLimit(350, -10);
  pid.setDeadZone(0.1);
  ros::Subscriber sub_traj = n.subscribe<std_msgs::Float64>("trajtory", 1, bind(trajCallback, _1, &pid));
  ros::Subscriber sub_chasis = n.subscribe<geometry_msgs::Pose2D>("simulink_pose", 1, bind(pidCallback, _1, &pid, &cmd_pub)); 
  //set a subscription for car speed

  ros::Rate loop_rate(100);

  /**
   * A count of how many messages we have sent. This is used to create
   * a unique string for each message.
   */
  while (ros::ok())
  {
    //std_msgs::Float64 msgSend;

    // std::stringstream ss;
    // ss << "hello world " << count;
    //msgSend.data = pid.getLastOutput();// msg.data = ss.str();

    //cmd_pub.publish(msgSend);
    //ROS_INFO("current cmd is:%s", std::to_string(msgSend.data).data()); 

    ros::spinOnce();

    loop_rate.sleep();
  }

  return 0;
}
