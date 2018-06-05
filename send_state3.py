import socket # for socket communication

# for artikcloud
import artikcloud
from artikcloud.rest import ApiException
import sys, getopt
import time, json
from pprint import pprint

# warshing machine state
state = ["idle", "avilable"]


def main():
	global state

	server_ip = '192.168.0.28'	# IP of server(ARTIK), please update if Server IP change
	server_port = 3490
	max_users = 5

	# make server_socket
	server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	server_socket.bind((server_ip, server_port))

	# listen client socket
	server_socket.listen(max_users)

	print "Waiting for client on port",server_port,"\n"

	count = 0
	
	while True:
		client_socket, client_address = server_socket.accept()
		print "Connected :", client_address[0] # print client IP
		while True:
			data = client_socket.recv(128)
			if not data:
				break
			(num, new_state) = data.split() # split index, state
			if state[int(num)]!=new_state:  # when state change
				state[int(num)] = new_state
				send_cloud()
			print "state of laundry ", num , ":", state[int(num)]
			print "Disconnected :", client_address[0], "\n"
		client_socket.close()
		count = count+1
	
	server_socket.close()

# send state to artik cloud
def send_cloud():
	global state
	
	DEFAULT_CONFIG_PATH = 'config2.json'
	
	with open(DEFAULT_CONFIG_PATH, 'r') as config_file:
			config = json.load(config_file)
	# print(config)

	# Configure Oauth2 access token for the client application
	artikcloud.configuration = artikcloud.Configuration();
	artikcloud.configuration.access_token = config['device_token']

	# Create an instance of the API Class
	api_instance = artikcloud.MessagesApi()
	device_message = {}
	device_message['State1'] = state[0]
	device_message['State2'] = state[1]
	device_sdid = config['device_id']
	ts = None                         

	# Construct a Message Object for request
	data = artikcloud.Message(device_message, device_sdid, ts)

	try:
		# pprint(artikcloud.configuration.auth_settings()) # Debug Print
		api_response = api_instance.send_message(data) # Send Message
		# pprint(api_response)
	except ApiException as e:
		pprint("Exception when calling MessagesApi->send_message: %s\n" % e)

main()