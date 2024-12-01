'''
This file contains a test read script for the jetbot

the same as the remoteControlServer.py but without the joystick command elements

menymp 2024

History
2024 Dic 01 converted to test bench
'''
import sys
import signal
import time
from threading import Thread, Event
import math
# import tf # <---- not supported in melodig
from nav_msgs.msg import Odometry
from geometry_msgs.msg import Point, Pose, Quaternion, Twist, Vector3

#ROS packages
import rospy
from std_msgs.msg import String

from remote_control.serialControl import serialControl

serialObj = None

def serialOpen():
	serialObj = serialControl()
	serialObj.serialOpen('/dev/ttyACM1',9600,0.07,50)
	return serialObj

def serialHandler(command):
	cmd = command.data
	print(cmd)
	serialObj.write(accelerationRamp(cmd))
	pass

INCREMENT = 3
#Use the update cmd as increment delay interval

M1_POWER = 0
M2_POWER = 0

def accelerationRamp(newSetpointCmd):
	tokens = (newSetpointCmd[5:])[:1].split(',') #remove MOTXX10,20; --> 10,20 --> ["10", "20"]
	newMotorPower1 = int(tokens[0])
	newMotorPower2 = int(tokens[1])

	M1_POWER = calculateNewRampValue(M1_POWER, newMotorPower1)
	M2_POWER = calculateNewRampValue(M2_POWER, newMotorPower2)
	
	cmd = newSetpointCmd[5:] + str(M1_POWER) + "," + str(M2_POWER) + ";"
	return cmd

def calculateNewRampValue(current_setpoint, new_setpoint):
	if (current_setpoint == new_setpoint):
		return new_setpoint
	if (current_setpoint > new_setpoint ) and (current_setpoint - new_setpoint) < INCREMENT:
		return new_setpoint
	if (current_setpoint < new_setpoint ) and (new_setpoint - current_setpoint) < INCREMENT:
		return new_setpoint
	if (current_setpoint > new_setpoint ) and (current_setpoint - new_setpoint) >= INCREMENT:
		return (current_setpoint - INCREMENT)
	if (current_setpoint < new_setpoint ) and (new_setpoint - current_setpoint) >= INCREMENT:
		return (current_setpoint + INCREMENT)
	return current_setpoint


def taskReadSerialData(serialObj, publisher):
	while serialObj.isOpen() and not rospy.is_shutdown():
		if serialObj.available():
			publisher.publish(serialObj.readLine())

if __name__ == "__main__":
    # In ROS, nodes are uniquely named. If two nodes with the same
    # name are launched, the previous one is kicked off. The
    # anonymous=True flag means that rospy will choose a unique
    # name for our 'listener' node so that multiple listeners can
    # run simultaneously.
	rospy.init_node('remoteControlServer', anonymous=True)
	rospy.loginfo("available parameters::::::")
	rospy.loginfo(rospy.get_param_names())

	pub = rospy.Publisher('/encoder/raw', String, queue_size=10)
	rospy.init_node('encoder_talker', anonymous=True)
	rate = rospy.Rate(10) # 10hz
	#obtains the current configs
	
	#opens serial controller
	serialObj = serialOpen()
	#starts socket server for command handling

	#rospy.Subscriber('remote_control', String, serialHandler)

	serialDataThread = Thread(target=taskReadSerialData, args=(serialObj, pub, ))
	serialDataThread.start()

    # spin() simply keeps python from exiting until this node is stopped
	while not rospy.is_shutdown():
		rospy.loginfo("Moving front")
		for i in range(30):
			serialHandler("MOTFF120,120")
			time.sleep(.1)
		rospy.loginfo("Moving back")
		for i in range(30):
			serialHandler("MOTRR120,120")
			time.sleep(.1)
		rate.sleep()
	serialObj.serialClose()
	pass