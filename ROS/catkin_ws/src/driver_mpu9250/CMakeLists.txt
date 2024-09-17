cmake_minimum_required(VERSION 2.8.3)
project(driver_mpu9250)

# Find catkin and package dependencies
find_package(catkin REQUIRED COMPONENTS
  roscpp
  serial
  sensor_msgs_ext
)

# Define catkin package.
catkin_package(
  CATKIN_DEPENDS
    serial
    std_msgs
)

# Set include directories.
include_directories(
  ${catkin_INCLUDE_DIRS}
)


## Declare a C++ executable
## With catkin_make all packages are built within a single CMake context
## The recommended prefix ensures that target names across packages don't collide
add_executable(${PROJECT_NAME} src/main_rpi.cpp src/ros_node.cpp src/calibration.cpp src/rpi_driver.cpp src/driver.cpp src/serial_driver.cpp)

## Rename C++ executable without prefix
## The above recommended prefix causes long target names, the following renames the
## target back to the shorter version for ease of user use
## e.g. "rosrun someones_pkg node" instead of "rosrun someones_pkg someones_pkg_node"
# set_target_properties(${PROJECT_NAME}_rpi_node PROPERTIES OUTPUT_NAME driver_mpu9250_rpi PREFIX "")

## Add cmake target dependencies of the executable
## same as for the library above
# add_dependencies(${PROJECT_NAME}_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
## Specify libraries to link a library or executable target against
target_link_libraries(${PROJECT_NAME}_rpi_node
   ${catkin_LIBRARIES}
)

# Set up installation for RPi driver
#install(TARGETS ${PROJECT_NAME}_rpi_node
#    RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})
