#!/bin/bash

# Use Premake and a Makefile to compile
vendor/premake/premake5 gmake
make

echo "Built debug target in ./bin/"