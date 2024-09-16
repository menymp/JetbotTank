#include "ros_node.h"
#include "serial_driver.h"

int main(int argc, char **argv)
{
    // Create the driver.
    auto driver = std::make_shared<serial_driver>();

    // Create the node.
    ros_node node(driver, argc, argv);

    ros::Rate loop_rate(5);
    while(ros::ok()){
        ros::spinOnce();
        node.perform_read();
        loop_rate.sleep();
    }
   node.deinitialize_driver();
    // Run the node.
    //node.spin();
}
