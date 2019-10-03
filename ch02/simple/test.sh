#!/bin/bash

sudo insmod simple.ko
sudo rmmod simple
sudo dmesg -c
