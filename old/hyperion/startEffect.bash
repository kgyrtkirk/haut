#!/bin/bash

#https://github.com/hyperion-project/hyperion/blob/master/src/hyperion-remote/JsonConnection.cpp

echo '{"command":"effect","effect":{"name":"clock1"},"priority":100}'| nc -w1 rpi1.lan 19444
