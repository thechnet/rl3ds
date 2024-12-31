@echo off
cls
clang++ -x c++ main.ino -std=c++17 -Wall -o "%temp%/CA_Project_main.exe" && "%temp%\CA_Project_main.exe"
2>nul del "%temp%\CA_Project_main.exe"
