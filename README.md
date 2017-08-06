mostly arduino code for my home automation project

more info:
http://rxd.hu

### haut-generic2 (esp)
https://easyeda.com/kirk/haut_generic2-c295f3f180a3433c981de758933fe765

* 4 pcb-s ordered
* one already built and functional
* has some more or less minor issues; it works
* 3d printed box

### haut-generic-v1 board:
https://easyeda.com/kirk/haut_generic1-Qf9rlDVd7

* never built...looked like a dead end because of arduino firmware space limitations


### getting this project to build

...I don't expect anyone else working on this :) but since I sometimes keep 3 month long pauses with this project :)
it might come handy :)

* fetch all the submodules
* install arduino eclipse
* install esp8266
* set this projects libraries to a library path of arduino eclipse
* create a project in sloeber...
* remove the Blink files; and hard link an ino from the project to it
* copy libraries/k-settings.sample.h and adjust settings
* add libraries; possibly install some via sloeber as well; if I'm lazy to add them as submodules


### setup

* run mosquito mqtt https://github.com/toke/docker-mosquitto
* apt-get install mosquitto-clients
* mosquitto_sub -h demeter -v -t '#'
* make sure that ota computes hash-es
  https://github.com/esp8266/Arduino/pull/2236/files#diff-7f9574f87fdcc5b8fce32df1decf313eR178
  (will be in 2.4.0, but 2.3.0 can be patched)
* startup ota server:
  docker build -t esp-ota .
  docker run -p8181:8181 -it -v $HOME/Documents/sloeber-workspace:/ws/ --rm esp-ota
* and the story of GPIO16 and the removal of _PULLUP because it doesnt work with it :D
  change INPUT_PULLUP to PULLUP in DHT.cpp if GPIO16 (esp-gen1)
