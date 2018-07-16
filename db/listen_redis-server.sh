#!/bin/bash

#程序监控


while [ true ]
do
	pro=$(ps -ef | grep -v grep | grep "test.sh")
	if [ -z $pro ]
	then
		sh ./test.sh
	fi
	sleep 2
done
