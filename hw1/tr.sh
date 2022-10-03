#!/usr/bin/bash

WORKING_DIR="/home/cole/Documents/ece355"

on_cont="8.8.8.8" # google DNS
off_cont="84.200.69.80" # dns.watch - german DNS server

echo "==== `date` ====" >> $WORKING_DIR/on-cont
echo "==== `date` ====" >> $WORKING_DIR/off-cont

traceroute $on_cont >> $WORKING_DIR/on-cont
traceroute $off_cont >> $WORKING_DIR/off-cont

echo -e "========\n" >> $WORKING_DIR/on-cont
echo -e "========\n" >> $WORKING_DIR/off-cont
