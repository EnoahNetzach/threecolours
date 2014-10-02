#!/bin/bash

files=(${@:2}/*.jpg)

while true; do
   f=${files[RANDOM % ${#files[@]}]}
   echo "$(dirname $0)/three_colours.exe $1 -i \"$f\""
   $(dirname $0)/__EXE__ $1 -i "$f"
   read -p "Continue? " c
   if [[ $c != "" ]]; then
      break
   fi
done