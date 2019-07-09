#pragma once
#include <vector>
#include <functional>

#define T_CALL std::function<void(char*, uint8_t*, unsigned int)>

class KMqttClient{
public:
	void	init(const char*_devicePrefix);
	void	loop();
	void	subscribe(const char*topic,T_CALL callback);
};
