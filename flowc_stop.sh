#!/bin/sh

PID=$(pidof flowc)

if [ -n $PID ];
then
    kill -15 $PID
fi
