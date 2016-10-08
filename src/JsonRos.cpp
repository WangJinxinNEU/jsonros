#include <ros/ros.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <json/json.h>
#include <sys/socket.h>
#include <string>
#include <sys/types.h>
#include <arpa/inet.h>
#include <boost/graph/graph_concepts.hpp>
#include "std_msgs/String.h"
#include<std_msgs/Int64.h>
#include<std_msgs/Int32.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <geometry_msgs/Twist.h>  
#include "geometry_msgs/Quaternion.h"
#include <tf/transform_datatypes.h>
#include <tf/LinearMath/Matrix3x3.h>
#include "send_goal.h"
#include "jsonros/protocol.h"
#include <sensor_msgs/LaserScan.h>
#include <nav_msgs/OccupancyGrid.h>
#include <tf/tfMessage.h>
#include <actionlib_msgs/GoalStatusArray.h>
#include <vector>
using namespace std;
#ifndef MAX_SIZE
#define MAX_SIZE 1024
#endif



json_object * jobj = json_object_new_object();                            //存放client端传来的json数据
 
int jsonflag=0,detectionflag=0,followflag=0,chargeflag=0 ,detectionbackflag=0;				/*jsonflag判断是否是json模式的标志位;detectionflag判断人体检测开始或者停止的标志位*/

int closeflag=0,changeflag=0;//closeflag用来判断客户端是否关闭   changeflag用来检测检测到的人体是否发生变化

int detection_num=0; //检测到的人体的个数
 
char buff[MAX_SIZE],callbuff[MAX_SIZE];  //buff为接收client 端缓冲区    //callbuff为server向client缓冲区followflag判断是否进行人体跟踪的标志位
																    													   
string id;//用来保存client发来的id  以字符串的格式
																    													   
int CreatePID(string comname);    //创建一个线程运行launch文件

void *run(void *arg);                           //子线程
void *listen_topic(void *arg);                           //子线程
void *call_back(void *arg);

char* int2char (int id);//将client端发来的id转化成int

void detection_callback(const std_msgs::Int32::ConstPtr& msg)
{
    detection_num=msg->data;
    cout<<"detection_callback_detection_num"<<detection_num<<endl;
}

																    
int main(int argc,char **argv)
{
    ros::init(argc,argv,"JsonRos");
    ros::NodeHandle n;    
    ros::Subscriber pose_sub, velocity_sub;
    boost::shared_ptr<nav_msgs::OccupancyGrid const>map_Ptr;
    boost::shared_ptr<sensor_msgs::LaserScan const>scan_Ptr;
    boost::shared_ptr<tf::tfMessage const>tf_Ptr;
    boost::shared_ptr<geometry_msgs::PoseWithCovarianceStamped const>amcl_Ptr;
    boost::shared_ptr< geometry_msgs::PoseStamped const>goal_Ptr;
    boost::shared_ptr< actionlib_msgs::GoalStatusArray const>goal_status_Ptr;
    //ros::Subscriber people_sub_ = n.subscribe("/body_tracker/people_num", 50,detection_callback1); 
    int listenfd = 0, connfd = 0,res=0,flag=0;   //
    struct sockaddr_in serv_addr;
    unsigned int port=8888;  
    listenfd = socket ( AF_INET, SOCK_STREAM, 0 );
    if(listenfd<0)  
    {  
        perror("socket");  
        exit(-1);  
    }  
   
    cout<<"create socket success,fd:"<<listenfd<<endl;  
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl ( INADDR_ANY );
    serv_addr.sin_port = htons ( port );
    res=bind ( listenfd, ( struct sockaddr* ) &serv_addr, sizeof ( serv_addr ) );
    if(res!=0)  
    {  
        perror("bind");  
        close(listenfd);  
        exit(-1);  
    }  
    cout<<"bind success."<<endl;  
    
    res=listen ( listenfd, 5 );
    if(res!=0)  
    {  
        perror("bind");  
        close(listenfd);  
        exit(-1);  
    }  
    cout<<"listen success."<<endl;  
    
    geometry_msgs::PoseStamped set_msg;
    ofstream fin1("./error.txt");
    if(fin1.is_open())
    {
      fin1<<"";
    }
    fin1.close();
    ros::Rate loop_rate(20);  
    //ros::Subscriber people_sub_ = n.subscribe("/body_tracker/people_num", 100,detection_callback);  
    cout<<"server open|status:ok !!!"<<endl;
    jsonros::protocol msg;                           																					//什么消息类型
       
   //4,waiting and accept client connect request...  
    int client_fd=0;  
    unsigned client_port=0;  
    char client_ip[20];  
    pthread_t tid,tid_listen_topic,tid_callback;  
    pthread_mutex_t mutex;
    sockaddr_in client_addr;  
    socklen_t socklen = sizeof(client_addr);  
      
    cout<<"server listenning at port "<<port<<endl;  
	      
    while ( ros::ok() )   
    {
	    //ros::Subscriber people_sub_ = n.subscribe("/body_tracker/people_num", 50,detection_callback); 
	
           client_fd = accept(listenfd,(struct sockaddr *)&client_addr,&socklen);  
           if(client_fd<0)  
           {  
               perror("accept");  
               close(listenfd);  
               exit(-1);  
            }      
  
       //get client ip and port  
        inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,client_ip,sizeof(client_ip));  
        client_port = ntohs(client_addr.sin_port);  
        cout<<"client "<<client_ip<<":"<<client_port<<" connected,client fd:"<<client_fd<<endl;  
		     		    
       //create new thread handle client  
        pthread_create(&tid,NULL,run,(void *)client_fd);      //将client_fd作为参数传递给子线程
	//pthread_create(&tid_listen_topic,NULL,listen_topic,NULL);      //将client_fd作为参数传递给子线程
	pthread_create(&tid_callback,NULL,call_back,(void *)client_fd);      //将client_fd作为参数传递给子线程
	pthread_detach(tid_callback); 
	//pthread_detach(tid_listen_topic);  	
        pthread_detach(tid);  	        
	
	cout<<"cehsi1"<<endl;
	//ros::MultiThreadedSpinner spinner1(1); // Use 4 threads
	//spinner1.spin();
	ros::spinOnce();
	loop_rate.sleep();
    }
	cout<<"cehsi2"<<endl;

        close(listenfd);  
        return 0;  
}




