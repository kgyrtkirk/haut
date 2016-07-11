#pragma	once

class Damper{
	float	value;
	float	threshold;
public:
	Damper(float _threshold) : threshold(_threshold){
		value=0;
	}
	const float getValue() const{
		return value;
	}
	void update(float newValue){
		value*=threshold;
		value+=(1.0-threshold)*newValue;
	}
};
