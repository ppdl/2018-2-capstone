#!/bin/bash
sudo sysctl -w net.core.rmem_default=4096
sudo sysctl -w net.core.rmem_max=4096
sudo sysctl -w net.ipv4.tcp_mem='4096 4096 4096'
sudo sysctl -w net.ipv4.tcp_rmem='4096 4096 4096'

