import serial
import socket
import time

from baseStateMessage import state, createStateMsg

'''
Simple socket wrapper to handle repetitive specific tasks

ToDo: add error handling using base state message class
'''

class socketServerW():
	def __init__(self, configsObj):
		self.HOST = configsObj["host"]
		self.PORT = configsObj["port"]
		pass
	
	def start(self):
		self.socketObj = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		result = createStateMsg(state.SUCCESS_TYPE, "port open")
		try:
			self.socketObj.bind((self.HOST,self.PORT))
		except socket.error as msg:
			result = createStateMsg(state.FAILURE_TYPE, msg)
		return result
	
	def stop(self):
		#self.socketObj.shutdown(socket.SHUT_RDWR)
		self.socketObj.close()
		return True
		
	
	def acceptConn(self, conn = 10):
		self.socketObj.listen(conn)
		conn, addr = self.socketObj.accept()
		return conn, addr
	
	def read(self, conn, buffLen = 100):
		return conn.recv(buffLen).decode()
	
	def write(self, conn, buff):
		conn.send(buff.encode())
		pass

class socketClientW():
	def __init__(self):
		self.sock = socket.socket()
		pass
	
	def connect(self, host, port):
		self.sock.connect((host, port))
		pass
	
	def close(self):
		self.sock.close()
		pass
			
	
	def send(self, buff, encodeFmt = "utf-8"):
		self.sock.send(buff.encode(encodeFmt))
	
	def recv(self, buffLen = 4096):
		return self.sock.recv(buffLen).decode()