#include "lastile.h"
#include "threadpool.h"

PC_UTILS_BEGIN
/*--------------------------implement---------------------------*/

size_t LASTILE::MINIMUM_POINTS_SIZE = 1000;

LASTILE::LASTILE()
{

}

LASTILE::~LASTILE()
{

}

bool LASTILE::divideGrid(const std::string laslist_path,
	double tile_size,
	double buffer,
	std::string output_dir,
	std::string output_prefix,
	PC_FORMAT format,
	std::list<TILE_TASK>& subtile_list,
	double range_min_x,
	double range_min_y,
	double range_max_x,
	double range_max_y,
	bool keep_xy)
{
	std::string suffix = format == LAS ? ".las" : ".laz";
	std::vector<TILE_INFO> las_configinfo_list;

	if (!readLasConfig(laslist_path, las_configinfo_list))
	{
		std::cerr << "ERROR: could not read las config information\n";
		return false;
	}

	std::vector<double> sort_x, sort_y;
	for (auto las_config_info : las_configinfo_list)
	{
		sort_x.emplace_back(las_config_info.min_x);
		sort_x.emplace_back(las_config_info.max_x);
		sort_y.emplace_back(las_config_info.min_y);
		sort_y.emplace_back(las_config_info.max_y);
	}

	std::sort(sort_x.begin(), sort_x.end());
	std::sort(sort_y.begin(), sort_y.end());

	long boundary_minx = std::round(sort_x[0]);
	long boundary_maxx = std::round(sort_x.back());
	long boundary_miny = std::round(sort_y[0]);
	long boundary_maxy = std::round(sort_y.back());

	long integer_minx = boundary_minx - boundary_minx % 1000;
	long integer_miny = boundary_miny - boundary_miny % 1000;
	long x_block_num = std::ceil((boundary_maxx - integer_minx) / tile_size);
	long y_block_num = std::ceil((boundary_maxy - integer_miny) / tile_size);

	for (int i = 0; i < x_block_num; i++)
		for (int j = 0; j < y_block_num; j++)
		{
			TILE_TASK subtile;
			subtile._tile_info.min_x = integer_minx + tile_size * i;
			subtile._tile_info.min_y = integer_miny + tile_size * j;
			subtile._tile_info.max_x = subtile._tile_info.min_x + tile_size;
			subtile._tile_info.max_y = subtile._tile_info.min_y + tile_size;
			subtile_list.emplace_back(subtile);
		}

	auto iterator = subtile_list.begin();

	while (iterator != subtile_list.end())
	{
		TILE_TASK& current_subtile = *iterator;

		for (auto las_config_info : las_configinfo_list)
		{
			if (isIntersect(current_subtile._tile_info, las_config_info))
				current_subtile._associate_laslist.emplace_back(las_config_info.lasfile_name);
		}

		if (current_subtile._associate_laslist.empty())
		{
			subtile_list.erase(iterator++);
		}
		else
		{
			current_subtile._tile_info.min_x -= buffer;
			current_subtile._tile_info.min_y -= buffer;
			current_subtile._tile_info.max_x += buffer;
			current_subtile._tile_info.max_y += buffer;

			std::string search_lasfile_name = current_subtile._associate_laslist[0];

			int index = searchIndex(las_configinfo_list, search_lasfile_name);

			current_subtile._tile_info.x_scale_factor = las_configinfo_list[index].x_scale_factor;
			current_subtile._tile_info.y_scale_factor = las_configinfo_list[index].y_scale_factor;
			current_subtile._tile_info.z_scale_factor = las_configinfo_list[index].z_scale_factor;

			current_subtile._tile_info.x_offset = las_configinfo_list[index].x_offset;
			current_subtile._tile_info.y_offset = las_configinfo_list[index].y_offset;
			current_subtile._tile_info.z_offset = las_configinfo_list[index].z_offset;

			current_subtile._tile_info.version_major = las_configinfo_list[index].version_major;
			current_subtile._tile_info.version_minor = las_configinfo_list[index].version_minor;

			current_subtile._tile_info.point_data_format = las_configinfo_list[index].point_data_format;
			current_subtile._tile_info.point_data_record_length = las_configinfo_list[index].point_data_record_length;


			current_subtile._tile_info.lasfile_name = output_dir + "/" + output_prefix + std::to_string(static_cast<int>(current_subtile._tile_info.min_x)) + "_" +
				std::to_string(static_cast<int>(current_subtile._tile_info.min_y)) + "_" + std::to_string(static_cast<int>(tile_size)) + suffix;

			iterator++;
		}
	}

	if (subtile_list.empty())
	{
		std::cerr << "ERROR:subtile list is empty\n";
		return false;
	}

	if (keep_xy)
	{
		TILE_INFO range_tile;
		range_tile.min_x = range_min_x;
		range_tile.min_y = range_min_y;
		range_tile.max_x = range_max_x;
		range_tile.max_y = range_max_y;

		iterator = subtile_list.begin();

		while (iterator != subtile_list.end())
		{
			TILE_TASK& current_subtile = *iterator;
			if (isIntersect(current_subtile._tile_info, range_tile))
				subtile_list.erase(iterator++);

			iterator++;
		}
	}

	if (subtile_list.empty())
	{
		std::cerr << "ERROR:subtile list is empty\n";
		return false;
	}

	std::cout << "Divide tiles Complete!" << std::endl;
	return true;

}

