#!/bin/bash

echo "10" >input.txt #input file w/ single int

./printFibonacci < input.txt | ./printAdd1K > output.txt #redirect input.txt to printFibonacci.c, pipe output to printAdd1K, redirect final result to output.txt

cat output.txt #display final output
