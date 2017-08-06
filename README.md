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

* fetch all the submodules
* install arduino eclipse
* install esp8266
* set this projects libraries to a library path of arduino eclipse
* create a project in sloeber...
* remove the Blink files; and hard link an ino from the project to it
* add libraries; possibly install some via sloeber as well; if I'm lazy to add them as submodules
