#! /usr/bin/env bash

if ! command -v zip &> /dev/null
then
    echo "This script requires the zip command-line utility to be installed"
    exit 1
fi

zip submission.zip compile run *.cpp *.hpp CMakeLists.txt
