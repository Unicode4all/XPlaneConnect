#ifndef OBSERVER_H
#define OBSERVER_H

class ConfigObserver {
public:
	virtual ~ConfigObserver() {};
	virtual void UpdateConfig() = 0;
};

#endif