void *run(void *arg)
{
    
    ros::NodeHandle nh;    
    boost::shared_ptr<nav_msgs::OccupancyGrid const>map_Ptr;
    boost::shared_ptr<sensor_msgs::LaserScan const>scan_Ptr;
    boost::shared_ptr<tf::tfMessage const>tf_Ptr;
    boost::shared_ptr<geometry_msgs::PoseWithCovarianceStamped const>amcl_Ptr;
    boost::shared_ptr< geometry_msgs::PoseStamped const>goal_Ptr;
    boost::shared_ptr< actionlib_msgs::GoalStatusArray const>goal_status_Ptr;                                     //这些参数的作用
    string  callback("");
    jsonros::protocol msg;                                          										//什么消息类型
    ros::Publisher protocolPub = nh.advertise<jsonros::protocol> ( "protocol",50,true );  //这是用来发布什么消息的？ 谁来接收？
    //ros::Subscriber people_sub_ = nh.subscribe("/body_tracker/people_num", 50,detection_callback); 
    cout<<"touchuan transmit success!!!"<<endl;       //默认是透传模式吗？
    int client_fd = (long)arg;     // client_fd来代表是json模式还是透传模式
    cout<<"client_fd         :"<<client_fd<<endl;
    
    
    int len=0;  
    char buf[1024];  
    
    while(1)
    {
	
	 json_object *callobj = json_object_new_object();      
	 json_object *detection_back = json_object_new_object();                   //返回给client端人体检测的json对象
	 json_object *detection_back_body = json_object_new_object();      //返回给client端json对象里面的body
	 json_object *detection_back_found = json_tokener_parse("{\"id\":\"1\",\"iface\":\"detection.result\",\"body\":{\"reason\":\"FOUNG\",\"num\":\"1\"}}");//检测人体回调给客户端的json对象
	 json_object *detection_back_found_body = json_tokener_parse("{\"reason\":\"FOUNG\",\"num\":\"1\"}");//检测人体回调给客户端的json对象
	 json_object *detection_back_lost = json_tokener_parse("{\"id\":\"1\",\"iface\":\"detection.result\",\"body\":{\"reason\":\"LOST\"}}");//检测人体回调给客户端的json对象
// 	if ( strcpy ( callbuff, json_object_to_json_string ( detection_back_found) ) == NULL )
// 	{
// 	    perror ( "strcpy" );
// 	    exit(1);
// 	}
// 
// 	if ( send (  client_fd, callbuff, strlen ( callbuff ) ,0) == -1 )string id;
// 	{
// 	    perror ( "write" );
// 	    exit(1);
// 	}
                      //发送完一次以后就将callbuff清零
	 cout<<"监听函数之前"<<endl;
        len = recv(client_fd,buff,1024,0);//read data from client          
        cout<<"server receive data!"<<endl; 
        if(len==0)  
        {  
            cout<<"client disconnected."<<endl;  
	    closeflag=1;
            close(client_fd);  
            break;  
        }  
        if ( buff[0]==0x7B )
        {
	     
            cout<<"json transmit sucess！"<<endl;            
    		    
	    jsonflag=1;
	    jobj = json_tokener_parse ( buff );							//将json格式的string转化为json对象：失败返回null   这是client传来的数据
	    cout<<json_object_to_json_string ( jobj )<<endl;
	    //double pos_x=position[0],pos_y=position[1],pos_theta=position[2];
	    json_object *people_detection; //*mapping,*pose,*conmode,*nav,*amcl_pos,*status,//  json_object *mapping与json_object *jobj = json_object_new_object()的区别？
	    json_object *people_follow;  
	    json_object *detection_back;  
	    //int map_n=0,path_n=0,target_n=0;
	    if(json_object_object_get_ex(jobj,"id",NULL))
	    {
		id=json_object_get_string(json_object_object_get (jobj,"id" ));string id;
	    }
	    /*判断json消息是不是进行人体检测*/
	    if(json_object_object_get_ex(jobj,"iface",NULL))
	    {             //action.mapping现在没有了  //json_object_object_get_ex函数的作用
		string peopledetection=json_object_get_string ( json_object_object_get(jobj,"iface") );	//从json中按名字取一个对象。从jobj中获取键值为action.mapping的子对象//jobj是什么类型 mapping就是什么类型
		if(peopledetection=="detection.switch")
		{
		    if(json_object_object_get_ex(jobj,"body",NULL))
		    {
			json_object *detectionbody=json_object_object_get (jobj,"body" );  
			if(json_object_object_get_ex(detectionbody,"switch",NULL))
			{
			    string detectionswitch=json_object_get_string(json_object_object_get (detectionbody,"switch") );
			    if(detectionswitch=="OPEN" ){detectionflag=1;detectionbackflag=1;}            //
			    if(detectionswitch=="CLOSE" ){detectionflag=2;detectionbackflag=1;changeflag=0;detection_num=0;}
			}
			
		    }
		//backflag=1;
		}
	    }
		
	    /*判断json消息是不是进行人体跟踪*/
	     if(json_object_object_get_ex(jobj,"iface",NULL))
	     {             //action.mapping现在没有了  //json_object_object_get_ex函数的作用
		string peopledetection=json_object_get_string ( json_object_object_get(jobj,"iface") );	//从json中按名字取一个对象。从jobj中获取键值为action.mapping的子对象//jobj是什么类型 mapping就是什么类型
		if(peopledetection=="detection.trace")
		{
		    if(json_object_object_get_ex(jobj,"body",NULL))
		    {
			json_object *followbody=json_object_object_get (jobj,"body" );  
			if(json_object_object_get_ex(followbody,"switch",NULL))
			{
			    string followswitch=json_object_get_string(json_object_object_get (followbody,"switch") );
			    if(followswitch=="START" ){followflag=1;}            //
			    if(followswitch=="STOP" ){followflag=2;}
			}
			
		    }
		//backflag=1;
		}
	    }
	    
	    /*判断json消息是不是查找充电站*/
	     if(json_object_object_get_ex(jobj,"iface",NULL))
	     {             //action.mapping现在没有了  //json_object_object_get_ex函数的作用
		string actioncharge=json_object_get_string ( json_object_object_get(jobj,"iface") );	//从json中按名字取一个对象。从jobj中获取键值为action.mapping的子对象//jobj是什么类型 mapping就是什么类型
		if(actioncharge=="action.goCharging")
		{
		    if(json_object_object_get_ex(jobj,"body",NULL))
		    {
			json_object *chargebody=json_object_object_get (jobj,"body" );  
			if(json_object_object_get_ex(chargebody,"switch",NULL))
			{
			    string chargeswitch=json_object_get_string(json_object_object_get (chargebody,"switch") );
			    if(chargeswitch=="START" ){chargeflag=1;}            //
			    if(chargeswitch=="STOP" ){chargeflag=2;}
			}
			
		    }
		//backflag=1;
		}
	    }
	    
	    //人体检测的开关
	    switch(detectionflag)
	    {
		case 1:
		    cout<<"people detection start"<<endl;
		    CreatePID("roslaunch cob_openni2_tracker body_tracker_nodelet.launch standalone_without_camera_driver:=true") ;
		    
		    sleep(3);
		    
		    detectionflag=0;
		    break;
		case 2:
		    cout<<"people detection stop"<<endl;
		    detection_num=0;
		    cout<<"detection_num"<<detection_num<<endl;
 		    CreatePID("rosnode kill /body_tracker /camera/camera_nodelet_manager /rosout");
		    detectionflag=0;
		    detectionbackflag=0;
		    break;
		default:
		    break;
	    }
	    cout<<"检测1"<<endl;
	    //人体跟踪的开关
	    switch(followflag)
	    {
		case 1:
		    cout<<"people follow start"<<endl;
		    CreatePID("roslaunch turtlebot_follower follower.launch");
		    followflag=0;
		    break;
		case 2:
		    cout<<"people follow stop"<<endl;
		    CreatePID("rosnode kill /turtlebot_follower /camera/camera_nodelet_manager /camera/depth_metric /camera/depth_metric_rect /switch");	
		    CreatePID("rosnode kill /camera/depth_points /camera/depth_rectify_depth /camera/driver /camera/rectify_color /camera/rectify_ir /camera_throttle /follower_velocity_smoother ");//杀死跟踪的节点
		    followflag=0;
		    break;    
		default:
		    break;
	    }
	    //查找充电站的判断
	    switch(chargeflag)
	    {
		case 1:
		    cout<<"go charge start"<<endl;
		    //CreatePID("roslaunch turtlebot_follower follower.launch");
		    chargeflag=0;
		    break;
		case 2:
		    cout<<"go charge stop"<<endl;
		    //CreatePID("rosnode kill  ");															//杀死跟踪的节点
		    chargeflag=0;
		    break;    
		default:
		    break;
	    }
	    
	    cout<<"检测2"<<endl;
	    /*人体检测结果回掉  将检测的信息发送给client*/
// 	    if(detectionbackflag==1)
// 	    {
// 		    cout<<"检测3"<<endl;
// 		if(detection_num>0)             //这个要返回检测到的人体的数量  待修改
// 		{
// 		    cout<<"检测到人体"<<endl;
// 		    json_object *client_id=json_object_new_string(id.c_str());
// 		    json_object_object_add(detection_back_found,"id",client_id);
// 		    //json_object_object_add(detection_back_found_body,"num","1");
// 		  //  json_object_object_add(detection_back_found,"body",detection_back_found_body);
// 		    if ( strcpy ( callbuff, json_object_to_json_string ( detection_back_found ) ) == NULL )
// 		    {
// 			perror ( "strcpy" );
// 			exit(1);
// 		    }
// 
// 		    
// 		    if ( send (  client_fd, callbuff, strlen ( callbuff ) ,0) == -1 )
// 		    {
// 			perror ( "write" );
// 			exit(1);
// 		    }
// 		    cout<<json_object_to_json_string (detection_back_found )<<endl; 
// 	        }
// 	        else          //这个要返回检测到的人体的数量  待修改
// 	        {
// 		    cout<<"未检测到人体"<<endl;
// 		   
// 		    json_object *client_id=json_object_new_string(id.c_str());
// 		    json_object_object_add(detection_back_lost,"id",client_id);
// 		    if ( strcpy ( callbuff, json_object_to_json_string ( detection_back_lost ) ) == NULL )
// 		    {
// 			perror ( "strcpy" );
// 			exit(1);
// 		    }
// 
// 		    
// 		    if ( send (  client_fd, callbuff, strlen ( callbuff ) ,0) == -1 )
// 		    {
// 			perror ( "write" );
// 			exit(1);
// 		    }
// 		    
// 		    cout<<json_object_to_json_string (detection_back_lost )<<endl; 
// 	        }
//	    }
	    

 	}
	memset(callbuff,0,1024);
	memset(buff,0,1024);
   
    }

    
}


