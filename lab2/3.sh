#!/bin/bash

function calculate(){
    temp=$(echo "scale=3; $1" | bc)
    echo -n $temp
    return $?
}

read number

a=$(calculate "$number*9/5+32")
echo $a