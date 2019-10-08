#!/bin/bash

sudo insmod ps_dfs.ko
sudo rmmod ps_dfs
sudo dmesg -c
