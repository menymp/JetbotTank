#include "serial_driver.h"


int serial_driver::open_serial(unsigned int i2c_bus, unsigned int i2c_address)
{
    serialObject.setPort("/dev/ttyACM0");
    serialObject.setBaudrate(9600);
    serial::Timeout to = serial::Timeout::simpleTimeout(1000);
    serialObject.setTimeout(to);
    serialObject.open();
}

void serial_driver::serial_read()
{
    return serialObject.readline(result);
}

// METHODS
void serial_driver::read_data()
{
    // Create data storage structure.
    driver::data data;
    
    
    // Parse out accel data.
    data.accel_x = driver::m_accel_fsr * static_cast<float>(static_cast<short>(be16toh(*reinterpret_cast<unsigned short*>(&atg_buffer[0])))) / 32768.0f;
    data.accel_y = driver::m_accel_fsr * static_cast<float>(static_cast<short>(be16toh(*reinterpret_cast<unsigned short*>(&atg_buffer[2])))) / 32768.0f;
    data.accel_z = driver::m_accel_fsr * static_cast<float>(static_cast<short>(be16toh(*reinterpret_cast<unsigned short*>(&atg_buffer[4])))) / 32768.0f;

    // Parse out gyro data.
    data.gyro_x = driver::m_gyro_fsr * static_cast<float>(static_cast<short>(be16toh(*reinterpret_cast<unsigned short*>(&atg_buffer[8])))) / 32768.0f;
    data.gyro_y = driver::m_gyro_fsr * static_cast<float>(static_cast<short>(be16toh(*reinterpret_cast<unsigned short*>(&atg_buffer[10])))) / 32768.0f;
    data.gyro_z = driver::m_gyro_fsr * static_cast<float>(static_cast<short>(be16toh(*reinterpret_cast<unsigned short*>(&atg_buffer[12])))) / 32768.0f;

    // Store measurements.
    data.magneto_x = 4900.0f * static_cast<float>(static_cast<short>(le16toh(*reinterpret_cast<unsigned short*>(&magneto_buffer[0])))) / resolution;
    data.magneto_y = 4900.0f * static_cast<float>(static_cast<short>(le16toh(*reinterpret_cast<unsigned short*>(&magneto_buffer[2])))) / resolution;
    data.magneto_z = 4900.0f * static_cast<float>(static_cast<short>(le16toh(*reinterpret_cast<unsigned short*>(&magneto_buffer[4])))) / resolution;

    // Read interrupt status register to clear interrupt.
    try
    {
        read_mpu9250_register(driver::register_mpu9250_type::INT_STATUS);
    }
    catch(const std::exception& e)
    {
        // Quit before callback. Do not report error in loop.
        return;
    }

    // Initiate the data callback.
    driver::m_data_callback(data);
}
void serial_driver::close_serial(int i2c_handle)
{
    int result = i2c_close(rpi_driver::m_pigpio_handle, static_cast<unsigned int>(i2c_handle));
    if(result < 0)
    {
        switch(result)
        {
        case PI_BAD_HANDLE:
        {
            std::stringstream message;
            message << "deinitialize: Specified invalid I2C handle: " << i2c_handle;
            throw std::runtime_error(message.str());
        }
        default:
        {
            std::stringstream message;
            message << "deinitialize: Unknown error: " << result;
            throw std::runtime_error(message.str());
        }
        }
    }
}
