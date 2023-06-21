import serial
import socket
import time

from baseStateMessage import state, createStateMsg
#es recomendable anadir al usuario como
#parte del grupo dialout para poder usar
#los puertos serial
#el comando es:
# sudo adduser $USER dialout

class socketServerWrapper()
	def __init__(self, configsObj):
		self.HOST = configsObj["host"]
		self.port = configsObj["port"]
		
		pass
	
	def start(self):
		self.socketObj = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		result = createStateMsg(state.SUCCESS_TYPE, "port open")
		try:
			self.socketObj.bind((self.HOST,self.PORT))
		except socket.error as msg:
			result = createStateMsg(state.FAILURE_TYPE, msg)
		return result
	
	def acceptConn(self, conn = 10):
		self.socketObj.listen(conn)
		conn, addr = self.socketObj.accept()
		return conn, addr