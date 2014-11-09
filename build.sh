#!/bin/bash
rm -R bin
mkdir bin
gcc src/server.c -o bin/server