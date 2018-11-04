#!/bin/bash
python3 ../application/pktrcv.py
sudo insmod ../netfilter/hub/iotyr-hub.ko
