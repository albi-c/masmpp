#!/bin/sh

make release
make cleankrelease
make -f Makefile_win release
