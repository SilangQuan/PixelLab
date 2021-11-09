#include <iostream>
#include "../include/BRDFGenerator.hpp"

void show_menu() {
	puts("======================================");
	puts("IBLTools v1.0");
	puts("======================================");
}


int main(int argc, char *argv[])
{
	show_menu();
	//Must have at least 3 arguments to account for filename
	if (argc < 3)
	{
		std::cout << "Usage: " << argv[0] << " -lut/-Irradiance/-Radiance <Options>\n";

		// Inform the user of how to use the program
		/*
		std::cout << "Usage: " << argv[0] << " -f filename <Options>\n"
			<< "Options:\n"
			<< "\t-s SIZE \tThe size of the lookup table in pixels [size x size]. Default: 256\n"
			<< "\t-n SAMPLES \tThe number of BRDF samples to integrate per pixel. Default: 1024\n"
			<< "\t-b BITS \tThe number of floating point bits used for texture storage. Can either be 16 or 32. Default: 16\n"
			<< std::endl;
			*/
		exit(0);
	}
	else
	{
		if (!strcmp(argv[1], "-lut"))
		{
			puts("Start GenerateBRDFLUT....Waiting...");
			GenerateBRDFLUT(argc, argv);
		}
		else if (!strcmp(argv[1], "-Irradiance"))
		{
		}
		else if (!strcmp(argv[1], "-Radiance"))
		{

		}
	}
	return 1;
}
