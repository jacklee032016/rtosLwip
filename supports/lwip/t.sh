#!/bin/bash
function copyFiles() {
   arr=("$@")
   for i in "${arr[@]}";
      do
          echo "$i"
      done

}

array=("one" "two" "three")

copyFiles "${array[@]}"

