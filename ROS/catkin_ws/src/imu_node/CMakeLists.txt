cmake_minimum_required(VERSION 2.8.3)
project(imu_node)

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
add_executable(${PROJECT_NAME} src/imu_publisher.cpp)

## Add cmake target dependencies of the executable
## same as for the library above
# add_dependencies(${PROJECT_NAME}_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
## Specify libraries to link a library or executable target against
target_link_libraries(${PROJECT_NAME}
   ${catkin_LIBRARIES}
)
