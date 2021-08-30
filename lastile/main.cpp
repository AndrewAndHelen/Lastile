#include "lastile.h"
#include "assert.h" 
#ifdef  _WIN32
#include<io.h>
#include<direct.h>
#else defined linux
#include <sys/io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif
static void Usage(const char* pszErrorMsg = NULL)
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "lastile -lof *.txt -o T_.las -tile_size 1000 -buffer 25 -odir subblock -olas -cores 4\n");
	fprintf(stderr, "options:\n");
	fprintf(stderr, "[-help,-h]						[produce help message]\n");
	fprintf(stderr, "[-lof]								[input the list of lasfile]\n");
	fprintf(stderr, "[-o]								[output prefix of lasfile and postfix]\n");
	fprintf(stderr, "[-tile_size]					[the size of tile][defalut 1000]\n");
	fprintf(stderr, "[-buffer]						[the size of buffer][defalut 25]\n");
	fprintf(stderr, "[-odir]							[output lasfile dir]\n");
	fprintf(stderr, "[-olas]							[output file format][defalut olas]\n");
	fprintf(stderr, "[-cores]						[the number of threads][defalut 4]\n");
	fprintf(stderr, "[-keep_xy]					[min_x min_y max_x max_y][defalut don't limit]\n");
	fprintf(stderr, "[-keep_z]						[min_z max_z][defalut don't limit]\n");

	if (pszErrorMsg != NULL)
		fprintf(stderr, "\nFAILURE: %s\n", pszErrorMsg);

	exit(1);
}

int main(int argc, char* argv[])
{
	PC_UTILS::PC_FORMAT output_format = PC_UTILS::PC_FORMAT::LAS;
	
	double tile_size = 500;
	double buffer = 0;
	double range_min_x = -DBL_MAX / 2, range_min_y = -DBL_MAX / 2, range_min_z = -DBL_MAX / 2; 
	double range_max_x = DBL_MAX / 2,  range_max_y = DBL_MAX / 2,  range_max_z = DBL_MAX / 2;

	int threadNum = 4;
	bool keep_xy = false; bool keep_z = false;
	std::string input_dir, output_dir, output_prefix;
	

	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "-h") == 0)
		{
			Usage();
		}
		else if (strcmp(argv[i], "-lof") == 0)
		{
			i++; if (i >= argc) continue;
			input_dir = std::string(argv[i]);
		}
		else if (strcmp(argv[i], "-o") == 0) {
			i++; if (i >= argc) continue;
			std::string temp = argv[i];
			size_t dotPos = temp.find_last_of('.');

			if (dotPos > 0 && dotPos < temp.size())
				output_prefix = temp.substr(0, dotPos);
			else
				output_prefix = temp;
		}
		else if (strcmp(argv[i], "-tile_size") == 0)
		{
			i++; if (i >= argc) continue;
			tile_size = atof(argv[i]);
		}
		else if (strcmp(argv[i], "-buffer") == 0)
		{
			i++; if (i >= argc) continue;
			buffer = atof(argv[i]);
		}
		else if (strcmp(argv[i], "-odir") == 0)
		{
			i++; if (i >= argc) continue;
			output_dir = std::string(argv[i]);
		}
		else if (strcmp(argv[i], "-olas") == 0)
		{
			output_format = PC_UTILS::PC_FORMAT::LAS;
		}
		else if (strcmp(argv[i], "-olaz") == 0)
		{
			output_format = PC_UTILS::PC_FORMAT::LAZ;
		}
		else if (strcmp(argv[i], "-cores") == 0)
		{
			i++; if (i >= argc) continue;
			threadNum = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "-keep_xy") == 0)
		{
			keep_xy = true;
			i++; if (i >= argc) continue;
			range_min_x = atof(argv[i]);
			i++; if (i >= argc) continue;
			range_min_y = atof(argv[i]);
			i++; if (i >= argc) continue;
			range_max_x = atof(argv[i]);
			i++; if (i >= argc) continue;
			range_max_y = atof(argv[i]);
		}
		else if (strcmp(argv[i], "-keep_z") == 0)
		{
			keep_z = true;
			i++; if (i >= argc) continue;
			range_min_z = atof(argv[i]);
			i++; if (i >= argc) continue;
			range_max_z = atof(argv[i]);
		}
		else
		{
			Usage("Too many command options.");
		}
	}

	if (input_dir.empty()) {
		std::cerr << "ERROR:no input las folder!" << "\n";
		return 0;
	}
	else if (output_dir.size() < 1) {
		std::cerr << "ERROR:no output las dir!" << "\n";
		return 0;
	}

#ifdef _WIN32
    if (0 != _access(output_dir.c_str(), 0)) 
		_mkdir(output_dir.c_str());
#else defined linux
    if (0 != eaccess(output_dir.c_str(), F_OK))
        int flag = mkdir(output_dir.c_str(),S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
#endif
	
	std::list<PC_UTILS::TILE_TASK> subtile_list;

	PC_UTILS::LASTILE lastile;

	if (!lastile.divideGrid(input_dir,
		tile_size,
		buffer,
		output_dir,
		output_prefix,
		output_format,
		subtile_list,
		range_min_x,
		range_min_y,
		range_max_x,
		range_max_y,
		keep_xy))
	{
		std::cerr << "ERROR:divide grid happen error!\n";
		return 0;
	}

	lastile.run(subtile_list,
		threadNum,
		range_min_z,
		range_max_z,
		keep_z);

	return 0;
}
