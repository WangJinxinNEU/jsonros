

/*****************************************************************************
** Includes
*****************************************************************************/

#ifndef Q_MOC_RUN
   #include <ros/ros.h>  
  #include <move_base_msgs/MoveBaseAction.h>  
  #include <actionlib/client/simple_action_client.h>  
  #include <tf/transform_listener.h>
  #include<geometry_msgs/PoseWithCovarianceStamped.h>
  #include <vector>
  #include<iostream>
  #include <string>
#include <fstream>
#include <stdio.h>
#include<stdlib.h>
#include<time.h>
#include <tf/transform_datatypes.h>

#endif



/*****************************************************************************
** Class
*****************************************************************************/
using namespace std;
class send_goal{
        //   Q_OBJECT
public:
 vector<pair<double,double> >get_target;
 vector<pair<double,double> >random_target;
    void getPoseSet(int flag,double x,double y,double theta);
    void randomPose(int odrign_x,int x,int odrigin_y,int y,int number);
    void launch(double x,double y,double theta);
    void undercontrol();
    int random_pose[2];
      struct Node{
       double x;
       double y;
       double theta;
      };  
     vector<Node>pos; 

};

