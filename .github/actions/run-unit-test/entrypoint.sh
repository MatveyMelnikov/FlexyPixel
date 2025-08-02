#!/bin/bash

echo "ARM none eabi gcc test docker started"

make TESTS=Y -f MakefileMain.mk clean

make TESTS=Y FOR_TARGET=N -f MakefileMain.mk start
if [ $? -eq 0 ]; then
  echo "unit_tests_result=SUCCESS" >> $GITHUB_OUTPUT
  exit 0
else
  echo "unit_tests_result=FAIL" >> $GITHUB_OUTPUT
  exit 1
fi
