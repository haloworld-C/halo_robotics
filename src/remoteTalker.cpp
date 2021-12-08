//talker.cpp
//reference:https://github.com/xiaoyuyang0901/Simulation-platform/tree/master/ROS-Simulink-Carsim
// modified by halo
// time:20211029
#include <stdio.h>
#include "ros/ros.h"
#include "geometry_msgs/Pose2D.h"
#include "std_msgs/Float64.h"
void callBack(const std_msgs::Float64::ConstPtr& cmd, geometry_msgs::Pose2D* msg){
    if(cmd->data >= 0.1){//if cmd > 0, it will output to thottle
        msg->x = cmd->data;
        msg->theta = 0;}
    else if(cmd->data < -0.1){//if cmd < 0, it will output to brake
        msg->theta = cmd->data;
        msg->x = 0;}
    else{}
    //msg->x = cmd->data;
}
int main(int argc,char **argv)
{
    ros::init(argc,argv,"remoteTalker");
    ros::NodeHandle n;
    ros::Publisher chatter_pub = n.advertise<geometry_msgs::Pose2D>("control",10);
    ros::Rate loop_rate(10);           
    geometry_msgs::Pose2D msg;
    ros::Subscriber chatter_sub = n.subscribe<std_msgs::Float64>("cmd",1000, bind(callBack, _1, &msg));
    while(ros::ok())
    {
        //msg.x = count * 0.01;      // 油门开度
        //msg.y = 0.5;               //方向盘扭矩 
        //msg.theta = 0;             //刹车气缸的压强
        //ROS_INFO("%f",msg.x);
        chatter_pub.publish(msg);
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}
