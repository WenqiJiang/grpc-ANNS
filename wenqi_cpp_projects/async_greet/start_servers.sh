#!/bin/bash
# Function: start N async servers with the script-defined IP and start port
# Usage: ./start_servers <num_servers>

num_servers=$1

IP=127.0.0.1
start_port=8888
end_port=`expr $start_port + $num_servers - 1`

bin='build/greeter_async_server'
PIDs=()

echo "Starting the servers..."
for (( port = $start_port; port <= $end_port; port++ ))
do
    echo "port: $port"
    $bin $IP:$port &
    PID=$!
    echo "PID: " $PID
    PIDs+=($PID)
done

echo "Press any key to kill the servers..."
while [ true ] ; do
    read -n 1
    if [ $? = 0 ] ; then
        break
    fi
done

echo "Killing the server processes..."
for PID in "${PIDs[@]}"
do 
    echo $PID
    kill -9 $PID
done