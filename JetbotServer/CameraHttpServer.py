#!/usr/bin/python
'''
	Author: Igor Maculan - n3wtron@gmail.com
	A Simple mjpg stream http server
'''
import cv2
from PIL import Image
import threading
from BaseHTTPServer import BaseHTTPRequestHandler,HTTPServer
from SocketServer import ThreadingMixIn
import cStringIO
import os
import time
import Queue

capture=None
GSTREAMER_PIPELINE = 'nvarguscamerasrc ! video/x-raw(memory:NVMM), width=640, height=480, format=(string)NV12, framerate=21/1 ! nvvidconv flip-method=0 ! video/x-raw, width=640, height=480, format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink'


Buff_http = Queue.Queue(2)


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
					rc = True
					
					if not Buff_http.empty():
						img = Buff_http.get()
						rc = False
					
					if not rc:
						continue
					imgRGB=cv2.cvtColor(img,cv2.COLOR_BGR2RGB)
					jpg = Image.fromarray(imgRGB)
					tmpFile = cStringIO.StringIO()
					jpg.save(tmpFile,'JPEG')
					tmpFile.seek(0,os.SEEK_END)
					self.wfile.write("--jpgboundary\r\n")
					self.send_header('Content-type','image/jpeg')
					self.send_header('Content-length',str(tmpFile.tell()))
					self.end_headers()
					jpg.save(self.wfile,'JPEG')
					time.sleep(0.05)
				except KeyboardInterrupt:
					break
			return
		if self.path.endswith('.html'):
			self.send_response(200)
			self.send_header('Content-type','text/html')
			self.end_headers()
			self.wfile.write('<html><head></head><body>')
			self.wfile.write('<img src="cam.mjpg"/>')
			self.wfile.write('</body></html>')
			return


class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
	"""Handle requests in a separate thread."""

def main():
	
	objCamara = cv2.VideoCapture(GSTREAMER_PIPELINE, cv2.CAP_GSTREAMER)
	thread_actualizar = threading.Thread(target = Actualizar, args = (objCamara,Buff_http, ))
	thread_actualizar.start()
	

	
	try:
		server = ThreadedHTTPServer(('', 8080), CamHandler)
		print "server started"
		server.serve_forever()
	except KeyboardInterrupt:
		capture.release()
		server.socket.close()

if __name__ == '__main__':
	main()
