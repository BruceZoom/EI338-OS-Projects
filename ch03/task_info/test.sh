#!/bin/bash

sudo insmod pid.ko

echo "1" > /proc/pid
cat /proc/pid

echo "2274" > /proc/pid
cat /proc/pid

sudo rmmod pid
sudo dmesg -c
