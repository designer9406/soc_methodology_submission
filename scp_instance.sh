#!/bin/bash

if [ $# -ne 1 ]; then
 echo "Usage: $0 <Manager-instance-IP>"
 exit -1
fi

MANAGER_INSTANCE_IP=$1

scp -i firesim.pem -r ./firesim.pem  ./firesim_config  ./hwacha-net  ./instance_setup.sh centos@$1:/home/centos
