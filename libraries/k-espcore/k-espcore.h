#pragma once
#include <vector>
#include <functional>

#define T_CALL std::function<void(char*, uint8_t*, unsigned int)>

class KMqttClient{
public:
	void	init(const char*_devicePrefix);
	void	loop();
	// FIXME: introduce @/xxx to refer prefix
	void	subscribe(const char*topic,T_CALL callback);
	void	blink();
	void	publishMetric(std::string topic,long value);
};


class KPrometheusService{
public:
	void	init();
	void	loop();
	// FIXME: introduce @/xxx to refer prefix
	void	subscribe(const char*topic,T_CALL callback);
	void	blink();
	void	publishMetric(std::string topic,long value);
};

