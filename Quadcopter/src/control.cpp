#include<iostream>
#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/Wrench.h"
#include "geometry_msgs/PoseStamped.h"
#include "sensor_msgs/Imu.h"
#include "ros/rate.h"
#include<math.h>
#include "tf/tf.h"

class Drone{
   public: 
    Drone(int argc,char** argv){
        ros::init(argc,argv,"drone") ;

        ros::NodeHandle n ;

        force_pub = n.advertise<geometry_msgs::Wrench>("/quad_rotor/cmd_force",100) ;
        vel_pub  = n.advertise<geometry_msgs::Twist>("/quad_rotor/cmd_vel",100) ;
        imu_sub = n.subscribe("/quad_rotor/imu",100,&Drone::updateImu,this) ;
        pose_sub = n.subscribe("/quad_rotor/pose",100,&Drone::updatePose,this) ;

 }

    void updateImu(const sensor_msgs::Imu::ConstPtr& msg){
        this->imu = *msg ;
        tf::Quaternion q ;
        tf::quaternionMsgToTF(imu.orientation,q) ;
        tf::Matrix3x3 v(q) ;
        v.getRPY(r,p,y) ;
    }

    void updatePose(const geometry_msgs::PoseStamped::ConstPtr& msg){
        this->pose = *msg ;
    }

    void altitude_control(float z){
     
       auto  msg = geometry_msgs::Wrench() ;
       
       ros::Rate loop_rate(10) ;
       float last_pose  = 0.0 ;
       float e =0 ;
       while(true && ros::ok()){
           msg.force.x=5 ;
           msg.force.y =0 ;
           // PD control
           msg.force.z = 19.6 + 0.9*(z - pose.pose.position.z) + 3.9*((z - pose.pose.position.z) - last_pose)*10; 
           last_pose = z-pose.pose.position.z;
          
           
           e = (30 - p);
           msg.torque.y = 0 ;
           msg.torque.z =0.5 ;

           force_pub.publish(msg) ;

          
          // vel_pub.publish(vel) ;
            

           ros::spinOnce() ;
           loop_rate.sleep() ;
           std::cout<<pose.pose.position.z<<std::endl ;
       // ROS_INFO("current : %f\n",pose.pose.position.z) ;


       }
    


    }
   void move(float height,float radius){

             }

    ros::Publisher force_pub ;
    ros::Subscriber imu_sub ;
    ros::Subscriber pose_sub ;
    ros::Publisher vel_pub ;
    geometry_msgs::PoseStamped pose ;
    sensor_msgs::Imu imu ;
    double r,p,y ;



} ;


int main(int argc,char** argv){
freopen("/home/abhyuday/diff.txt","w",stdout);
Drone drone(argc,argv) ;
float z ;
std::cout<<"Altitude : " ;
std::cin>>z ;

drone.altitude_control(z) ;
return 0 ;
}
