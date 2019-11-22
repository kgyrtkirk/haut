#!/bin/bash

set -e
docker build -t esp-ota .
docker run -p8181:8181 -it -v `pwd`/../:/ws/ --rm esp-ota

