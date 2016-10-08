#include "listener.h"
#include <iostream>

using namespace std;
int map_flag=0;int tf_flag=0;int scan_flag=0;
Listener::Listener()
{  
    boost::shared_ptr<nav_msgs::OccupancyGrid const>sharedPtr;
   pose_sub=n.subscribe<geometry_msgs::PoseWithCovarianceStamped>("/amcl_pose", 1000, &Listener::goal_Callback, this);
   velocity_sub = n.subscribe<geometry_msgs::Twist>("/mobile_base/commands/velocity",1000, &Listener::velocity_Callback,this);
    map_sub = n.subscribe<nav_msgs::OccupancyGrid>("/map",1000, &Listener::map_Callback,this);
    tf_sub = n.subscribe<tf::tfMessage>("/tf",1000, &Listener::tf_Callback,this);
    scan_sub = n.subscribe<sensor_msgs::LaserScan>("/scan",1000, &Listener::scan_Callback,this);
    goal_sub = n.subscribe<geometry_msgs::PoseStamped>("move_base_simple/goal",100,&Listener::sendgoal_Callback,this);
    //geometry_msgs::Twist::ConstPtr twist1=ros::topic::waitForMessage<geometry_msgs::Twist>("/mobile_base/commands/velocity",ros::Duration(2.0));
      sharedPtr=ros::topic::waitForMessage<nav_msgs::OccupancyGrid>("/map",ros::Duration(5.0));
    
       protocol_sub = n.subscribe<jsonros::protocol>("/protocol",10, &Listener::protocol_Callback,this);
//     if(sharedPtr==NULL)
//     {cout<<"error:"<<endl;}
//     else
//     {cout<<"ok"<<endl;}
    //else
     // vel= *sharedPtr;
      
   // ROS_INFO("Waiting for the topic to come up");  
  
}

void Listener::goal_Callback(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr& msg)
{        
	   std_msgs::String frame;
	   frame.data="map";
           position[0]=msg->pose.pose.position.x;
	   position[1]=msg->pose.pose.position.y;
 	   position[2]=msg->pose.pose.position.z;
	   position[3]=msg->pose.pose.orientation.x;
	   position[4]=msg->pose.pose.orientation.y;
	   position[5]=msg->pose.pose.orientation.z;
	   position[6]=msg->pose.pose.orientation.w; 
	   cout<<"receiv data"<<endl;
	   cout<<position[0]<<endl;

}
void Listener::velocity_Callback(const geometry_msgs::Twist::ConstPtr& twist)
{
//     //ar_pose::ARMarkersConstPtr msg_mark = ros::topic::waitForMessage<ar_pose::armarkers>("ar_pose_marker");
//     geometry_msgs::Twist::ConstPtr twist1=ros::topic::waitForMessage<geometry_msgs::Twist>("/mobile_base/commands/velocity1",ros::Duration(1.0));
//   while (!twist1)
//   {
//     cout<<"error"<<endl;
//     ROS_INFO("Waiting for the topic to come up");  
//   }
   cout<<"receive speed data"<<endl;
    velocity[0]=twist->linear.x;
    velocity[1]=twist->linear.y;
    velocity[2]=twist->linear.z;
    velocity[3]=twist->angular.x;
    velocity[4]=twist->angular.y;
    velocity[5]=twist->angular.z; 
}

void Listener::map_Callback(const nav_msgs::OccupancyGrid::ConstPtr& map_data)
{ 
       
	map_flag=map_flag+1;
       //cout<<"map_data received|gmapping status:ok"<<endl;
}

void Listener::tf_Callback(const tf::tfMessage::ConstPtr& tf_data)
{
       //cout<<"tf_data received|tf status:ok"<<endl;
  
}
 void Listener::scan_Callback(const sensor_msgs::LaserScan::ConstPtr& scan_data)
 {
                scan_flag=scan_flag+1;
  // cout<<"scan_data received|scan status:ok"<<scan_flag<<endl;
    
}
void Listener::sendgoal_Callback(const geometry_msgs::PoseStamped::ConstPtr& goal_data)
{
  
  //cout<<"goal_data received|goal status:ok"<<endl;
  
  
}

void Listener::touchuan()
{               
        char buff[1024];
   ros::Publisher protocolPub = n.advertise<jsonros::protocol> ( "protocol",50,true );
                       jsonros::protocol msg;
		      
		      msg.header.seq = 1;
               msg.header.stamp = ros::Time::now();
                msg.header.frame_id = "touchuan";
 	         msg.referenceStamp = ros::Time::now();    
                      msg.byte1 = 0;
		      msg.byte2 = 1;
		      msg.byte3 = 2;
		      msg.byte4 = 3;
		      msg.byte5 = 4;
		      msg.byte6 = 5;
		      msg.byte7 = 6;
		      msg.byte8 = 7;
		      
		       ROS_INFO("S1:%02x",msg.byte1 );
                       ROS_INFO("S2:%02x",msg.byte2 );
                       ROS_INFO("S3:%02x",msg.byte3);
                       ROS_INFO("S4:%02x",msg.byte4 );
                       ROS_INFO("S5:%02x",msg.byte5 );
                       ROS_INFO("S6:%02x",msg.byte6 );
                       ROS_INFO("S7:%02x",msg.byte7 );
                       ROS_INFO("S8:%02x",msg.byte8 );
		      
		      protocolPub.publish (msg);
 
}


 void Listener::protocol_Callback(const jsonros::protocol::ConstPtr& msg)
    {
       char buf1[1024];
      
      ROS_INFO("touchuan goal ");
    
      buf1[0] = msg->byte1;    
      buf1[1] = msg->byte2;
      buf1[2] = msg->byte3;
      buf1[3] = msg->byte4;
      buf1[4] = msg->byte5;
      buf1[5] = msg->byte6; 
      buf1[6] = msg->byte7;
      //buf1[6] = 0x43+buf1[2]+buf1[3]+buf1[4]+buf1[5];
      //buf1[6] = buf1[0]+buf1[1]+buf1[2]+buf1[3]+buf1[4]+buf1[5];
      buf1[7] = msg->byte8;
      
       ROS_INFO("S1:%02x",buf1[0] );
       ROS_INFO("S2:%02x",buf1[1] );
       ROS_INFO("S3:%02x",buf1[2] );
       ROS_INFO("S4:%02x",buf1[3] );
       ROS_INFO("S5:%02x",buf1[4] );
       ROS_INFO("S6:%02x",buf1[5] );
       ROS_INFO("S7:%02x",buf1[6] );
       ROS_INFO("S8:%02x",buf1[7] );
	      
      // printf("S4:%02x",buf1[7] );
      
      
    }










 int main(int argc,char**argv)
 { 
   
 
    ros::init(argc,argv,"listener");
   
    Listener m;
    //m.touchuan();
//     while(ros::ok)
//     {   char key;
//        std::cin>>key;
//        switch(key)
//        {
// 	 case'0':m.touchuan();break;
// 	 
// 	 default:break;
//        } 
//      }
    ros::spin();
    return 0;
   
}

