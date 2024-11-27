/*
This simple node handles the serial communication and conversion of data from
an IMU9250 coupled to the system with an arduino board

menymp
*/
#include "ros/ros.h"
#include "std_msgs/String.h"
#include <std_msgs/Empty.h>
#include <string> // for string class
#include <sstream>
#include <sensor_msgs_ext/accelerometer.h>
#include <sensor_msgs_ext/gyroscope.h>
#include <sensor_msgs_ext/magnetometer.h>
#include <sensor_msgs_ext/temperature.h>


/// \brief A structure for storing IMU data.
struct IMU_DATA
{
  float accel_x, accel_y, accel_z;
  float gyro_x, gyro_y, gyro_z;
  float magneto_x, magneto_y, magneto_z;
  //float temp;
};

serial::Serial serialObject;
ros::Publisher m_publisher_accelerometer;
ros::Publisher m_publisher_gyroscope;
ros::Publisher m_publisher_magnetometer;
ros::Publisher m_publisher_temperature;

std::vector<std::string> split(std::string str, char delim);
void data_publish(IMU_DATA data);
void open_serial(std::string port_path, unsigned int baud_rate, unsigned int timeout);
std::string serial_read();
IMU_DATA read_data();
void close_serial();


int main(int argc, char **argv)
{
  IMU_DATA data;
  ros::init(argc, argv, "imu_9250_publisher");

  ros::NodeHandle n;

  // Set up data publishers.
  ros::Publisher m_publisher_accelerometer = n.advertise<sensor_msgs_ext::accelerometer>("imu_9250/accelerometer", 1);
  ros::Publisher m_publisher_gyroscope = n.advertise<sensor_msgs_ext::gyroscope>("imu_9250/gyroscope", 1);
  ros::Publisher m_publisher_magnetometer = n.advertise<sensor_msgs_ext::magnetometer>("imu_9250/magnetometer", 1);
  ros::Publisher m_publisher_temperature = n.advertise<sensor_msgs_ext::temperature>("imu_9250/temperature", 1);

  ros::Rate loop_rate(10);

  open_serial("/dev/ttyACM0", 9600, 500);

  while (ros::ok())
  {
    std_msgs::String msg;
    data = read_data();
    data_publish(data);
    // ROS_INFO("%s", msg.data.c_str());

    ros::spinOnce();

    loop_rate.sleep();
  }
  close_serial();

  return 0;
}

// CALLBACKS
void data_publish(IMU_DATA data)
{
    // Create accelerometer message.
    sensor_msgs_ext::accelerometer message_accel;
    // Set accelerations (convert from g's to m/s^2)
    message_accel.x = static_cast<double>(data.accel_x) * 9.80665;
    message_accel.y = static_cast<double>(data.accel_y) * 9.80665;
    message_accel.z = static_cast<double>(data.accel_z) * 9.80665;
    // Apply calibration. TODO Meny
    // m_calibration_accelerometer.calibrate(message_accel.x, message_accel.y, message_accel.z);
    // Publish message.
    m_publisher_accelerometer.publish(message_accel);

    // Create gyroscope message.
    sensor_msgs_ext::gyroscope message_gyro;
    // Set rotation rates (convert from deg/sec to rad/sec)
    message_gyro.x = static_cast<double>(data.gyro_x) * M_PI / 180.0;
    message_gyro.y = static_cast<double>(data.gyro_y) * M_PI / 180.0;
    message_gyro.z = static_cast<double>(data.gyro_z) * M_PI / 180.0;
    // If gyroscope calibration is running, add uncalibrate data to window. TODO Meny
    // if(f_gyroscope_calibrating)
    // {
    //    m_gyroscope_calibration_window.push_back({message_gyro.x, message_gyro.y, message_gyro.z});
    // }
    // Apply calibration. TODO Meny
    // m_calibration_gyroscope.calibrate(message_gyro.x, message_gyro.y, message_gyro.z);
    // Publish message.
    m_publisher_gyroscope.publish(message_gyro);

    // Check if there was a magneto overflow.
    if(std::isnan(data.magneto_x) == false)
    {
        // Create magneto message.
        sensor_msgs_ext::magnetometer message_mag;
        // Fill magnetic field strengths (convert from uT to T)
        message_mag.x = static_cast<double>(data.magneto_x) * 0.000001;
        message_mag.y = static_cast<double>(data.magneto_y) * 0.000001;
        message_mag.z = static_cast<double>(data.magneto_z) * 0.000001;
        // Apply calibration. TODO Meny
        // m_calibration_magnetometer.calibrate(message_mag.x, message_mag.y, message_mag.z);
        // Publish message.
        m_publisher_magnetometer.publish(message_mag);
    }

    // Create temperature message.
    sensor_msgs_ext::temperature message_temp;
    //message_temp.temperature = static_cast<double>(data.temp);
    message_temp.temperature = static_cast<double>(27.0); // ToDo: implement a way to retrive temp from imu arduino firmware
    // Publish temperature message.
    m_publisher_temperature.publish(message_temp);
}

void open_serial(std::string port_path, unsigned int baud_rate, unsigned int timeout)
{
    serialObject.setPort(port_path);
    serialObject.setBaudrate(baud_rate);
    serial::Timeout to = serial::Timeout::simpleTimeout(timeout);
    serialObject.setTimeout(to);
    serialObject.open();
}

/* sends a read comands and wait for the result */
std::string serial_read()
{
    std::string received_data;
    serialObject.write("R");
    auto read_cnt = serialObject.readline(received_data, 400, "\n");
    return received_data;
}

// METHODS
IMU_DATA read_data()
{
    // Create data storage structure.
    IMU_DATA data;
    std::string buffer;
    
    try
    {
        buffer = serial_read();
    }
    catch(const std::exception& e)
    {
        return data;
    }
    auto mpu9250_fields = split(buffer, ',');

    if (mpu9250_fields[0] != "a/g/m:")
    {
        /* no recognized format */
        return data;
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
        return data;
    }

    // Initiate the data callback.
    // driver::m_data_callback(data);
    return data;
}

void close_serial()
{
    serialObject.close();
}

/* Utility split string function */
std::vector<std::string> split(std::string str, char delim)
{
  std::vector<std::string> result; 
  auto left = str.begin(); 
  for (auto it = left; it != str.end(); ++it) 
  { 
    if (*it == delim) 
    { 
      result.emplace_back(&*left, it - left); 
      left = it + 1; 
    } 
  } 
  if (left != str.end()) 
    result.emplace_back(&*left, str.end() - left); 
  return result; 
}