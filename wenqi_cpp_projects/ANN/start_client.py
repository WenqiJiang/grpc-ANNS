"""
Start the client that sends request to multiple servers.

Example usage:
    python start_client.py --num_servers 8 --IP 127.0.0.1 --start_port 8888
"""

import os
import argparse 

parser = argparse.ArgumentParser()
parser.add_argument('--num_servers', type=int, default=1)
parser.add_argument('--IP', type=str, default='127.0.0.1')
parser.add_argument('--start_port', type=int, default=8888)

args = parser.parse_args()
num_servers = args.num_servers
IP = args.IP
start_port = args.start_port

bin='build/greeter_async_client_multi_servers'

arguments = " "
for i in range(num_servers):
	arguments += IP + ':' + str(start_port + i) + ' '

cmd = bin + arguments
print("run command: ", cmd)
os.system(cmd)