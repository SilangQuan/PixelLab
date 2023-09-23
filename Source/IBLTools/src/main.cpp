#include <iostream>
#include "../include/BRDFGenerator.h"
#include "../include/IBLBaker.h"

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
			//./main -lut -s 256 -n 1024 -t 16
			puts("Start GenerateBRDFLUT....Waiting...");
			GenerateBRDFLUT(argc, argv);
		}
		else if (!strcmp(argv[1], "-Irradiance"))
		{
			//./main -Irradiance -f ./BuiltinAssets/texture/skyboxes/barcelona -s 256
			puts("Start Generate Irradiance map....Waiting...");
			//GenerateConvolutionMap(argc, argv);

		}
		else if (!strcmp(argv[1], "-Radiance"))
		{
			//./main -Irradiance -f ./BuiltinAssets/texture/skyboxes/barcelona -s 256
			puts("Start Generate Radiance map....Waiting...");
			//GeneratePrefilterMap(argc, argv);
		}
		else if (!strcmp(argv[1], "-ReflectionData"))
		{
			std::string filepath;
			int customSize;
			//Must have at least 3 arguments to account for filename
			if (argc < 4)
			{
				// Inform the user of how to use the program
				std::cout << "Usage: " << argv[0] << " -f filename <Options>\n"
					<< "Options:\n"
					<< "\t-s SIZE \tThe size of the cube in pixels [size x size]. Default: 256\n"
					<< std::endl;

				exit(0);
			}
			else
			{
				//variables for error checking
				errno = 0;
				char* p;

				/* We will iterate over argv[] to get the parameters stored inside.
				 * Note that we're starting on 1 because we don't need to know the
				 * path of the program, which is stored in argv[0] */
				for (int i = 1; i < argc; i++)
				{
					if (i + 1 != argc)
					{
						if (!strcmp(argv[i], "-f")) {
							filepath = argv[i + 1];
						}
						else if (!strcmp(argv[i], "-s")) {
							customSize = strtol(argv[i + 1], &p, 10);
							if (errno != 0 || *p != '\0' || customSize > INT_MAX || customSize < 0) {
								std::cout << "Invalid size input, should be an integer value greater than 0.\n";
								exit(0);
							}
						}
					}
				}

				if (filepath.empty())
				{
					std::cout << "Must provide filename, please try again.\n";
					exit(0);
				}
			}

			//./main -ReflectionData -f ./BuiltinAssets/texture/skyboxes/barcelona -s 256
			puts("Start Generate Radiance map....Waiting...");
			GenerateReflectionData(customSize, filepath);
		}
	}
	return 1;
}
