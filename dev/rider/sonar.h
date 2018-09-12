// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef HCSR04_H_
#define HCSR04_H_


class	Sonar{
public:

	Sonar(int a);
	void	init();
		long measure();
};

#endif
