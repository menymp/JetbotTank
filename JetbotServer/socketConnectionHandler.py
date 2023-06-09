'''
menymp Jun 23 2023
this file contains the handler class for user conection handler
'''
from threading import Thread
from threading import Lock
from threading import Event

from socketServerWrapper import socketServerW

class socketConnectionHandler():
	def __init__(self, configs, messageReceivedHandler):
		'''
		self.configs = {
		"host":"localhost",
		"port":9797
		}
		'''
		self.messageReceivedHandler = messageReceivedHandler
		self.serv = socketServerW(configs)
		self.handleSocketTask = None
		self.stopEvent = Event()
		pass
		
	def start(self):
		if self.handleSocketTask is not None:
			print("warning!! a thread already started for this, restarting")
			self.stop()
		self.handleSocketTask = Thread(target = self._serverTaskHandler)
		self.handleSocketTask.start()
		pass
	
	def stop(self):
		self.stopEvent.set()
		self.serv.stop()
		print("exit")
		pass
		
	def _serverTaskHandler(self):
		self.serv.start()
		while(self.stopEvent.is_set() == False):
			print("Server listening for connection!")
			conn, addr  = self.serv.acceptConn()
			print("connection received from: " + str(addr))
			while True:
				recv = self.serv.read(conn)
				if not recv:
					break
				self.messageReceivedHandler(recv)
		pass