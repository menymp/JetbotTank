#!/usr/bin/env python3
'''
This file contains the working code for the jetbot tank

menymp 2023


starts a web camera server and a websocket interface server for  command control

History
2024 Jul 07
	adapted to ROS environment
'''
import sys
import signal
import time
from threading import Thread, Event
import math
import tf
from nav_msgs.msg import Odometry
from geometry_msgs.msg import Point, Pose, Quaternion, Twist, Vector3

#ROS packages
import rospy
from std_msgs.msg import String

from remote_control.serialControl import serialControl
from remote_control.configsJetBotUtils import configsJetBotServerHandler
from remote_control.cameraServerController import videoHandler

serialObj = None

def handleConfigs():
	configs = loadConfigs()
	newParameters = parseLineCommands()
	saveNewConfigs(configs, newParameters)
	return loadConfigs()

def loadConfigs():
	configsHandler = configsJetBotServerHandler()
	configsObj = configsHandler.getConfigs()
	return configsObj

def parseLineCommands():	
	#loop over each arg to build the relevant args list
	args = rospy.get_param("~lineParameters").split(' ')#ToDo: parametrize this
	inputParameters = {
	}
	for argStr in args:
		if argStr == "main.py":
			continue
		argStr = argStr.replace(' ','')
		tokens = argStr.split('=')
		if len(tokens) != 2:
			print("arg: '" + argStr + "' unknown format")
			sys.exit(0)
		inputParameters[tokens[0]] = tokens[1]
	return inputParameters

def saveNewConfigs(currentConfigs, additionalArgs):
	#Map current configs into the object and stores it
	# is there a better approach?? ToDo: review
	for key, value in additionalArgs.items():
		if key == "--host":
			currentConfigs["host"] = value
		elif key == "--port":
			currentConfigs["port"] = int(value)
		elif key == "--serialPath":
			currentConfigs["portPath"] = value
		elif key == "--serialTimeout":
			currentConfigs["serialTimeout"] = float(value)
		elif key == "--serialBaudRate":
			currentConfigs["serialBaudRate"] = int(value)
		elif key == "--maxLen":
			currentConfigs["maxLen"] = int(value)
		else:
			print("WARNING: command '" + key + "' still not defined, ignored!")
	configsHandler = configsJetBotServerHandler()
	configsHandler.saveConfigs(currentConfigs)
	return True

def serialOpen(configs):
	serialObj = serialControl()
	serialObj.serialOpen(configs["portPath"],configs["serialBaudRate"],configs["serialTimeout"],configs["maxLen"])
	return serialObj

def serialHandler(command):
	cmd = command.data
	print(cmd)
	serialObj.write(cmd)
	pass

def taskReadSerialData(serialObj):
	while serialObj.isOpen():
		if serialObj.available():
			handleIncomingData(serialObj.readLine())




# Encoder characteristics
STEP_LENGTH = 0.015   #ToDo: measure this with caliper, distance in mts
TRACK_DISTANCE = 0.20	#ToDo: measure this with caliper, distance in mts

#Global positions
x_pos = 0.0
y_pos = 0.0
actualTheta = 0.0
last_time = rospy.Time.now()

odom_pub = rospy.Publisher("odom", Odometry, queue_size=50)
odom_broadcaster = tf.TransformBroadcaster()

def handleIncomingData(data):
	#calculate oddometry
	print(data)

	tokens = data.split(':')

	#calculate differential oddometry
	dir1 = int(tokens[0])
	dir2 = int(tokens[1])
	steps1 = int(tokens[7])
	steps2 = int(tokens[8])

	current_time = rospy.Time.now() 
	dt = (current_time - last_time).toSec()

	currentEnc1Distance = steps1 * STEP_LENGTH
	currentEnc2Distance = steps2 * STEP_LENGTH

	if (dir1):
		currentEnc1Distance = -currentEnc1Distance
	else:
		currentEnc1Distance = currentEnc1Distance

	if (dir2):
		currentEnc2Distance = -currentEnc2Distance
	else:
		currentEnc2Distance = currentEnc2Distance

	Davg = float(currentEnc1Distance + currentEnc2Distance) / 2.0
	deltaTheta = float(currentEnc1Distance - currentEnc2Distance) / float(TRACK_DISTANCE)

	# Keep angle between -PI and PI  
	if (newTheta> math.pi):
		newTheta = newTheta - (2 * math.pi)
	if (newTheta < -math.pi):
		newTheta = newTheta + (2 * math.pi)

	deltaX = Davg * math.cos(newTheta) 
	deltaY = Davg * math.sin(newTheta)

	# calculate new positions with differences
	x_pos = x_pos + deltaX
	y_pos = y_pos + deltaY
	newTheta = actualTheta + deltaTheta

	# obtain XY speeds
	vx = deltaX / dt
	vy = deltaY / dt
	vth = deltaTheta / dt
	#convert to odom message and publish
	# since all odometry is 6DOF we'll need a quaternion created from yaw
	odom_quat = tf.transformations.quaternion_from_euler(0, 0, newTheta)
	odom_broadcaster.sendTransform(
        (x_pos, y_pos, 0.),
        odom_quat,
        current_time,
        "base_link",
        "odom"
    )

    # next, we'll publish the odometry message over ROS
	odom = Odometry()
	odom.header.stamp = current_time
	odom.header.frame_id = "odom"

    # set the position
	odom.pose.pose = Pose(Point(x_pos, y_pos, 0.), Quaternion(*odom_quat))

    # set the velocity
	odom.child_frame_id = "base_link"
	odom.twist.twist = Twist(Vector3(vx, vy, 0), Vector3(0, 0, vth))

    # publish the message
	odom_pub.publish(odom)

	last_time = rospy.Time.now()
	pass

def taskVideoServer(configs):
	#ToDo: parametrize this
	connectionArgs = {
		"type": "picam",#can be local or picam for now
		"port": configs["port"],
		"width": 640,
		"height": 480,
		"camId":0,#ignore this for pi camera
	}
	videoHandlerObj = videoHandler(connectionArgs)
	videoHandlerObj.serverListen()
	pass

if __name__ == "__main__":
    # In ROS, nodes are uniquely named. If two nodes with the same
    # name are launched, the previous one is kicked off. The
    # anonymous=True flag means that rospy will choose a unique
    # name for our 'listener' node so that multiple listeners can
    # run simultaneously.
	rospy.init_node('remoteControlServer', anonymous=True)
	rospy.loginfo("available parameters::::::")
	rospy.loginfo(rospy.get_param_names())
	#obtains the current configs
	configs = handleConfigs()
	#opens serial controller
	serialObj = serialOpen(configs)
	#starts socket server for command handling

	rospy.Subscriber('remote_control', String, serialHandler)

	event = Event()
	thread = Thread(target=taskVideoServer, args=(configs, ))
	thread.start()

    # spin() simply keeps python from exiting until this node is stopped
	rospy.spin()
	serialObj.serialClose()
	pass