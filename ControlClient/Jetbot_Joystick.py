'''
menymp 24 jun 2023
example use for now:
python Jetbot_Joystick.py --mode=tank --host=IP --port=8990 --videoPort=9090
software to create a control client for the jetbot with a joystick
'''
import numpy as np
import urllib.request as request
import cv2
import sys
import math
import time
sys.path.append("../JetbotServer")

from socketServerWrapper import socketClientW
from joystickUtills import uiPygameJetbot
from configsJetBotUtils import configsJetBotClientHandler
'''
s = socket.socket()

host = 'xxx.xxx.x.x'
port = 8990 #1234
s.connect((host, port))
'''
'''
The following lines are the same in the server and client, place in a common file
'''
def handleConfigs():
	configs = loadConfigs()
	newParameters = parseLineCommands()
	saveNewConfigs(configs, newParameters)
	return loadConfigs()

def loadConfigs():
	configsHandler = configsJetBotClientHandler()
	configsObj = configsHandler.getConfigs()
	return configsObj

def saveNewConfigs(currentConfigs, additionalArgs):
	#Map current configs into the object and stores it
	# is there a better approach?? ToDo: review
	for key, value in additionalArgs.items():
		if key == "--mode":
			currentConfigs["joystickMode"] = value
		elif key == "--host":
			currentConfigs["host"] = value
		elif key == "--port":
			currentConfigs["port"] = int(value)
		elif key == "--videoPort":
			currentConfigs["videoPort"] = int(value)
		else:
			print("WARNING: command '" + key + "' still not defined, ignored!")
	configsHandler = configsJetBotClientHandler()
	configsHandler.saveConfigs(currentConfigs)
	return True

def parseLineCommands():	
	#loop over each arg to build the relevant args list
	inputParameters = {
	}
	for argStr in sys.argv:
		if argStr == "Jetbot_Joystick.py":
			continue
		argStr = argStr.replace(' ','')
		tokens = argStr.split('=')
		if len(tokens) != 2:
			print("arg: '" + argStr + "' unknown format")
			sys.exit(0)
		inputParameters[tokens[0]] = tokens[1]
	return inputParameters

def initSocketClient(configs):
	client = socketClientW()
	#print(configs)
	client.connect(configs['host'], configs['port'])
	return client

def userValuesMap(socketObj, configs, userCmds):
	#ToDo:  handle multiple joysticks For now just takes the first found joystick in the array
	mode = configs["joystickMode"]
	cmd = userCmds[0]
	if mode == "tank":
		mappedStr1 = tankMode(cmd)
		socketObj.send(mappedStr1)
		time.sleep(0.05)
		#socketObj.send(mappedStr2)
		#time.sleep(0.05)
	elif mode == "car":
		mappedStr = carMode(cmd)
		socketObj.send(mappedStr)
	else:
		print("ERROR: '" + mode + "' is not defined!")
		sys.exit(1)
	
	#socketObj.send(mappedStr) #by default encode to utf-8
	return True

def tankMode(cmdObj):
	CMD = ''
	Dir1 = 'f'
	Dir2 = 'f'
	rawValP1 = 0
	rawValP2 = 0
	POWER_M1 = 0
	POWER_M2 = 0
	
	for key, value in cmdObj.items():
		if key == "axis3":
			rawValP1 = value
		elif key == "axis4":
			rawValP2 = value
	leftP,rightP = singleJoystickTransform(rawValP1,rawValP2)
	POWER_M1 = int(abs(leftP*100))
	POWER_M2 = int(abs(rightP*100))
	DirT = "MOT"
	if(leftP < 0):
		DirT = DirT + "R"
	else:
		DirT = DirT + "F"
	
	if(rightP < 0):
		DirT = DirT + "R"
	else:
		DirT = DirT + "F"
	
	DirT = DirT + str(POWER_M1) + "," + str(POWER_M1) + ";"
	
	#print(CMD)
	
	#print("L R: " + str(leftP) + " " + str(rightP))
	#s.send(CMD.encode("utf-8"))
	return DirT

def carMode(cmdObj):
	return str


def singleJoystickTransform(x, y):
    # convert to polar
    r = math.hypot(x, y)
    t = math.atan2(y, x)

    # rotate by 45 degrees
    t += math.pi / 4

    # back to cartesian
    left = r * math.cos(t)
    right = r * math.sin(t)

    # rescale the new coords
    left = left * math.sqrt(2)
    right = right * math.sqrt(2)

    # clamp to -1/+1
    left = max(-1, min(left, 1))
    right = max(-1, min(right, 1))

    return left, right

def url_to_image(url):
	resp = request.urlopen(url)
	image = cv2.imdecode(np.frombuffer(resp.read(), np.uint8), 1)
	return image

def displayCamera(configs):
	window_name = 'camera'
	jpgVideoEndpoint = "http://"+configs["host"] + ":" + str(configs["videoPort"]) + "/video_feed"
	cv2.imshow(window_name, url_to_image(jpgVideoEndpoint))
	cv2.waitKey(1)

#ToDo: add the single joystick mapper for the single joystick mode

if __name__ == "__main__":
	configs = handleConfigs()
	socketClient = initSocketClient(configs)
	joystickUiObj = uiPygameJetbot()
	joystickUiObj.pygameInit()
	
	while True:
		userCmds, exitSignal = joystickUiObj.loop()
		if(exitSignal):
			break
		userValuesMap(socketClient, configs, userCmds)
		displayCamera(configs)
	# closing all open windows
	cv2.destroyAllWindows()
	socketClient.close()
	print("bye")
	pass
