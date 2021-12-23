#!/bin/bash

if [ $# -ne 1 ]; then
 echo "Usage: $0 <Manager-instance-IP>"
 exit -1
fi

FIRESIM_KEYS_LOCATION=$1
MANAGER_INSTANCE_IP=$2

scp -i firesim.pem -r ./firesim.pem  ./firesim_config  ./hwacha-net  ./instance_setup.sh centos@$1:/home/centos
