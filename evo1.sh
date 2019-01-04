#!/bin/bash

while true; do
    ./evo1
    if [ $? -eq 0 ]
    then
        break
    fi
    sleep 0.5
done
