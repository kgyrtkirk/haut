#pragma once
#include <vector>
#include <functional>
#include <string>

#define T_CALL std::function<void(char*, uint8_t*, unsigned int)>
#define T_METRICS_VALUES std::function<String()>

class KMqttClient{
public:
	void	init(const char*_devicePrefix);
	void	loop();
	// FIXME: introduce @/xxx to refer prefix
	void	subscribe(const char*topic,T_CALL callback);
	void	blink();
	void	publishMetric(std::string topic,long value);
};

class KWebServer {
public:
	void init(T_METRICS_VALUES getMetricsValues);
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

