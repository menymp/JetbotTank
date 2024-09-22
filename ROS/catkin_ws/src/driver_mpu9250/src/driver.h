/// \file driver.h
/// \brief Defines the MPU9250 driver class.
#ifndef MPU9250_DRIVER_H
#define MPU9250_DRIVER_H

#include <functional>
#include <string> // for string class 

/// \brief The base driver class for the MPU9250.
class driver
{
public:
    // ENUMERATIONS
    /// \brief Enumerates the digital low-pass filter (DLPF) cutoff frequencies for the accelerometers.
    enum class accel_dlpf_frequency_type
    {
        F_218HZ = 0x00,
        F_99HZ = 0x02,
        F_44HZ = 0x03,
        F_21HZ = 0x04,
        F_10HZ = 0x05,
        F_5HZ = 0x06
    };
    /// \brief Enumerates the digital low-pass filter (DLPF) cutoff frequencies for the gyros.
    enum class gyro_dlpf_frequency_type
    {
        F_250HZ = 0x00,
        F_184HZ = 0x01,
        F_92HZ = 0x02,
        F_41HZ = 0x03,
        F_20Hz = 0x04,
        F_10Hz = 0x05,
        F_5HZ = 0x06
    };
    /// \brief Enumerates the full scale ranges (FSR) available for the accelerometers in g.
    enum class accel_fsr_type
    {
        G_2 = 0x00,
        G_4 = 0x01,
        G_8 = 0x02,
        G_16 = 0x03
    };
    /// \brief Enumerates the full scale ranges (FSR) available for the gyros in degress/second.
    enum class gyro_fsr_type
    {
        DPS_250 = 0x00,
        DPS_500 = 0x01,
        DPS_1000 = 0x02,
        DPS_2000 = 0x03
    };

    // CLASSES
    /// \brief A structure for storing IMU data.
    struct data
    {
        float accel_x, accel_y, accel_z;
        float gyro_x, gyro_y, gyro_z;
        float magneto_x, magneto_y, magneto_z;
        //float temp;
    };

    // CONSTRUCTORS
    /// \brief Initializes a new driver instance.
    driver();
    virtual ~driver() = 0;

    // CONFIGURATION
    /// \brief Attaches a callback to handle data when it becomes available.
    /// \param callback The callback function to execute.
    void set_data_callback(std::function<void(driver::data)> callback);

    // INITIALIZATION
    /// \brief Initializes the MPU9250.
    /// \param port_path serial port path for the driver.
    /// \param baud_rate baud rate for the driver communication.
    /// \param timeout timeout for the read command.
    void initialize(std::string port_path, unsigned int baud_rate, unsigned int timeout);
    /// \brief Deinitializes the MPU9250.
    void deinitialize();

    // PROPERTIES

protected:

    // METHODS
    ///
    /// \brief open_serial opens the communication with the serial microcontroller for mpu9250 read.
    /// \param port_path path for the serial port.
    /// \param baud_rate baud rate.
    /// \param timeout port read line timeout.
    /// \return A handle to the I2C interface.
    ///
    virtual void open_serial(std::string port_path, unsigned int baud_rate, unsigned int timeout) = 0;

    ///
    /// \brief read_data attempts to read mpu9250 data and returns the parsed result.
    ///
    virtual data read_data() = 0;

    ///
    /// \brief close_serial closes the serial connection for the device communication.
    ///
    virtual void close_serial() = 0;

private:
    // FULL SCALE RANGE
    /// \brief m_gyro_fsr Stores the full scale range of the gyroscopes for ADC conversion.
    float m_gyro_fsr;
    /// \brief m_accel_fsr Stores the full scale range of the accelerometers for ADC conversion.
    float m_accel_fsr;

    // CALLBACKS
    /// \brief m_data_callback The callback function to execute when IMU data is read.
    std::function<void(driver::data)> m_data_callback;

};

#endif // DRIVER_H
