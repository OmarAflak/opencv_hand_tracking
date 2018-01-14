#! /bin/sh
g++ main.cpp -o main `pkg-config --cflags --libs opencv`
