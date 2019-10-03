#!/bin/bash

sudo insmod seconds.ko
sleep 3
cat /proc/seconds
sleep 2
cat /proc/seconds
sudo rmmod seconds
sudo dmesg -c
