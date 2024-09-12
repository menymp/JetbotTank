#include <serial/serial.h>
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>
/// \file serial_driver.h
/// \brief Defines the serial_driver class.
#ifndef SERIAL_DRIVER_H
#define SERIAL_DRIVER_H

///
/// \brief An MPU9250 driver for an specific serial driver.
///
class serial_driver
{
public:
    // CONSTRUCTORS
    ///
    /// \brief rpi_driver Initializes a new Raspberry Pi driver.
    ///
    serial_driver();
    ~serial_driver() override;

    // METHODS
    void initialize_serial(unsigned int i2c_bus, unsigned int i2c_address, unsigned int interrupt_gpio_pin) override;
    void deinitialize_serial() override;

private:

    // METHODS
    serial::Serial serialObject;
    std_msgs::String result;

    ///
    /// \brief open_i2c Opens an I2C channel with a specific I2C slave device.
    /// \param i2c_bus The I2C bus number to communicate over.
    /// \param i2c_address The I2C address of the slave device.
    /// \return A handle to the I2C interface.
    ///
    int open_serial(unsigned int i2c_bus, unsigned int i2c_address);
    ///
    /// \brief read_register Reads a byte of data from a register on an I2C device.
    /// \param i2c_handle The I2C interface handle to read from.
    /// \param address The register address to write to.
    /// \return The value read from the register.
    ///
    unsigned char read_data(unsigned int i2c_handle, unsigned int address);
    ///
    /// \brief close_i2c Closes an I2C channel for a specific I2C device.
    /// \param i2c_handle The I2C handle to close.
    ///
    void close_serial(int i2c_handle);
};

#endif // SERIAL_DRIVER_H
