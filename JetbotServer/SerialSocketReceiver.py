import serial
import socket
import time
#es recomendable anadir al usuario como
#parte del grupo dialout para poder usar
#los puertos serial
#el comando es:
# sudo adduser $USER dialout
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
