#!/bin/bash
#
# this script perform the jetbot control and mapping server startup for all services
# menymp 25 Jul 2024
#
echo "ROS Jetbot server startup script"
# source the ROS environment first
source ./catkin_ws/devel/setup.bash
# get local system ip
ipaddr=$(ifconfig wlan0 | grep -Eo 'inet (addr:)?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*' | grep -v '127.0.0.1')
#export environment variables
export ROS_IP="$ipaddr"
export ROS_MASTER_URI="http://$ipaddr:11311"

# start the ros core process
roscore &

# start ROS bridge services for windows communication
roslaunch rosbridge_server rosbridge_websocket.launch &
rosrun tf2_web_republisher tf2_web_republisher &

# start lidar service
roslaunch hector_slam_launch jetson_nano.launch &

# start jetbot remote control server
roslaunch remote_control remote_server.launch &
