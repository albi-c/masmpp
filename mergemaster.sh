#!/bin/sh

git switch master
git merge devel
git switch devel
git push -u origin devel
git push -u origin master
