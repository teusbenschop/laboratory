#!/bin/bash

find . -name "*.o" -delete
if [ $? -ne 0 ]; then exit 1; fi

for input_file in mbedtls/*.c; do
  output_file=${input_file%.c}.o
  echo Compile $input_file '->' $output_file
  gcc -I. -c -o $output_file.o $input_file
done

rm -rf mbedtls/.deps
if [ $? -ne 0 ]; then exit 1; fi
rm -rf mbedtls/.dirstamp
if [ $? -ne 0 ]; then exit 1; fi
