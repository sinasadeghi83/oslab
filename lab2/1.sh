#!/bin/bash

read number

if [ $number -lt 0 ]; then
    echo -e "The weather is freezing\n"
elif [ $number -lt 30 ]; then
    echo -e "The weather is cool\n"
else
    echo -e "The weather is hot\n"
fi