unsigned short LASTILE::GetFormatRecordLength(unsigned char point_format)
{
	switch (point_format)
	{
	case 0:
		return 20;              //0 - base
	case 1:
		return 20 + 8;          //1 - base + GPS
	case 2:
		return 20 + 6;          //2 - base + RGB
	case 3:
		return 20 + 8 + 6;      //3 - base + GPS + RGB
	case 4:
		return 20 + 8 + 29;     //4 - base + GPS + FWF
	case 5:
		return 20 + 8 + 6 + 29; //5 - base + GPS + FWF + RGB
	case 6:
		return 30;              //6  - base (GPS included)
	case 7:
		return 30 + 6;          //7  - base + RGB
	case 8:
		return 30 + 6 + 2;      //8  - base + RGB + NIR (not used)
	case 9:
		return 30 + 29;         //9  - base + FWF
	case 10:
		return 30 + 6 + 2 + 29; //10 - base + RGB + NIR + FWF
	default:
		assert(false);
		return 0;
	}
}

void LASTILE::run(const std::list<TILE_TASK>& subtile_list,
	int thread_num,
	double range_min_z,
	double range_max_z,
	bool keep_z)
{
	int max_thread_num = std::thread::hardware_concurrency();
	
	if (thread_num<1 || thread_num>max_thread_num)
		thread_num = max_thread_num - 1;

	threadpool pool(thread_num);
	std::vector<std::future<bool>> results;

	auto start_time = std::chrono::high_resolution_clock::now();
	for (auto iterator = subtile_list.cbegin(); iterator != subtile_list.cend(); ++iterator)
	{
		results.emplace_back(pool.commit(std::bind(LASTILE::tileRunningTask, *iterator, range_min_z, range_max_z, keep_z)));
	}
	for (auto&& result : results)
		result.get();

	auto end_time = std::chrono::high_resolution_clock::now();
	auto cost_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	std::cout << "Task Finished! the cost of time is " << static_cast<double>(cost_time.count())/1000 << "s" << std::endl;
}

