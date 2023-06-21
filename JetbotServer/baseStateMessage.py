from enum import Enum
 
class state(Enum):
	SUCCESS_TYPE = 0
	FAILED_TYPE = 1
	PENDING_TYPE 2



def createStateMsg(result, msg):
	msgObj = {
		"result":result,
		"message":msg
	}
	return msgObj