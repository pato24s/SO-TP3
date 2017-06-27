#!/bin/bash

if [ "$1" -eq "1" ];then
mpiexec -np 4 ./dist_hashmap << LimitString | sed -e '1,/finalizado/d' | sed '/>/d' | sort | diff -u - corpus-post
load corpus-0
load corpus-1
print
q
LimitString
fi

if [ "$1" -eq "2" ];then
mpiexec -np 3 ./dist_hashmap << LimitString | sed -e '1,/finalizado/d' | sed '/>/d' | sort | diff -u - corpus-post
load corpus-0
load corpus-1
load corpus-2
load corpus-3
load corpus-4
print
q
LimitString
fi

if [ "$1" -eq "3" ];then
mpiexec -np 5 ./dist_hashmap << LimitString | sed -e '1,/finalizado/d' | sed '/>/d' | sort | diff -u - corpus-post
load corpus-0
load corpus-1
load corpus-2
load corpus-3
print
q
LimitString
fi