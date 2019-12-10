#!/bin/bash

set -e
docker build -t esp-ota esp-ota
docker run -p8181:8181 -it -v `pwd`/:/ws/ --rm esp-ota

