cmake_minimum_required(VERSION 3.0.2)
project(odometry_node)

find_package(catkin REQUIRED COMPONENTS
  rospy
  std_msgs
)

catkin_python_setup()


catkin_package()

include_directories(
  ${catkin_INCLUDE_DIRS}
)

catkin_install_python(PROGRAMS
   scripts/odometry_jetbot.py
   DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
