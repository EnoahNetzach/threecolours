#!/bin/bash

for f in $@/*.jpg; do
   $(dirname $0)/three_colours -p 45 -i "$f"
   read -p "Continue? " c
   if [[ $c != "" ]]; then
      break
   fi
done