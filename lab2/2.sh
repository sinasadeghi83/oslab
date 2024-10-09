#!/bin/bash

a=$1
op=$2
b=$3

echo -e "$a\n$op\n$b\n"

function is_num(){
    check=$(echo "$1==$1" | bc)
    [ -n $1 ] && [ $check -eq 1 ] 2>/dev/null;
    if [ $? -eq 0 ]; then
        return 0
    fi
    return 1
}

function calculate(){
    temp=$(echo "scale=3; $1" | bc)
    echo -n $temp
    return $?
}

is_num $a
if [ $? -ne 0 ]; then
    echo "Error: first operand is not an integer"
    exit
fi

is_num $b
if [ $? -ne 0 ]; then
    echo "Error: second operand is not an integer"
    exit
fi

case $op in
    "+")
    d=$(calculate "$a + $b")
    ;;

    "*")
    d=$(calculate "$a \* $b")
    ;;

    "/")
    if [ $(calculate "$b==0") -eq 1 ]; then
    echo "Error: divison by zero"
    exit
    fi 
    d=$(calculate "$a / $b")
    ;;

    "-")
    d=$(calculate "$a - $b")
    ;;

    *)
    echo "Error: operator is invalid"
    exit
    ;;
esac

echo $d