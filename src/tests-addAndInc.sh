#!/bin/bash

if [ "$1" -eq "1" ];then
mpiexec -np 3 ./dist_hashmap << LimitString | sed -e '1,/primerstring/d' | sed '/>/d' | diff -u - <(echo "primerstring 1")
addAndInc primerstring
print
q
LimitString
fi

if [ "$1" -eq "2" ];then
mpiexec -np 3 ./dist_hashmap << LimitString | sed -e '1,/nuevapalabrados/d' | sed '/>/d' | sort | diff -u - corpus-post
load corpus
addAndInc nuevapalabrauno
addAndInc nuevapalabrados
print
q
LimitString
fi

if [ "$1" -eq "3" ];then
mpiexec -np 5 ./dist_hashmap << LimitString | sed -e '1,/finalizado/d' | sed '/>/d' | sort | diff -u - corpus-post
load corpus
addAndInc archivo
addAndInc estabilidad
addAndInc funcional
print
q
LimitString
fi