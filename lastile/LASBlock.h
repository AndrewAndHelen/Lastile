#pragma once
#ifndef LASBLOCK_HPP
#define LASBLOCK_HPP
#define _CRT_SECURE_NO_WARNINGS

#include "lasreader.hpp"
#include "laswriter.hpp"
#include "vector"
#include "algorithm"
#include "fstream"
#include "sstream"
#include "cmath"
#include "cfloat"
#include"QtCore/QRunnable"
#include "QtCore/QThreadPool"
#include "QtCore/QProcess"
#include "QtCore/QStringList"
#include "string"
#include "iostream"
#include "chrono"
#include "ctime"

enum POXTFIX { LAS, LAZ };
const size_t MINIMUM_POINT_SIZE = 1000;
struct LASinfo
{
	F64 max_x;
	F64 min_x;
	F64 max_y;
	F64 min_y;
	F64 max_z=0;
	F64 min_z=0;

	F64 x_scale_factor;
	F64 y_scale_factor;
	F64 z_scale_factor;
	F64 x_offset;
	F64 y_offset;
	F64 z_offset;

	U8 version_major;                        // starts at byte  24
	U8 version_minor;                        // starts at byte  25
	U8 point_data_format;                    // starts at byte 104
	U16 point_data_record_length;            // starts at byte 105
};
class LASBlock {
public:
	struct params
	{
		bool keep_xy = false;
		bool keep_z = false;
		double range_min_x = -DBL_MAX/2;
		double range_min_y = -DBL_MAX/2;
		double range_max_x = DBL_MAX/2;
		double range_max_y = DBL_MAX/2;
		double range_min_z = -DBL_MAX/2;
		double range_max_z = DBL_MAX/2;
		POXTFIX poxtfix;
		int ThreadNum;
		double tile_size;
		double buffer_width;
		std::string input_dir;
		std::string output_prefix;
		std::string output_dir;
	};

	LASBlock(){}
	~LASBlock() {}
	void setParam(params param);
	void divide();
	void run();
protected:
	bool isIntersect(LASinfo rectA, LASinfo rectB);
private:
	params BlockParam;
	std::vector<std::vector<std::string>> subtile_contain_laspath;
	std::vector<std::pair<std::string, LASinfo>> LasOriginBoundary_info;
	std::vector<std::pair<std::string, LASinfo>> LasRetainBoundary_info;
};

class TaskRunable :public QRunnable
{
public:
	explicit TaskRunable(int id, LASBlock::params param,std::pair<std::string, LASinfo> subtile_info, std::vector<std::string> tile_contain_laspath) :
		_id(id), _param(param), _subTileInfo(subtile_info), _tileContainLaspath(tile_contain_laspath){}
	~TaskRunable() {}
	void run();

private:
	int _id;
	LASBlock::params _param;
	std::pair<std::string, LASinfo> _subTileInfo;
	std::vector<std::string> _tileContainLaspath;
};
unsigned GetFormatRecordLength(uint8_t pointFormat);

#endif // !LASBLOCK_HPP

