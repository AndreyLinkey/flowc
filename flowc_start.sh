#!/bin/sh

PID=$(pidof flowc)

if [ -z $PID ];
then
    cd $(dirname "$0")
    flowc
fi
