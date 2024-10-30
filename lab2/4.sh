#!/bin/bash

read number

while [ $number -gt 0 ];
do
echo -n $(( number%10 ))
let number=$number/10
done

echo -e "\n"
