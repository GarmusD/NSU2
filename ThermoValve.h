#ifndef ThermoValve_h
#define ThermoValve_h

enum ValveType
{
	NC,
	NO
};

struct ThermoValve
{
	ValveType type;
	byte relay_channel;
};


#endif

