#ifndef LISTENER_H_H
#define LISTENER_H_H
#include "ros/ros.h"
#include "std_msgs/String.h"
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <geometry_msgs/Twist.h>  
#include <nav_msgs/OccupancyGrid.h>
#include <tf/tfMessage.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/PoseStamped.h>
//#include "/home/neu/json_server/devel/include/jsonros/protocol.h"
#include "jsonros/protocol.h"
class Listener{
           
public:
      ros::Subscriber pose_sub, velocity_sub,map_sub,tf_sub,scan_sub,goal_sub,protocol_sub;
      ros::NodeHandle n;   
      Listener();
      void goal_Callback(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr&msg);
      void  velocity_Callback(const geometry_msgs::Twist::ConstPtr& twist);
      void  map_Callback(const nav_msgs::OccupancyGrid::ConstPtr& map_data);
      void  tf_Callback(const tf::tfMessage::ConstPtr& tf_data);
      void  scan_Callback(const sensor_msgs::LaserScan::ConstPtr& scan_data);
      void  sendgoal_Callback(const geometry_msgs::PoseStamped::ConstPtr& goal_data);
      double position[7],velocity[6];
      void touchuan();
      void protocol_Callback(const jsonros::protocol::ConstPtr& msg);
};
#endif