bool LASTILE::isIntersect(const TILE_INFO& tile_a, const TILE_INFO& tile_b)
{
	TILE_INFO EnvelopeRect;
	EnvelopeRect.min_x = tile_a.min_x < tile_b.min_x ? tile_a.min_x : tile_b.min_x;
	EnvelopeRect.min_y = tile_a.min_y < tile_b.min_y ? tile_a.min_y : tile_b.min_y;
	EnvelopeRect.max_x = tile_a.max_x > tile_b.max_x ? tile_a.max_x : tile_b.max_x;
	EnvelopeRect.max_y = tile_a.max_y > tile_b.max_y ? tile_a.max_y : tile_b.max_y;

	double tilea_width = tile_a.max_x - tile_a.min_x;
	double tilea_height = tile_a.max_y - tile_a.min_y;
	double tileb_width = tile_b.max_x - tile_b.min_x;
	double tileb_height = tile_b.max_y - tile_b.min_y;
	double EnvelopeRect_width = EnvelopeRect.max_x - EnvelopeRect.min_x;
	double EnvelopeRect_height = EnvelopeRect.max_y - EnvelopeRect.min_y;

	if (EnvelopeRect_width <= tilea_width + tileb_width && EnvelopeRect_height <= tilea_height + tileb_height)
		return true;
	else
		return false;
}

bool  LASTILE::readLasConfig(const std::string laslist_path,
	std::vector<TILE_INFO>& las_configinfo_list)
{
	std::vector<std::string> laslist;
	std::ifstream fp(laslist_path);
	if (!fp)
	{
		std::cerr << "ERROR:Couldn't open " << laslist_path;
		return false;
	}

	while (!fp.eof())
	{
		std::string filepath;
		std::getline(fp, filepath);

		if (filepath.empty())
			continue;

		laslist.emplace_back(filepath);
	}
	fp.close();

	if (laslist.empty())
	{
		std::cerr << "ERROR: laslist is empty in" << laslist_path << std::endl;
		return false;
	}

	for (auto filepath : laslist)
	{
		TILE_INFO las_config_info;
		LASreadOpener lasreadopener;
		LASreader* lasreader;

		lasreadopener.set_file_name(filepath.c_str());

		if (!lasreadopener.active())
		{
			std::cerr << "ERROR: could not open " << filepath << std::endl;
			continue;
		}

		lasreader = lasreadopener.open();
		if (!lasreader)
			continue;

		las_config_info.min_x = lasreader->get_min_x();
		las_config_info.min_y = lasreader->get_min_y();
		las_config_info.min_z = lasreader->get_min_z();
		las_config_info.max_x = lasreader->get_max_x();
		las_config_info.max_y = lasreader->get_max_y();
		las_config_info.max_z = lasreader->get_max_z();

		if (las_config_info.min_x > las_config_info.max_x || las_config_info.min_y > las_config_info.max_y)
		{
			std::cerr << "ERROR: invalid boundary information in " << filepath << std::endl;
			continue;
		}

		las_config_info.x_scale_factor = lasreader->header.x_scale_factor;
		las_config_info.y_scale_factor = lasreader->header.y_scale_factor;
		las_config_info.z_scale_factor = lasreader->header.z_scale_factor;
		las_config_info.x_offset = lasreader->header.x_offset;
		las_config_info.y_offset = lasreader->header.y_offset;
		las_config_info.z_offset = lasreader->header.z_offset;

		las_config_info.version_major = lasreader->header.version_major;
		las_config_info.version_minor = lasreader->header.version_minor;
		las_config_info.point_data_format = lasreader->header.point_data_format;

		delete lasreader;

		if (las_config_info.point_data_format < 2) {
			las_config_info.point_data_format += 2;
		}
		else if (las_config_info.point_data_format == 4) {
			las_config_info.point_data_format = 5;
		}

		las_config_info.point_data_record_length = GetFormatRecordLength(las_config_info.point_data_format);
		las_config_info.lasfile_name = filepath;

		las_configinfo_list.emplace_back(las_config_info);
	}

	return true;
}

