#!/usr/bin/python
'''
	Author: Igor Maculan - n3wtron@gmail.com
	A Simple mjpg stream http server
'''
'''
ToDo: a major refractor of this should be crafted
'''
'''
ToDo: migrate to Python3
menymp
'''
import cv2
from PIL import Image
import threading
from http.server import BaseHTTPRequestHandler,HTTPServer
from socketserver import ThreadingMixIn
#import cStringIO
from io import BytesIO as cStringIO
import os
import time
import queue

capture=None
GSTREAMER_PIPELINE = 'nvarguscamerasrc ! video/x-raw(memory:NVMM), width=640, height=480, format=(string)NV12, framerate=21/1 ! nvvidconv flip-method=0 ! video/x-raw, width=640, height=480, format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink'


Buff_http = queue.Queue(2)


def Actualizar(Cam_obj,Buff_out):
	while True:
		ret,frame = Cam_obj.read()
		if not Buff_out.full():
			Buff_out.put(frame)
		#si se va a realizar procesamiento de imagenes
		#se hace aqui, la imagen resultante se manda al buffer
			
	Cam_obj.release()



class CamHandler(BaseHTTPRequestHandler):
	def do_GET(self):
		if self.path.endswith('.mjpg'):
			self.send_response(200)
			self.send_header('Content-type','multipart/x-mixed-replace; boundary=--jpgboundary')
			self.end_headers()
			while True:
				try:
					print("keh")
					rc = True
					
					if not Buff_http.empty():
						img = Buff_http.get()
						rc = False
					
					if not rc:
						continue
					imgRGB=cv2.cvtColor(img,cv2.COLOR_BGR2RGB)
					jpg = Image.fromarray(imgRGB)
					tmpFile = cStringIO()
					jpg.save(tmpFile,'JPEG')
					tmpFile.seek(0,os.SEEK_END)
					self.wfile.write(b'--jpgboundary\r\n')
					self.send_header('Content-type','image/jpeg')
					self.send_header('Content-length',str(tmpFile.tell()))
					self.end_headers()
					jpg.save(self.wfile,'JPEG')
					time.sleep(0.05)
					print("ending")
				except KeyboardInterrupt:
					print("except")
					break
			print("exiting")
			return
		if self.path.endswith('.html'):
			self.send_response(200)
			self.send_header('Content-type','text/html')
			self.end_headers()
			self.wfile.write(b'<html><head></head><body>')
			self.wfile.write(b'<img src="cam.mjpg"/>')
			self.wfile.write(b'</body></html>')
			return


class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
	"""Handle requests in a separate thread."""

def webcamIPServerHandle(host, port):
	#objCamara = cv2.VideoCapture(GSTREAMER_PIPELINE, cv2.CAP_GSTREAMER)
	objCamara = cv2.VideoCapture(0) #for testing purposes, expected to use the upper line in jetson nano
	thread_actualizar = threading.Thread(target = Actualizar, args = (objCamara,Buff_http, ))
	thread_actualizar.start()
	
	try:
		server = ThreadedHTTPServer((host, port), CamHandler)
		print("server started")
		server.serve_forever()
		print("server stops")
	except KeyboardInterrupt:
		print("server error")
		capture.release()
		server.socket.close()

if __name__ == '__main__':
	webcamIPServerHandle('',8080)
