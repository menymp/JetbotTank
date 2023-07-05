import cv2
import numpy as np

#ToDo: sets a base and split into specific configs
#	expected hw to support
#		usb webcam / IP camera
#		PI camera
#		kinect cam ??
# these drivers should be part of a generic lib shared with device node server ??
class jetsonPICameraService():
	GSTREAMER_PIPELINE = 'nvarguscamerasrc ! video/x-raw(memory:NVMM), width=640, height=480, format=(string)NV12, framerate=21/1 ! nvvidconv flip-method=0 ! video/x-raw, width=640, height=480, format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink'
	objCamara = cv2.VideoCapture(GSTREAMER_PIPELINE, cv2.CAP_GSTREAMER)


class BaseVideoService():
	def __init__(self, connectionArgs = None):
		self.connectionArgs = connectionArgs
		self.image = np.zeros((640,480,3), dtype=np.uint8)
		cv2.putText(self.image, "No Image", (10,50), cv2.FONT_HERSHEY_SIMPLEX, 3, (255,255,255), 2, cv2.LINE_AA)
		#Init CV2 camera obj
		self.cam = cv2.VideoCapture(0)
		pass

	def _readImage(self, connectionArgs = None):
		# select first video device in system
		#self.cam = cv2.VideoCapture(0) #ToDo: the parameters must contain init camera cases
		#this in order to support jetbot raspicam
		#ToDo: this logic should be part of another class for specific init purposes
		# set camera resolution
		self.w = 640
		self.h = 480
		# set crop factor
		#self.cam.set(cv2.CAP_PROP_FRAME_HEIGHT, self.h)
		#self.cam.set(cv2.CAP_PROP_FRAME_WIDTH, self.w)
		
		
		success, image = self.cam.read()
		return success, cv2.resize(image, (self.w, self.h))
	
	#ToDo: h and w are shared by the thread, so a threadsafe approach should be 
	#      implemented in the future
	def set_resolution(self, new_w, new_h):
		if isinstance(new_h, int) and isinstance(new_w, int):
			# check if args are int and correct
			if (new_w <= 800) and (new_h <= 600) and \
				(new_w > 0) and (new_h > 0):
				self.h = new_h
				self.w = new_w
			else:
				# bad params
				raise Exception('Bad resolution')
		else:
			# bad params
			raise Exception('Not int value')
	
	def getJpg(self):
		rs, img = self._readImage()
		if not rs:
			return False, None
		ret, jpeg = cv2.imencode('.jpg', img)
		return jpeg.tobytes()