#!/bin/bash

sudo insmod color.ko
sudo dmesg -c
sudo rmmod color
sudo dmesg -c
