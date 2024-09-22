#include "serial_driver.h"
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>
#include <string> // for string class 


int serial_driver::open_serial(std::string port_path, unsigned int baud_rate, unsigned int timeout)
{
    serialObject.setPort(port_path);
    serialObject.setBaudrate(baud_rate);
    serial::Timeout to = serial::Timeout::simpleTimeout(timeout);
    serialObject.setTimeout(to);
    serialObject.open();
}
/* sends a read comands and wait for the result */
std::string serial_driver::serial_read()
{
    std::string received_data;
    serialObject.write('R');
    auto read_cnt = serialObject.readline(received_data, 400, "\n");
    return received_data;
}

serial_driver::serial_driver()
{

}

serial_driver::~serial_driver()
{

}

// METHODS
void serial_driver::read_data()
{
    // Create data storage structure.
    driver::data data;
    
    try
    {
        auto buffer = serial_read();
    }
    catch(const std::exception& e)
    {
        return;
    }
    auto mpu9250_fields = buffer.data.split(',');

    if (mpu9250_fields[0] != "a/g/m:")
    {
        /* no recognized format */
        return;
    }

    /* attempts to parse the received data */
    try
    {
        // Parse out accel data.
        data.accel_x = std::stof(mpu9250_fields[1]);
        data.accel_y = std::stof(mpu9250_fields[2]);
        data.accel_z = std::stof(mpu9250_fields[3]);

        // Parse out gyro data.
        data.gyro_x = std::stof(mpu9250_fields[4]);
        data.gyro_y = std::stof(mpu9250_fields[5]);
        data.gyro_z = std::stof(mpu9250_fields[6]);

        // Store measurements.
        data.magneto_x = std::stof(mpu9250_fields[7]);
        data.magneto_y = std::stof(mpu9250_fields[8]);
        data.magneto_z = std::stof(mpu9250_fields[9]);
    }
    catch(const std::exception& e)
    {
        return;
    }

    // Initiate the data callback.
    driver::m_data_callback(data);
}
void serial_driver::close_serial()
{
    serialObject.close();
}
