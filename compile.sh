#!/bin/bash

mpic++ -g -O3 main.cpp -o vec_operations.p
mpic++ -g -O3 pingPong.cpp -o pingpong.p
mpic++ -g -O3 functional.cpp -o functional.p