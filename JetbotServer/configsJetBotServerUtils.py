'''
reconverted form yapsc project, better to make a shared module if this will be repeated or not???
'''
from baseConfigs import baseConfigs

ALLOWED_CONN_PORTS = (8081, 10000) #MIN MAX buffer len for port
SERIAL_TIMEOUT = (0.01,0.5)
ALLOWED_BAUDS = [9600, 19200, 38400, 57600, 115200]


class configsJetBotHandler(baseConfigs):
	def __init__(self):
		self.DEFAULT_PATH = "./configsJetBot.json"
		self.DEFAULT_CONFIGS_STRUCT = {
			"lastConnectionHost": '',
			"lastConnectionPort": 8990,
			"lastSerialPortPath": '/dev/ttyACM0',
			"serialTimeout": 0.07,
			"serialBaudRate": 9600,
			"maxLen": 50
		}
		pass
	
	def checkConfigFields(self, path = None):
		if path is None:
			spath = self.DEFAULT_PATH
		else:
			spath = path
		
		configObj = self.readConfigFile(spath)
		if configObj["serialBaudRate"] not in ALLOWED_BAUDS:
			return False		
		if configObj["lastConnectionHost"] == "":#ToDo: Evaluate use a regex
			return False 
		if configObj["lastSerialPortPath"] == "":#ToDo: Evaluate use a regex
			return False 
		if  not (ALLOWED_CONN_PORTS[0] < configObj["lastConnectionPort"] <= ALLOWED_CONN_PORTS[1]):
			return False
		if  not (SERIAL_TIMEOUT[0] < configObj["serialTimeout"] <= SERIAL_TIMEOUT[1]):
			return False
		return True	
	
	


