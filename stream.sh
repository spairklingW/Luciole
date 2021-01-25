#!/bin/bash


cd build
raspivid -o - -t 0 -hf -w 640 -h 360 -fps 25 | ./Main_Stream
