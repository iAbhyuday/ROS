#include<iostream>
#include "ros/ros.h"
#include "geometry_msgs/Twist.h" 
#include "geometry_msgs/Pose2D.h"
#include "turtlesim/Pose.h"
#include "ros/rate.h"
#include<math.h>

class Turtle{

    public :
           Turtle(int argc,char** argv){
           // argc and argv are the remapping aguments
            ros::init(argc,argv,"mover") ;
            ros::NodeHandle n ;
            pub = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel",100) ;
            sub = n.subscribe("/turtle1/pose",100,&Turtle::Update,this) ;
            }

       void Update(const turtlesim::Pose::ConstPtr& msg){
            
             //   ROS_INFO("Pose recieved : x = %f y = %f\n",msg->x,msg->y) ;
                this->pose = *msg ;
            }
        float distance(turtlesim::Pose goalPose){
        
            return sqrtf(powf(goalPose.y - pose.y,2 ) + powf(goalPose.x - pose.x,2)) ;

        }

        float linear_velocity(turtlesim::Pose goalPose,float constant){
            return constant*distance(goalPose) ;
        }

        float steering_angle(turtlesim::Pose goalPose){
            return atan2f(goalPose.y - pose.y ,goalPose.x - pose.x) ;
        }
       
        float angular_velocity(turtlesim::Pose goalPose,float constant){
            return constant*(steering_angle(goalPose) - pose.theta) ;
        }

        void move2goal(){
            turtlesim::Pose goalPose= turtlesim::Pose() ;

            std::cout<<"Enter goal x : "<<" ";
            std::cin>>goalPose.x ;
            std::cout<<"Enter goal y : "<<" ";
            std::cin>>goalPose.y ;
        //    goalPose.theta =0;
            float d ;
            std::cout<<"Enter distance tolerance (d) : "<<" ";
            std::cin>>d ;

            auto vel_msg = geometry_msgs::Twist() ;
            ros::Rate loop_rate(10);
            while(distance(goalPose)>=d && ros::ok()){
                vel_msg.linear.x = linear_velocity(goalPose,1.5); 
                vel_msg.linear.y =0 ;
                vel_msg.linear.z = 0 ;
                // angular velocity 
                vel_msg.angular.x = 0 ;
                vel_msg.angular.y= 0 ;
                vel_msg.angular.z = angular_velocity(goalPose,4) ;

                pub.publish(vel_msg) ;
                ros::spinOnce();
                loop_rate.sleep() ;
             ROS_INFO("current : %f %f\n",pose.x,pose.y) ;

            }
            // relax the bot after reaching at the tolerance point
            vel_msg.angular.z=0 ;
            vel_msg.linear.x =0 ;
            pub.publish(vel_msg) ;
         }

private:
ros::Publisher pub ;
ros::Subscriber sub ;
turtlesim::Pose pose ;

} ;

int main(int argc,char** argv){
    auto turtle = Turtle(argc,argv) ;
    turtle.move2goal() ;
    return 0 ;
}