void* call_back(void* arg)
{

    
    char num_str[1024];
    int client_fd = (long)arg; 
    cout<<"子线程的client_fd"<<client_fd<<endl;
    ros::NodeHandle nh_;
  
    ros::Rate r(1);
    ros::Subscriber people_pub_ = nh_.subscribe("/body_tracker/people_num",50 , detection_callback);  
    

    while(1)
    {
	json_object *detection_back_found = json_tokener_parse("{\"id\":\"1\",\"iface\":\"detection.result\",\"body\":{\"reason\":\"FOUNG\",\"num\":\"1\"}}");//检测人体回调给客户端的json对象
	json_object *detection_back_found_body = json_tokener_parse("{\"reason\":\"FOUNG\",\"num\":\"1\"}");//检测人体回调给客户端的json对象
	json_object *detection_back_lost = json_tokener_parse("{\"id\":\"1\",\"iface\":\"detection.result\",\"body\":{\"reason\":\"LOST\"}}");//检测人体回调给客户端的json对象
	cout<<"回调线程正在运行"<<endl;
	if(closeflag==1)
	{
	    cout<<"客户端断开链接"<<endl;
	    changeflag=0;
	    detection_num=0;
	    closeflag=0;
	    break;
	}
	if(detectionbackflag==1)
	{
	       cout<<"检测3"<<endl;
		if(detection_num>=0)             //这个要返回检测到的人体的数量  待修改
		{
		     if(changeflag==detection_num)
		    {
			cout<<"人体数量未发生变化"<<endl;
		    }
		    if(changeflag<detection_num)
		    {
			sprintf(num_str,"%d",detection_num);
			cout<<"检测到人体"<<endl;
			json_object *client_id=json_object_new_string(id.c_str());
			json_object_object_add(detection_back_found,"id",client_id);
			json_object *peo_num=json_object_new_string(num_str);
			json_object_object_add(detection_back_found_body,"num",peo_num);
			json_object_object_add(detection_back_found,"body",detection_back_found_body);
			if ( strcpy ( callbuff, json_object_to_json_string ( detection_back_found ) ) == NULL )
			{
			    perror ( "strcpy" );
			    exit(1);
			}

		    
			if ( send (  client_fd, callbuff, strlen ( callbuff ) ,0) == -1 )
			{
			    perror ( "write" );
			    exit(1);
			}
			cout<<json_object_to_json_string (detection_back_found )<<endl; 
			changeflag=detection_num;
		    }
		    if(changeflag>detection_num)
		    {
			cout<<"人体丢失"<<endl;
		   
			json_object *client_id=json_object_new_string(id.c_str());
			json_object_object_add(detection_back_lost,"id",client_id);
			if ( strcpy ( callbuff, json_object_to_json_string ( detection_back_lost ) ) == NULL )
			{
			    perror ( "strcpy" );
			    exit(1);
			}

		    
			if ( send (  client_fd, callbuff, strlen ( callbuff ) ,0) == -1 )
			{
			    perror ( "write" );
			    exit(1);
			}
			changeflag=detection_num;
			cout<<json_object_to_json_string (detection_back_lost )<<endl; 
		    }
		   
		    
	        }
	      
	    }
	    sleep(1);
	    memset(callbuff,0,1024);
	    memset(buff,0,1024);
	    ros::spinOnce();
            r.sleep();
	}
	
		 
	    
	    
  
    
}


int CreatePID(string comname)     //创建子进程函数
{
     string command;
     command=comname;

     if( fork() == 0)//创建子进程
      {
	
	if(execl("/bin/sh","sh","-c",const_cast<char*>(command.c_str()),(char *)0)==-1)
// 	if(execl("/usr/bin/gnome-terminal","gnome-terminal","-x",const_cast<char*>(command.c_str()),(char *)0) == -1)
	{
	  perror("fork error");
	  return -1;
	}
      }
      return 0;
}



