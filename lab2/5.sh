#!/bin/bash

path=$1

arr=( )

for i in $(find $path -type f)
do
    arr+=( $i )
done

echo ${#arr[@]}