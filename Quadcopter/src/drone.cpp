#include<iostream>
#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/Wrench.h"
#include "geometry_msgs/PoseStamped.h"
#include "sensor_msgs/Imu.h"
#include "ros/rate.h"
#include<math.h>

class Drone{
   public: 
    Drone(int argc,char** argv){
        ros::init(argc,argv,"drone") ;

        ros::NodeHandle n ;
        force_pub = n.advertise<geometry_msgs::Wrench>("/quad_rotor/cmd_force",100) ;
        imu_sub = n.subscribe("/quad_rotor/imu",100,&Drone::updateImu,this) ;
        pose_sub = n.subscribe("/quad_rotor/pose",100,&Drone::updatePose,this) ;
    }
    void updateImu(const sensor_msgs::Imu::ConstPtr& msg){
        this->imu = *msg ;
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
           msg.force.x=0 ;
           msg.force.y =0 ;
           // PD control
           msg.force.z = 19.6 + (z - pose.pose.position.z) + 2.6*((z - pose.pose.position.z) - last_pose)*10;
           last_pose = z-pose.pose.position.z;
           e+=last_pose ;
           msg.torque.x =0 ;
           msg.torque.y = 0 ;
           msg.torque.z =0 ;

           force_pub.publish(msg) ;
           ros::spinOnce() ;
           loop_rate.sleep() ;
           std::cout<<pose.pose.position.z<<std::endl ;
           ROS_INFO("current : %f\n",pose.pose.position.z) ;
} }
    ros::Publisher force_pub ;
    ros::Subscriber imu_sub ;
    ros::Subscriber pose_sub ;
   geometry_msgs::PoseStamped pose ;
    sensor_msgs::Imu imu ;};


int main(int argc,char** argv){
freopen("/home/abhyuday/diff.txt","w",stdout);
Drone drone(argc,argv) ;
float z ;
std::cout<<"Altitude : " ;
std::cin>>z ;

drone.altitude_control(z) ;
return 0 ;
}
