#include "driver.h"
#include <serial/serial.h>
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>
#include <string> // for string class 

/// \file serial_driver.h
/// \brief Defines the serial_driver class.
#ifndef SERIAL_DRIVER_H
#define SERIAL_DRIVER_H

///
/// \brief An MPU9250 driver for an specific serial driver.
///
class serial_driver : public driver
{
public:
    // CONSTRUCTORS
    ///
    /// \brief serial_driver Initializes a new serial driver.
    ///
    serial_driver();
    ~serial_driver() override;

    // METHODS
    ///
    /// \brief open_serial opens the communication with the serial microcontroller for mpu9250 read.
    /// \param port_path path for the serial port.
    /// \param baud_rate baud rate.
    /// \param timeout port read line timeout.
    /// \return A handle to the I2C interface.
    ///
    void open_serial(std::string port_path, unsigned int baud_rate, unsigned int timeout);

    ///
    /// \brief read_data attempts to read mpu9250 data and returns the parsed result.
    ///
    driver::data read_data();

    ///
    /// \brief close_serial closes the serial connection for the device communication.
    ///
    void close_serial();
private:

    // METHODS
    serial::Serial serialObject;
    std_msgs::String result;

    ///
    /// \brief serial_read creates a request read for the serial microcontroller and returns the result.
    /// \return the read line with the possible the data.
    ///
    std::string serial_read();

};

#endif // SERIAL_DRIVER_H
