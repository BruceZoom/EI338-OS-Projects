#!/bin/bash

sudo insmod pid.ko

echo "12765" > /proc/pid
cat /proc/pid

echo "500" > /proc/pid
cat /proc/pid

sudo rmmod pid
sudo dmesg -c
