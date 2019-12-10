
void pre_main(void) __attribute__ ((naked)) __attribute__ ((section (".init8")));

void setup() {
	pre_main();
}

void loop() {
}

