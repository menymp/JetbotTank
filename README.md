
# Jetbot Tank
## About
This project contains the software and relevant modules for a robot tank under development, the expected behavior is to implement the hardware sensor utils that are contained on the current hardware. The current plataform consist of a Jetson Nano development board as the main control core, For the hardware control, the system contains a microcontroller stm32f411 BlackPill as a motor controller and data adquisition. A XV11 Lidar module is included as a source of cloud points for 2d mapping. An Xbox 360 kinect allow RGB 3d reconstruction. A ublox-gps is equiped for the system. 


## Disclaimer
This project and sources are under development and in  no way are intended to be part of a functional system, use at your own risk.
A licence is provided under LICENSE.md 

## Current folders
### ControlClient
this directory contains the software for teleoperation of the robot
### JetbotServer
Contains the server software for the jetbot to allow a client for teleoperation
### Firmwares
Contains the firmware projets for the current modules
### ROS
Contains the needed modules configured for the current system

## ToDo list
- Documentation of the testing and overview for critical modules, this includes comments, electrical diagrams, High level block diagram etc.
- Testing with real environment.

## Future ideas and features
- to be writen