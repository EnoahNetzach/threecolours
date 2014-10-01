#!/bin/bash

for f in $@/*.jpg; do
   $(dirname $0)/__EXE__ -p 45 -i "$f"
   read -p "Continue? " c
   if [[ $c != "" ]]; then
      break
   fi
done