#!/bin/sh
clear
clang++ -x c++ main.ino -std=c++17 -Wall -o /tmp/CA_Project_main && /tmp/CA_Project_main
2>/dev/null rm /tmp/CA_Project_main
