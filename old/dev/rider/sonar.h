// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef HCSR04_H_
#define HCSR04_H_




class	Sonar{
public:
	virtual void init() = 0 ;
	virtual long measure()= 0;
};

class	UHSonar : public Sonar{
public:
	UHSonar(int a);
	void	init();
	long measure();
};

class	LoxSonar : public Sonar{
public:
	LoxSonar(int a);
	void	init();
	long measure();
};



#endif
