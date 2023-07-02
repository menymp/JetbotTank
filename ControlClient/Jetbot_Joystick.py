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
sys.path.append("../JetbotServer")

from socketServerWrapper import socketClientW
from joystickUtills import uiPygameJetbot
'''
s = socket.socket()

host = 'xxx.xxx.x.x'
port = 8990 #1234
s.connect((host, port))
'''
 


def handleConfigs():
	configs = {
	}
	newParameters = parseLineCommands()
	nconfigs = mapCommands(configs, newParameters)
	return nconfigs

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

def mapCommands(currentConfigs, additionalArgs):
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
	return currentConfigs

def initSocketClient(configs):
	client = socketClientW()
	#print(configs)
	client.connect(configs['host'], configs['port'])
	return client

def userValuesMap(socketObj, configs, userCmds):
	mappedStr = ""
	#ToDo:  handle multiple joysticks For now just takes the first found joystick in the array
	mode = configs["joystickMode"]
	cmd = userCmds[0]
	if mode == "tank":
		mappedStr = tankMode(cmd)
	elif mode == "car":
		mappedStr = carMode(cmd)
	else:
		print("ERROR: '" + mode + "' is not defined!")
		sys.exit(1)
	
	socketObj.send(mappedStr) #by default encode to utf-8
	return True

def tankMode(cmdObj):
	CMD = ''
	Dir1 = 'f'
	Dir2 = 'f'
	POWER_M1 = 0
	POWER_M2 = 0
	
	for key, value in cmdObj.items():
		if key == "axis1":
			POWER_M2 = int(abs(value*253))
			if(value < 0):
				#valor menor que 0
				Dir2 = 'r'
			else:
				#valor mayor que 0
				Dir2 = 'f'
		elif key == "axis4":
			#valor del segundo eje
			POWER_M1 = int(abs(value*253))
			if(value < 0):
				#valor menor que 0
				Dir1 = 'r'
			else:
				#valor mayor que 0
				Dir1 = 'f'
	
	if(Dir1 == 'f' and Dir2 == 'f'):
		CMD = 'c' + ',' + str(POWER_M1)+','+str(POWER_M2)+'#'
	if(Dir1 == 'f' and Dir2 == 'r'):
		CMD = 'e' + ',' + str(POWER_M1)+','+str(POWER_M2)+'#'
	if(Dir1 == 'r' and Dir2 == 'f'):
		CMD = 'd' + ',' + str(POWER_M1)+','+str(POWER_M2)+'#'
	if(Dir1 == 'r' and Dir2 == 'r'):
		CMD = 'b' + ',' + str(POWER_M1)+','+str(POWER_M2)+'#'
	
	#print(CMD)
	#s.send(CMD.encode("utf-8"))
	return CMD

def carMode(cmdObj):
	return str

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
	pass
