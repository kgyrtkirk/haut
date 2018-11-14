
#include "Radar.h"
#include "Arduino.h"
#include	"sonar.h"



#ifndef ArduinoSort_h
#define ArduinoSort_h

/**** These are the functions you can use ****/

// Sort an array
template<typename AnyType> void sortArray(AnyType array[], size_t sizeOfArray);

// Sort in reverse
template<typename AnyType> void sortArrayReverse(AnyType array[], size_t sizeOfArray);

// Sort an array with custom comparison function
template<typename AnyType> void sortArray(AnyType array[], size_t sizeOfArray, bool (*largerThan)(AnyType, AnyType));

// Sort in reverse with custom comparison function
template<typename AnyType> void sortArrayReverse(AnyType array[], size_t sizeOfArray, bool (*largerThan)(AnyType, AnyType));





/**** Implementation below. Do not use below functions ****/

namespace ArduinoSort {
	template<typename AnyType> bool builtinLargerThan(AnyType first, AnyType second) {
		return first > second;
	}

	template<> bool builtinLargerThan(char* first, char* second) {
		return strcmp(first, second) > 0;
	}

	template<typename AnyType> void insertionSort(AnyType array[], size_t sizeOfArray, bool reverse, bool (*largerThan)(AnyType, AnyType)) {
		for (size_t i = 1; i < sizeOfArray; i++) {
			for (size_t j = i; j > 0 && (largerThan(array[j-1], array[j]) != reverse); j--) {
				AnyType tmp = array[j-1];
				array[j-1] = array[j];
				array[j] = tmp;
			}
		}
	}
}

template<typename AnyType> void sortArray(AnyType array[], size_t sizeOfArray) {
	ArduinoSort::insertionSort(array, sizeOfArray, false, ArduinoSort::builtinLargerThan);
}

template<typename AnyType> void sortArrayReverse(AnyType array[], size_t sizeOfArray) {
	ArduinoSort::insertionSort(array, sizeOfArray, true, ArduinoSort::builtinLargerThan);
}

template<typename AnyType> void sortArray(AnyType array[], size_t sizeOfArray, bool (*largerThan)(AnyType, AnyType)) {
	ArduinoSort::insertionSort(array, sizeOfArray, false, largerThan);
}

template<typename AnyType> void sortArrayReverse(AnyType array[], size_t sizeOfArray, bool (*largerThan)(AnyType, AnyType)) {
	ArduinoSort::insertionSort(array, sizeOfArray, true, largerThan);
}


#endif


const	uint8_t radar_servo_offset[RADAR_DIR_MAX]={120,90,60};

Radar::Radar(Sonar&_sonar) : sonar(_sonar){
}

void	Radar::attach(int pin){
	servo.attach(pin);
}

#define	K	3
void	Radar::sweep(){
	int	dir=getDirection();
	int	m[K];
	int	s=0;
	for(int i=0;i<K;i++){
		m[i]=sonar.measure();
		s+=m[i];
		delay(2);
	}
	s/=K;

	sortArray(m,3);
	distance[dir]=m[K/2];
	int	s2=0;
	for(int i=0;i<K;i++){
		s2+=(m[i]-s)*(m[i]-s);
	}

	error[dir]=s2/K;
	nextPosition();
}

void	Radar::nextPosition(){
	dir_i++;
	int	dir=getDirection();
	servo.write(radar_servo_offset[dir]);
}
int		Radar::getDirection(){
	int	s=dir_i%(2*RADAR_DIR_MAX-2);
	if(s>=RADAR_DIR_MAX){
		s-=RADAR_DIR_MAX;
		s=(RADAR_DIR_MAX-2)-s;
	}
	return	s;
}

int		Radar::range(RadarDirection dir){
	return	distance[dir];
}

void	Radar::debug(){
	printf("dists:	%d[%d]	%d[%d]	%d[%d]\r\n",distance[LEFT],error[LEFT]
			,distance[HEAD],error[HEAD]
			,distance[RIGHT],error[RIGHT]
			);
}
