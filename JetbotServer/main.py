'''
This file contains the working code for the jetbot tank

menymp 2023


starts a web camera server and a websocket interface server for  command control
'''
from socketServerWrapper import socketServerWrapper
from serialControl import serialControl
from configsUtils import *
from serialControl import serialControl
from serialPortUtils import serialPortUtils
#this class contains the logic to handle incoming connections from controller clients
#for now only one
class socketConnectionHandler():
	pass

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