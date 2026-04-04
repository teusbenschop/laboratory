#!/bin/sh

export ANDROID_HOME=~/scr/android-sdk-macosx
export PATH=$PATH:~/scr/android-sdk-macosx/platform-tools:~/scr/android-sdk-macosx/tools:~/scr/android-ndk-r10e
ant release
EXIT_CODE=$?
if [ $EXIT_CODE != 0 ]; then
  exit
fi
