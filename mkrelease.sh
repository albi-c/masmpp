#!/bin/sh

make release
make clean
make -f Makefile_win release
