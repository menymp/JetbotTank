'''
This file contains the working code for the jetbot tank

menymp 2023


starts a web camera server and a websocket interface server for  command control
'''
import sys
import signal
import time

from socketConnectionHandler import socketConnectionHandler
from serialControl import serialControl
from configsJetBotUtils import configsJetBotServerHandler
#not yet on use, but usefull if need to know availabe serial ports on windows and linux
from serialPortUtills import serial_ports
from CameraHttpServer import webcamIPServerHandle
from cameraServerController import videoHandler

serialObj = None
socketConnObj = None

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
	inputParameters = {
	}
	for argStr in sys.argv:
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
			currentConfigs["serialTimeout"] = int(value)
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
'''
if there are aditional processing things to do or parsing from arguments
those belong here

	#ser.write(b'a,0,0#')
	#print("detenido")
	#time.sleep(2)
	#ser.write(b'b,150,150#')
	#print("adelante")
	#time.sleep(2)
'''
def serialHandler(command):
	print(command)
	serialObj.write(command)
	pass
 
def exitHandler(signum, frame):
	socketConnObj.stop()
	serialObj.serialClose()
	exit(1)

if __name__ == "__main__":
	#sets the exit ctrl c signal
	signal.signal(signal.SIGINT, exitHandler)
	#obtains the current configs
	configs = handleConfigs()
	#opens serial controller
	serialObj = serialOpen(configs)
	#starts socket server for command handling
	socketConnObj = socketConnectionHandler(configs, serialHandler)
	socketConnObj.start()
	#sleeps until end of program
	while True:
		time.sleep(1)
		#webcamIPServerHandle('',8087) #old logic from 2018, unsuccessfully ported to python3
		#new logic with Tornado 6
		#check a way to manage multiple cams without threads
		connectionArgs = {
			"type": "picam",#can be local or picam for now
			"port": 9090,
			"width": 640,
			"height": 480,
			"camId":0,#ignore this for pi camera
		}
		videoHandlerObj = videoHandler(connectionArgs)
		videoHandlerObj.serverListen()
	print("ends")
	pass
#this class contains the logic to handle incoming connections from controller clients
#for now only one
'''
	
	serv.start()
	
	for x in range(3):
		data = serv.read(conn)
		serv.write(conn,"received --->" + str(data))
		time.sleep(2)
	serv.stop()
	pass
'''



'''
ser = serial.Serial('/dev/ttyACM0')
time.sleep(2)

HOST = ''
PORT = 8990
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
	s.bind((HOST,PORT))
except socket.error as msg:
	print('bind failed err')
	exit()

s.listen(10)
print('ready')
conn, addr = s.accept()

while 1:
	#conn, addr = s.accept()
	data = conn.recv(100).decode()
	BufferIn = str(data)
	print(BufferIn)
	ser.write(BufferIn.encode())
	
'''
