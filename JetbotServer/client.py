import cv2
import numpy as np
import socket
import sys
import cPickle
#import pickle
import struct ### new code

#from PIL import Image
#import base64
#import StringIO
#from Adafruit_IO import Client

GSTREAMER_PIPELINE = 'nvarguscamerasrc ! video/x-raw(memory:NVMM), width=640, height=480, format=(string)NV12, framerate=21/1 ! nvvidconv flip-method=0 ! video/x-raw, width=640, height=480, format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink'

cap=cv2.VideoCapture(GSTREAMER_PIPELINE, cv2.CAP_GSTREAMER)
clientsocket=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
#48cc0d6ae94c4cb3b52a298f9156ad9a
#aio = Client('48cc0d6ae94c4cb3b52a298f9156ad9a')

clientsocket.connect(("192.168.1.128",8089))
#192.168.1.86
#192.168.10.106
#192.168.10.114
#    print "Servidor No Encontrado"
while True:
    
    ret,frame=cap.read()
    
    #if frame is None:
	#	continue
    '''
    jpg = np.fromarray(frame)
    tmpFile = StringIO.StringIO()
    jpg.save(tmpFile,'JPEG')
    
    #Strings = base64.b64encode(frame)
    aio.send('cam',tmpFile)
    '''
    frameShort = cv2.resize(frame,(640,480),interpolation = cv2.INTER_AREA)
    data = cPickle.dumps(frameShort,protocol = cPickle.HIGHEST_PROTOCOL) ### new code
    clientsocket.sendall(struct.pack("L", len(data))+data) ### new code
    
    #cv2.imshow("fig",frame)
    #cv2.waitKey(0)
    #    print "Error de conexion"
    #    break
    
