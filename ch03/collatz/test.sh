#!/bin/bash

echo "start = 35"
sudo insmod collatz.ko start=35
sudo dmesg -c
sudo rmmod collatz
sudo dmesg -c

echo "start = default"
sudo insmod collatz.ko
sudo dmesg -c
sudo rmmod collatz
sudo dmesg -c
