from socketServerWrapper import socketClientW

'''
#Server example code
if __name__ == "__main__":
	configs = {
		"host":"localhost",
		"port":9797
	}
	serv = socketServerW(configs)
	serv.start()
	conn, addr  = serv.acceptConn()
	for x in range(3):
		data = serv.read(conn)
		serv.write(conn,"received --->" + str(data))
		time.sleep(2)
	serv.stop()
	pass
'''
client = socketClientW()
client.connect('localhost', 9797)

client.send("halloe")
while True:
	data = client.recv()
	print("received -->" + str(data))
	client.send("halloe s")
	pass

print("end")