#include "slam2d.h"

#include <cmath>
#include <nav_msgs/Odometry.h>
#include <nav_msgs/Path.h>
#include <geometry_msgs/PoseStamped.h>

#include <ros/ros.h>
#include <sensor_msgs/MultiEchoLaserScan.h>
#include <sensor_msgs/LaserScan.h>
#include <Eigen/Eigen>

slam2d slam;
ros::Publisher pub_pose, pub_path;
void publish_pose(slam2d &slam);

void multiecho_laserscan_callback(const sensor_msgs::MultiEchoLaserScanConstPtr &msg)
{
    slam.update(msg);
    publish_pose(slam);
}

void publish_pose(slam2d &slam)
{
    static nav_msgs::Path path;
    geometry_msgs::PoseStamped pose;
    pose.header.stamp = ros::Time(slam.timestamp);
    pose.header.frame_id = "odom";
    double theta = slam.state.theta;
    pose.pose.orientation.w = cos(0.5 * theta);
    pose.pose.orientation.x = 0;
    pose.pose.orientation.y = 0;
    pose.pose.orientation.z = sin(0.5 * theta);
    pose.pose.position.x = slam.state.t(0);
    pose.pose.position.y = slam.state.t(1);
    pose.pose.position.z = 0;
    pub_pose.publish(pose);

    //cout << "publish pose: " << endl;
    //_pose.print_state();
    path.header.frame_id = "odom";
    path.poses.push_back(pose);
    pub_path.publish(path);
}

void laserscan_callback(const sensor_msgs::LaserScanConstPtr &msg)
{
    slam.update(msg);
    publish_pose(slam);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "slam2d");
    ros::NodeHandle nh;

    ros::Subscriber sub_multiecho_laserscan = nh.subscribe<sensor_msgs::MultiEchoLaserScan>("/multiecho_scan", 100, multiecho_laserscan_callback);
    ros::Subscriber sub_laserscan = nh.subscribe<sensor_msgs::LaserScan>("/scan", 100, laserscan_callback);

    pub_pose = nh.advertise<geometry_msgs::PoseStamped>("/est_pose", 10);
    pub_path = nh.advertise<nav_msgs::Path>("/path", 10);

    ros::spin();
    return 0;
}