#!/bin/bash

for n in $(seq 0 30); do
  N=$((2**$n))
  echo $N $(mpirun -n 2 pingpong.p $N)
done