int LASTILE::searchIndex(const std::vector<TILE_INFO>& las_configinfo_list,
	const std::string search_lasfile_name)
{
	int index = -1;
	for (int i = 0; i < las_configinfo_list.size(); ++i)
	{
		if (las_configinfo_list[i].lasfile_name == search_lasfile_name)
		{
			index = i;
			break;
		}
	}

	return index;
}

bool LASTILE::tileRunningTask(TILE_TASK task,
	double range_min_z,
	double range_max_z,
	bool keep_z)
{
	std::string lastile_path = task._tile_info.lasfile_name;
	int surviving_number_of_point_records = 0;
	unsigned int surviving_number_of_points_by_return[] = { 0,0,0,0,0,0,0,0 };

	LASwriteOpener laswriteopener;
	LASheader            las_header_write;
	LASwriter*           laswriter;
	laswriteopener.set_file_name(lastile_path.c_str());

	if (!laswriteopener.active()) {
		std::cerr << "Error: could not write las file: " << lastile_path << std::endl;
		return false;
	}

	std::string suffix = lastile_path.substr(lastile_path.find_last_of('.'));
	if (suffix == ".laz") {
		laswriteopener.set_format(LAS_TOOLS_FORMAT_LAZ);
	}
	else {
		laswriteopener.set_format(LAS_TOOLS_FORMAT_LAS);
	}

	las_header_write.x_offset = task._tile_info.x_offset;
	las_header_write.y_offset = task._tile_info.y_offset;
	las_header_write.z_offset = task._tile_info.z_offset;
	las_header_write.x_scale_factor = task._tile_info.x_scale_factor;
	las_header_write.y_scale_factor = task._tile_info.y_scale_factor;
	las_header_write.z_scale_factor = task._tile_info.z_scale_factor;

	las_header_write.version_major = task._tile_info.version_major; // 1
	las_header_write.version_minor = task._tile_info.version_minor; // 2

	las_header_write.point_data_format = task._tile_info.point_data_format;
	las_header_write.point_data_record_length = task._tile_info.point_data_record_length;

	laswriter = laswriteopener.open(&las_header_write);
	if (!laswriter) {
		return false;
	}

	LASpoint laspoint_w;
	if (!laspoint_w.init(&las_header_write, las_header_write.point_data_format, las_header_write.point_data_record_length, 0)) {
		return false;
	}
	// write points
	double minX = DBL_MAX, minY = DBL_MAX, minZ = DBL_MAX;
	double maxX = -DBL_MAX, maxY = -DBL_MAX, maxZ = -DBL_MAX;

	for (auto laspath:task._associate_laslist)
	{
		LASreadOpener lasreadopener;
		LASheader     las_header_read;
		LASreader     *lasreader;

		// Set maximum number of points to read
		double x, y, z;
		U8 intensity = 0;
		U16 classification = 0;
		int max_num = INT_MAX;
		int num_read = 0;

		// Open the LAS file
		lasreadopener.set_file_name(laspath.c_str());

		if (!lasreadopener.active()) {
			std::cout << "ERROR: no input specified" << std::endl;
			return false;
		}

		lasreader = lasreadopener.open();
		if (!lasreader) {
			std::cerr << "ERROR: could not open LAS file: " << laspath << std::endl;
			continue;
		}

		las_header_read = lasreader->header;
		las_header_read.user_data_after_header = nullptr;
		U8 point_type = las_header_read.point_data_format;
		U16 point_size = las_header_read.point_data_record_length;

		LASpoint point_r;
		point_r.init(&las_header_read, point_type, point_size, &las_header_read);

		while (lasreader->read_point() && num_read < max_num)
		{
			num_read++;
			point_r = lasreader->point;

			x = point_r.get_x();
			y = point_r.get_y();
			z = point_r.get_z();

			if (keep_z)
			{
				if (z<range_min_z || z>range_max_z)
					continue;
			}

			intensity = point_r.get_intensity();
			classification = point_r.get_classification();

			if ((x >= task._tile_info.min_x) && (x <= task._tile_info.max_x) && (y >= task._tile_info.min_y) && (y <= task._tile_info.max_y))
			{
				if (point_r.return_number - 1 >= 0 && point_r.return_number - 1 < 8)
				{
					surviving_number_of_points_by_return[point_r.return_number - 1]++;
				}
				if (point_r.have_rgb) {
					laspoint_w.set_R(point_r.rgb[0]);
					laspoint_w.set_G(point_r.rgb[1]);
					laspoint_w.set_B(point_r.rgb[2]);
					laspoint_w.set_I(point_r.rgb[3]);
					laspoint_w.have_rgb = TRUE;
				}
				else if (point_r.have_gps_time)
				{
					laspoint_w.set_gps_time(point_r.gps_time);
					laspoint_w.have_gps_time = TRUE;
				}
				else if (point_r.have_nir)
				{
					laspoint_w.set_NIR(point_r.get_NIR());
					laspoint_w.have_nir = TRUE;
				}

				laspoint_w.set_X((x - las_header_write.x_offset) / las_header_write.x_scale_factor);
				laspoint_w.set_Y((y - las_header_write.y_offset) / las_header_write.y_scale_factor);
				laspoint_w.set_Z((z - las_header_write.z_offset) / las_header_write.z_scale_factor);
				laspoint_w.set_intensity(intensity);
				laspoint_w.set_return_number(point_r.return_number);
				laspoint_w.set_number_of_returns(point_r.number_of_returns);
				laspoint_w.set_scan_direction_flag(point_r.get_scan_direction_flag());
				laspoint_w.set_edge_of_flight_line(point_r.get_edge_of_flight_line());
				laspoint_w.set_classification(classification);
				laspoint_w.set_synthetic_flag(point_r.get_synthetic_flag());
				laspoint_w.set_keypoint_flag(point_r.keypoint_flag);
				laspoint_w.set_withheld_flag(point_r.get_withheld_flag());
				laspoint_w.set_scan_angle_rank(point_r.get_scan_angle_rank());
				laspoint_w.set_user_data(point_r.get_user_data());
				laspoint_w.set_point_source_ID(point_r.point_source_ID);
				laspoint_w.set_scan_angle(point_r.get_scan_angle());

				// LAS 1.4 only
				if (las_header_write.version_minor == 4)
				{
					laspoint_w.extended_point_type = point_r.extended_point_type;
					laspoint_w.extended_scanner_channel = point_r.extended_scanner_channel;
					laspoint_w.extended_classification_flags = point_r.extended_classification_flags;
					laspoint_w.extended_return_number = point_r.extended_return_number;
					laspoint_w.extended_number_of_returns = point_r.extended_number_of_returns;
				}

				laswriter->write_point(&laspoint_w);
				laswriter->update_inventory(&laspoint_w);
				surviving_number_of_point_records++;
				// range
				if (x < minX) minX = x;
				if (x > maxX) maxX = x;
				if (y < minY) minY = y;
				if (y > maxY) maxY = y;
				if (z < minZ) minZ = z;
				if (z > maxZ) maxZ = z;
			}
		}

		// Close the LAS file
		lasreader->close();
		delete lasreader;
		lasreader = nullptr;
	}
	// update the boundary
	las_header_write.number_of_point_records = surviving_number_of_point_records;

	for (int i = 0; i < 5; i++)
		las_header_write.number_of_points_by_return[i] = surviving_number_of_points_by_return[i];

	las_header_write.set_bounding_box(minX, minY, minZ, maxX, maxY, maxZ, false, false);

	// update the header
	laswriter->update_header(&las_header_write, true);

	laswriter->close();
	delete laswriter;
	laswriter = nullptr;

	if (surviving_number_of_point_records <= MINIMUM_POINTS_SIZE)
		remove(lastile_path.c_str());
	return true;
}

PC_UTILS_END