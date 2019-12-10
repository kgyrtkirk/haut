#!/bin/bash

nc -w1 rpi1.lan 19444 <<EOF
{ "color": [0,55,55], "command": "color", "priority": 100, "duration": 10000 }
EOF

