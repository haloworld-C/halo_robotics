#include "ros/ros.h"
#include "std_msgs/Float64.h"
#include "../include/auto_car/controllers.hpp"
void chatterCallback(const std_msgs::Float64::ConstPtr& msg, Car_halo* car, ros::Publisher* pub) 
{
  //更新车辆位置信息
  std_msgs::Float64 msgSend;
  car->update(msg->data);
  msgSend.data = car->getVelocity();
  pub->publish(msgSend);

  ROS_INFO("currentSpeed is: [%f]", msgSend.data);
  
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "listener");

  ros::NodeHandle n;
  Car_halo car;
  car.setDt(0.1); //通信频率为10HZ
  int test = 0;
  ros::Publisher pub = n.advertise<std_msgs::Float64>("car_info", 1000);
  ros::Subscriber sub = n.subscribe<std_msgs::Float64>("cmd", 1000, bind(chatterCallback, _1, &car, &pub));

  //get cmd
  //float cmd = msg->data;
  //car.update(cmd, dt);
  ros::spin();

  return 0;
}
