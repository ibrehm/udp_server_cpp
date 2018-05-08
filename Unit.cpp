#include "Unit.h"

Unit::Unit() {

	c_stats.assign(STAT_total, 0);
	m_stats.assign(STAT_total, 0);

	// Default stats
	x = 0.0f;
	y = 0.0f;
	queue_timer = 0.0f;

	level = 1;
	exp = 0;

	c_stats[STAT_haste] = 0;
	c_stats[STAT_fire_def] = -10;
	c_stats[STAT_ice_def] = 0;
	c_stats[STAT_wind_def] = 0;
	c_stats[STAT_earth_def] = 0;
	c_stats[STAT_lightning_def] = 0;
	c_stats[STAT_water_def] = 0;
	c_stats[STAT_light_def] = 0;
	c_stats[STAT_dark_def] = 0;

	c_stats[STAT_hp] = 150;
	c_stats[STAT_np] = 50;
	c_stats[STAT_str] = 20;
	c_stats[STAT_vit] = 15;
	c_stats[STAT_intel] = 20;
	c_stats[STAT_mnd] = 15;

	c_stats[STAT_atk] = 20;
	c_stats[STAT_def] = 15;
	c_stats[STAT_acc] = 100;
	c_stats[STAT_eva] = 25;

	m_stats[STAT_haste] = 100;
	m_stats[STAT_fire_def] = 100;
	m_stats[STAT_ice_def] = 100;
	m_stats[STAT_wind_def] = 100;
	m_stats[STAT_earth_def] = 100;
	m_stats[STAT_lightning_def] = 100;
	m_stats[STAT_water_def] = 100;
	m_stats[STAT_light_def] = 100;
	m_stats[STAT_dark_def] = 100;

	m_stats[STAT_hp] = 150;
	m_stats[STAT_np] = 50;
	m_stats[STAT_str] = 20;
	m_stats[STAT_vit] = 15;
	m_stats[STAT_intel] = 20;
	m_stats[STAT_mnd] = 15;

	m_stats[STAT_atk] = 20;
	m_stats[STAT_def] = 15;
	m_stats[STAT_acc] = 100;
	m_stats[STAT_eva] = 25;
}
