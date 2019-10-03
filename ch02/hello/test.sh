#!/bin/bash

sudo insmod hello.ko
cat /proc/hello
sudo rmmod hello
sudo dmesg -c
