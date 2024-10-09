#include "driver.h"
#include <stdexcept>
#include <sstream>
#include <unistd.h>
#include <cmath>
#include <limits>
#include <string> // for string class 

/* 
TODO: MENY Reimplement this driver using the available firmware for arduino pro micro
Now instead of I2C communictaiont, the implementation should use the serial port
 */

// CONSTRUCTORS
driver::driver()
{

}
driver::~driver()
{

}


//void driver::set_data_callback(std::function<void (data)> callback)
//{
//    // Store the callback.
//    driver::m_data_callback = callback;
//}

// INITIALIZATION
void driver::initialize(std::string port_path, unsigned int baud_rate, unsigned int timeout)
{
    // Initialize I2C:
    open_serial(port_path, baud_rate, timeout);

    // Test MPU9250 communications.
    try
    {
        //if(read_mpu9250_register(register_mpu9250_type::WHO_AM_I) != 0x71)
        //{
        //    throw std::runtime_error("MPU9250 device ID mismatch.");
        //}
    }
    catch (std::exception& e)
    {
        //std::stringstream message;
        //message << "MPU9250 communications failure: " << e.what();
        //throw std::runtime_error(message.str());
    }

    // Sleep for 50ms to let sensors come online.
    usleep(50000);

    // Follow default FSR values.
    driver::m_gyro_fsr = 250.0f;
    driver::m_accel_fsr = 2.0f;

}
void driver::deinitialize()
{
    // Deinit I2C.
    close_serial();
}

// PROPERTIES