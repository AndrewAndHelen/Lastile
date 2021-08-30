#ifndef LASTILE_H
#define LASTILE_H

#include "lasreader.hpp"
#include "laswriter.hpp"
#include <vector>
#include <list>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cfloat>
#include <string>
#include <iostream>
#include <chrono>
#include <assert.h>
#include <limits.h> 
#define PC_UTILS_BEGIN namespace PC_UTILS {
#define PC_UTILS_END }

PC_UTILS_BEGIN

struct TILE_INFO
{
	double max_x;
	double min_x;
	double max_y;
	double min_y;
	double max_z;
	double min_z;

	double x_scale_factor;
	double y_scale_factor;
	double z_scale_factor;
	double x_offset;
	double y_offset;
	double z_offset;

	unsigned char version_major;                        // starts at byte  24
	unsigned char version_minor;                        // starts at byte  25
	unsigned char point_data_format;                    // starts at byte 104
	unsigned short point_data_record_length;            // starts at byte 105

	std::string lasfile_name;

	TILE_INFO()
	{

	}

	TILE_INFO(const TILE_INFO& other)
	{
		max_x = other.max_x;
		min_x = other.min_x;
		max_y = other.max_y;
		min_y = other.min_y;
		max_z = other.max_z;
		min_z = other.min_z;

		x_scale_factor = other.x_scale_factor;
		y_scale_factor = other.y_scale_factor;
		z_scale_factor = other.z_scale_factor;
		x_offset = other.x_offset;
		y_offset = other.y_offset;
		z_offset = other.z_offset;

		version_major = other.version_major;
		version_minor = other.version_minor;
		point_data_format = other.point_data_format;
		point_data_record_length = other.point_data_record_length;

		lasfile_name = other.lasfile_name;
	}
};

class TILE_TASK
{
public:
	TILE_TASK()
	{

	}

	TILE_TASK(const TILE_TASK& other) :
		_tile_info(other._tile_info), _associate_laslist(other._associate_laslist) { }

	~TILE_TASK()
	{

	}

	TILE_INFO _tile_info;
	std::vector<std::string> _associate_laslist;

	
};
enum PC_FORMAT { LAS, LAZ };

class LASTILE
{
public:
	LASTILE();
	~LASTILE();

	bool divideGrid(const std::string laslist_path,
		double tile_size,
		double buffer,
		std::string output_dir,
		std::string output_prefix,
		PC_FORMAT format,
		std::list<TILE_TASK>& subtile_list,
		double range_min_x = -DBL_MAX / 2,
		double range_min_y = -DBL_MAX / 2,
		double range_max_x = DBL_MAX / 2,
		double range_max_y = DBL_MAX / 2,
		bool keep_xy = false);

	void run(const std::list<TILE_TASK>& subtile_list,
		int thread_num = 4,
		double range_min_z = -DBL_MAX / 2,
		double range_max_z = DBL_MAX / 2,
		bool keep_z = false);

	unsigned short GetFormatRecordLength(unsigned char point_format);

	static bool tileRunningTask(TILE_TASK task, 
		double range_min_z = -DBL_MAX / 2,
		double range_max_z = DBL_MAX / 2,
		bool keep_z = false);

protected:
	bool readLasConfig(const std::string laslist_path,
		std::vector<TILE_INFO>& las_configinfo_list);

	bool isIntersect(const TILE_INFO& tile_a, 
		const TILE_INFO& tile_b);

	int searchIndex(const std::vector<TILE_INFO>& las_configinfo_list,
		const std::string search_lasfile_name);

private:
	static size_t MINIMUM_POINTS_SIZE ;
};

PC_UTILS_END

#endif
