'''
This file contains the working code for the jetbot tank

menymp 2023


starts a web camera server and a websocket interface server for  command control
'''
import sys

from socketConnectionHandler import socketConnectionHandler
from serialControl import serialControl
from configsJetBotServerUtils import configsJetBotHandler
from serialPortUtills import serial_ports

def handleConfigs():
	configs = loadConfigs()
	newParameters = parseLineCommands()
	saveNewConfigs(configs, newParameters)
	return loadConfigs()

def loadConfigs():
	configsHandler = configsJetBotHandler()
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
			currentConfigs["lastConnectionHost"] = value
		elif key == "--port":
			currentConfigs["lastConnectionPort"] = int(value)
		elif key == "--serialPath":
			currentConfigs["lastSerialPortPath"] = value
		elif key == "--serialTimeout":
			currentConfigs["serialTimeout"] = int(value)
		elif key == "--serialBaudRate":
			currentConfigs["serialBaudRate"] = int(value)
		elif key == "--maxLen":
			currentConfigs["maxLen"] = int(value)
		else:
			print("WARNING: command '" + key + "' still not defined, ignored!")
	configsHandler = configsJetBotHandler()
	configsHandler.saveConfigs(currentConfigs)
	return True

if __name__ == "__main__":
	print(handleConfigs())
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
	
	#ser.write(b'a,0,0#')
	#print("detenido")
	#time.sleep(2)
	#ser.write(b'b,150,150#')
	#print("adelante")
	#time.sleep(2)
'''