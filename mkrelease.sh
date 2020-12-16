#!/bin/sh

./cleanall.sh
make release
make cleankrelease
make -f Makefile_win release
