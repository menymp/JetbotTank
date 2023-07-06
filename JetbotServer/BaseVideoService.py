import cv2
import numpy as np

#ToDo: sets a base and split into specific configs
#	expected hw to support
#		usb webcam / IP camera
#		PI camera
#		kinect cam ??
# these drivers should be part of a generic lib shared with device node server ??

'''
connectionArgs = {
	"width":640,
	"height":480,
	"camId":0,#ignore this for pi camera
}
'''
class BaseVideoService():
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

class jetsonPICameraService(BaseVideoService):
	def __init__(self, connectionArgs):
		self.connectionArgs = connectionArgs
		self.GSTREAMER_PIPELINE = 'nvarguscamerasrc ! video/x-raw(memory:NVMM), width='+connectionArgs["width"]+', height='+connectionArgs["height"]+', format=(string)NV12, framerate=21/1 ! nvvidconv flip-method=0 ! video/x-raw, width='+connectionArgs["width"]+', height='+connectionArgs["height"]+', format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink'
		self.image = np.zeros((connectionArgs["width"],connectionArgs["height"],3), dtype=np.uint8)
		cv2.putText(self.image, "No Image", (10,50), cv2.FONT_HERSHEY_SIMPLEX, 3, (255,255,255), 2, cv2.LINE_AA)
		#Init CV2 camera obj
		self.cam = cv2.VideoCapture(GSTREAMER_PIPELINE, cv2.CAP_GSTREAMER)
		self.w = connectionArgs["width"]
		self.h = connectionArgs["width"]
		pass

	def _readImage(self):
		# select first video device in system
		success, image = self.cam.read()
		return success, cv2.resize(image, (self.w, self.h))




class localCameraService(BaseVideoService):
	def __init__(self, connectionArgs):
		self.connectionArgs = connectionArgs
		self.image = np.zeros((connectionArgs["width"],connectionArgs["width"],3), dtype=np.uint8)
		cv2.putText(self.image, "No Image", (10,50), cv2.FONT_HERSHEY_SIMPLEX, 3, (255,255,255), 2, cv2.LINE_AA)
		#Init CV2 camera obj
		self.cam = cv2.VideoCapture(connectionArgs["camId"])
		self.w = connectionArgs["width"]
		self.h = connectionArgs["width"]
		pass

	def _readImage(self):
		# select first video device in system
		success, image = self.cam.read()
		return success, cv2.resize(image, (self.w, self.h))
