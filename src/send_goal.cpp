 #include"send_goal.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include </opt/ros/indigo/include/tf/transform_listener.h>

using namespace std;
 typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;  


int main(int argc, char** argv)
{  
    ros::init(argc, argv, "simple_navigation_goals");  
 
     send_goal m;
    send_goal*launch_target=new send_goal; 	  
    string com=argv[1];
    double x,y,theta;
    int mapnum,pathnum,targetnum;

      if(com=="random") 
      for(int i=0;i<20;i++)
      {     
	    launch_target->randomPose(7,11,0,3,2);
	    // launch_target->load_amcl();
	}	
      if(com=="navigation") {
	int mode=atoi(argv[2]);
	switch(mode){
	  case 1:
		      mapnum=atof(argv[3]);
		      pathnum=atof(argv[4]);
		      targetnum=atof(argv[5]);
		      launch_target->getPoseSet(mode,mapnum,pathnum,targetnum);
		      break;
	  case 2:
		      x=atof(argv[3]);
		      y=atof(argv[4]);
		      theta=atof(argv[5]);
		      launch_target->getPoseSet(mode,x,y,theta);
		      break;
	  default:break;
	}
      }
      
      if(com=="under_control") 
      launch_target->undercontrol();
      
      delete launch_target;
      launch_target=NULL;
      return 0;  
}  




 

void send_goal::getPoseSet(int flag,double x,double y,double theta)
{  
  
  double x_,y_,theta_;
  int path=y;
  cout<<"send_goal::getPoseSet()"<<endl;
  if(flag==1)
  {
    cout<<"choose"<< x <<"map"<< y<<"path"<< theta<<"target"<<endl;
      ifstream infile;
        switch(path)
	{
	  case 1: infile.open("/home/neu/json_server/src/jsonros/src/amcl.txt");break;
	  case 2: infile.open("/home/neu/json_server/src/jsonros/src/position2.txt");break;
	  case 3: infile.open("/home/neu/json_server/src/jsonros/src/position3.txt");break;
	  default:break;  
	}
      char jieshu;
      double tmp1,tmp2,tmp3;
    while((jieshu=infile.get())!=EOF)
    {
         infile>>tmp1>>tmp2>>tmp3;
	 Node node;
	node.x=tmp1;
	node.y=tmp2;
	node.theta=tmp3;
	pos.push_back(node);	
	 	 
     }
    //  pos.erase(pos.end(node));
	 for(int i=0;i!=pos.size();i++)
     {
       
       cout<<" x: "<<pos[i].x<<" y: "<<pos[i].y<<" theta: "<<pos[i].theta<<endl;
       
    }
    int count=0;
     for(int i=0;i!=pos.size();i++)
     { 
        x_=pos[i].x; y_=pos[i].y;theta_=pos[i].theta;
            send_goal*auto_target=new send_goal;
	    auto_target->launch( x_, y_,theta_);
           cout<<  "arrive the "<<count<<"target"<<endl;
	   count++;
    }
	
  } 
  
  if(flag==2)
  {
       cout<<"choose target"<<endl;
       send_goal*target=new send_goal;
       target->launch( x, y,theta);
       
  }
          
 }
 
void send_goal::launch(double x,double y,double theta)
{
      
    MoveBaseClient ac("move_base", true);  
        while(!ac.waitForServer(ros::Duration(5.0))){  
        ROS_INFO("Waiting for the move_base action server to come up");  
	}  
     tf::Quaternion quat; 
     quat.setRPY(0.0,0.0,theta);
       move_base_msgs::MoveBaseGoal goal;  
       goal.target_pose.header.frame_id = "/map";  
       goal.target_pose.header.stamp = ros::Time::now(); 
       goal.target_pose.header.seq=0.0;
       goal.target_pose.pose.position.x =x;  
       goal.target_pose.pose.position.y=y;
       goal.target_pose.pose.position.z=0.0;
       goal.target_pose.pose.orientation.w=quat.w();
       goal.target_pose.pose.orientation.x= quat.x();  
       goal.target_pose.pose.orientation.y= quat.y();  
       goal.target_pose.pose.orientation.z= quat.z();  
	
        ROS_INFO("Sending goal");  
        ac.sendGoal(goal);       
        ac.waitForResult();  
              
	if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)  {      
	  ROS_INFO("arrive the designed target");  
	
	  
	  }else{
	    ROS_INFO("The robot failed to reach the next target?");  
	    ROS_INFO("Do you want to exit ?(yes/no)"); 
	    ac.waitForResult(ros::Duration(120));  
	  }  
   
}

 
 
 
 
 
 
void send_goal::randomPose(int odrign_x,int x,int odrigin_y,int y,int number)  //生成number^2个随机数
{       
    double x_r,y_r;
	  cout<<"send_goal::randomPose"<<endl;
          /* 写入随机数文件*/
                float a[10][10];
	        int i,j;
             srand(time(0)); //初始化随机数，要写在循环外
            for(i=0;i<number;i++)
            for(j=0;j<number;j++)
        {                                                                                        //远点为odrign
                a[i][j]=rand()%(x-odrign_x)+rand()/float(RAND_MAX)+odrign_x; //x:方向产生0-x大小的随机数
                a[i][j+1]=rand()%(y-odrigin_y)+rand()/float(RAND_MAX)+odrigin_y; //y:方向产生0-y大小的随机数
            ofstream out("/home/neu/catkin_ws/src/jsonros/src/random.txt",ios::app);
              if (out.is_open())   
           {   
                out <<" "<<a[i][j]<<" "<<a[i][j+1]<<endl;
             out.close();  
            }               		
       //      printf("[%d,%d]",a[i][j],a[i][j+1]);
          }
          //打开随机数文件
           ifstream infile;   
         infile.open("/home/neu/catkin_ws/src/jsonros/src/random.txt");  
          char jieshu;
	  double tmp1,tmp2;
	  
    while((jieshu=infile.get())!=EOF)
    {
       infile>>tmp1>>tmp2;
        random_target.push_back(make_pair<double,double>(tmp1,tmp2));
 }   
           random_target.erase(random_target.end());
          vector<pair<double,double> > ::iterator iter1; 
     for(iter1=random_target.begin();iter1!=random_target.end(); iter1++)
       {
             cout<<iter1->first<<" t "<<iter1->second<<endl;
 	      //  printf("[%d,%d]",iter1->first,iter1->second);
      }
    
            MoveBaseClient ac("move_base", true);  
        while(!ac.waitForServer(ros::Duration(5.0))){  
        ROS_INFO("Waiting for the move_base action server to come up");  
      }  
       move_base_msgs::MoveBaseGoal goal;  
        vector<pair<double,double> > ::iterator tmp; 

 for(tmp=random_target.begin();tmp!=random_target.end();tmp++ ) 
  {   
                x_r=tmp->first; y_r=tmp->second;
	     send_goal*ran_target=new send_goal;
	     ran_target->launch( x_r, y_r,0);
            cout<<  "arriving the next target"<<endl;
  
  }
 
//   
//    if(remove("/home/neu/peng_ws/src/jsonros/src/random.txt"))
//    printf("Could not delete the file random.txt");
//    else printf("The targets are deleted completely ! \n");
//    random_target.clear();
  
}  
  
  
void send_goal::undercontrol()
{

cout<<"start remote control !"<<endl;   
  
}

	     
	     
	     
	     
	     
	     
