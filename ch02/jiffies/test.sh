#!/bin/bash

sudo insmod jiffies.ko
cat /proc/jiffies
sudo rmmod jiffies
sudo dmesg -c
