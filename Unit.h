// (C) 2018 Ian Brehm
//-----------------------------------------------------------------------------------------------------------
#ifndef __Unit_h__
#define __Unit_h__

#include <vector>
#include <string>

enum istat {
	STAT_hp = 0,
	STAT_np,
	STAT_str,
	STAT_vit,
	STAT_intel,
	STAT_mnd,

	STAT_atk,
	STAT_def,
	STAT_acc,
	STAT_eva,

	STAT_haste,
	STAT_fire_def,
	STAT_ice_def,
	STAT_wind_def,
	STAT_earth_def,
	STAT_lightning_def,
	STAT_water_def,
	STAT_light_def,
	STAT_dark_def,

	STAT_total
};

class Item {
public:
	int id;
	int amount;
};

class Unit {

public:
	Unit();
	std::vector<int> c_stats;
	std::vector<int> m_stats;

	std::string name;

	int
		level,
		exp;

	float
		x,
		y,
		queue_timer;
	std::vector<Item> items;
};

#endif // __Unit_h__
