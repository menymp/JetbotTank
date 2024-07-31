:: windows startup BAT for ROS jetbot tank, this should be executed under ROS.exe shell
:: arguments
::  %1 --- ros jetbot ip
::
:: menymp 25 Jul 2024

ECHO ROS Jetbot Windows client startup script.
for /f "delims=[] tokens=2" %%a in ('ping -4 -n 1 %ComputerName% ^| findstr [') do set NetworkIP=%%a
ECHO Master ip is %1  Local ip is %NetworkIP%
:: set environment variables
set ROS_IP=%NetworkIP%
:: windows does not use double quotes
set ROS_MASTER_URI=http://%1:11311

rosparam set /use_sim_time false

:: start windows jetbot joystick remote control client
START /B python ./Jetbot_Joystick.py --host=%1 --port=9090 --videoPort=8990 --mode=tank --joystickMode=tank

:: start lidar service
START /B rviz -d ./mapping_demo.rviz