#!/bin/bash

sudo insmod ps_linear.ko
sudo rmmod ps_linear
sudo dmesg -c
