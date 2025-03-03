#!/bin/bash

echo "ARM none eabi gcc test docker started"

make TESTS=N GCC_PATH=/usr/bin/ -f MakefileMain.mk clean

make TESTS=N GCC_PATH=/usr/bin/ -f MakefileMain.mk
if [ $? -eq 0 ]; then
  echo "build_result=SUCCESS" >> $GITHUB_OUTPUT
  exit 0
else
  echo "build_result=FAIL" >> $GITHUB_OUTPUT
  exit 1
fi
