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
	cmd = str(command)
	print(cmd)
	serialObj.write(command)
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