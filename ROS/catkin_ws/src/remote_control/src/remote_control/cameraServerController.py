import time
import tornado.ioloop
import tornado.web
import tornado.httpserver
import tornado.process
import tornado.template
import gen
import os
from threading import Timer
import json
import asyncio

from remote_control.BaseVideoService import jetsonPICameraService, localCameraService

class videoFeedHandler(tornado.web.RequestHandler):
	def initialize(self, videoService):
		self.videoService = videoService
	
	@tornado.gen.coroutine
	def get(self):
		self.set_header('Cache-Control', 'no-store, no-cache, must-revalidate, pre-check=0, post-check=0, max-age=0')
		self.set_header( 'Pragma', 'no-cache')
		self.set_header( 'Content-Type', 'multipart/x-mixed-replace;boundary=--jpgboundary')
		self.set_header('Connection', 'close')
		
		my_boundary = "--jpgboundary"
		
		#Note: this code was based on Node server logic, the following lines are to process incoming
		#	   GET rest requests json parameters
		#strArg = self.get_argument('vidArgs')
		#argsObj = json.loads(strArg)
		#argsObj = json.loads(argsObj)
		#img = self.videoService.getJpg(argsObj)
		
		img = self.videoService.getJpg()
		
		#self.write(my_boundary)
		#self.write("Content-type: image/jpeg\r\n")
		#self.write("Content-length: %s\r\n\r\n" % len(img))
		self.write(img)
		
		self.flush()

class videoHandler():
	def __init__(self, args):
		'''
		connectionArgs = {
			"type": "local",#can be local or picam for now
			"port": 9090,
			"width": 640,
			"height": 480,
			"camId":0,#ignore this for pi camera
		}
		'''
		#
		self.connArgs = args
		if args["type"] == "local":
			self.videoService = localCameraService(args)
		elif args["type"] == "picam":
			self.videoService = jetsonPICameraService(args)
		else:
			raise Exception("non supported cam type: " + args["type"])
		
		self.app = self._make_app(self.videoService)
		pass
	
	def serverListen(self):
		asyncio.set_event_loop(asyncio.new_event_loop())
		self.server = tornado.httpserver.HTTPServer(self.app)
		self.server.listen(self.connArgs["port"]) #parametrize this
		tornado.ioloop.IOLoop.current().start()
	
	def _make_app(self, videoService):
		# add handlers
		return tornado.web.Application([(r'/video_feed', videoFeedHandler, {'videoService': videoService})],)